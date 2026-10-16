// uji_hx711.ino  -  uji U6/U7: modul HX711 + load cell CZL 601 80 kg
// Board: "DOIT ESP32 DEVKIT V1" (atau "ESP32 Dev Module"), Arduino-ESP32 core 3.x, Serial Monitor 115200 baud.
// Tanpa library: pembacaan bit-bang sesuai alur §7.1 (24 bit, pulsa ke-25 = kanal A gain 128).
//
// SEBELUM MENYAMBUNG (U6):
//   1. Modul HX711 dicatu 3V3 (BUKAN 5 V), supaya level DT tidak melebihi batas GPIO.
//   2. Ukur E+ terhadap E- pada modul yang menyala; catat nilainya.
//   3. Kabel load cell menurut hasil ukur, BUKAN warna datasheet:
//        eksitasi = hitam dan putih  -> E+ / E-
//        sinyal   = merah dan hijau  -> A+ / A-
//      Polaritas belum diketahui: bila angka turun saat ditekan, tukar dua kabel sinyal (alur §7.5).
// Sambungan: GND-GND, VCC-3V3, DT-GPIO18, SCK-GPIO19.
//
// Perintah Serial: 't' = tare ulang, 'k' = rata-rata 10 sampel mentah, 'p' = cetak status.

#include <Arduino.h>

const uint8_t DT_PIN = 18, SCK_PIN = 19;
const unsigned long T_TUNGGU_HX711_MS = 1000;   // alur §3.2
const uint8_t T_TARE_SAMPEL = 10;               // alur §3.2
const uint8_t SAMPEL_RATA = 5;                  // ukuran buffer plateau, alur §3.1
const float SKALA_COUNTS_PER_KG = 53687.0f;     // nilai awal, alur §3.2 (diganti hasil kalibrasi)
const int32_t ADC_MAX = 8388607;                // alur §3.1
const float SATURASI_FRAKSI = 0.95f;
const uint16_t BATAS_MACET = 20;                // nilai identik berturut-turut dianggap macet

int32_t offsetRaw = 0;
int32_t rawTerakhir = 0;
uint16_t samaBerturut = 0;
unsigned long tungguMaks = 0, tungguTotal = 0;
unsigned long jumlahBaca = 0, jumlahTimeout = 0;

// Menunggu DT turun (data siap). Ini bagian yang memblokir (alur §5.7). Mengembalikan false bila timeout.
bool tungguSiap(unsigned long &lamaMs) {
  unsigned long t0 = millis();
  while (digitalRead(DT_PIN) == HIGH) {
    if (millis() - t0 >= T_TUNGGU_HX711_MS) { lamaMs = millis() - t0; return false; }
    yield();
  }
  lamaMs = millis() - t0;
  return true;
}

// Membaca 24 bit + 1 pulsa. SCK tinggi > 60 us membuat HX711 power-down, jadi interupsi dimatikan sebentar
// (25 pulsa x ~2 us). Lama tunggu DT dicatat untuk temuan A5.
bool bacaRaw(int32_t &raw) {
  unsigned long lama;
  bool siap = tungguSiap(lama);
  if (!siap) { jumlahTimeout++; return false; }
  jumlahBaca++;
  tungguTotal += lama;
  if (lama > tungguMaks) tungguMaks = lama;

  uint32_t v = 0;
  noInterrupts();
  for (uint8_t i = 0; i < 24; i++) {
    digitalWrite(SCK_PIN, HIGH);
    delayMicroseconds(1);
    v = (v << 1) | (uint32_t)digitalRead(DT_PIN);   // MSB lebih dulu
    digitalWrite(SCK_PIN, LOW);
    delayMicroseconds(1);
  }
  digitalWrite(SCK_PIN, HIGH);                       // pulsa ke-25: kanal A, gain 128 untuk bacaan berikutnya
  delayMicroseconds(1);
  digitalWrite(SCK_PIN, LOW);
  interrupts();

  if (v & 0x800000UL) v |= 0xFF000000UL;             // perluasan tanda dua-komplemen (alur §7.1 langkah 4)
  raw = (int32_t)v;
  return true;
}

// Rata-rata n sampel; juga mengembalikan selisih terkecil-terbesar sebagai gambaran noise.
bool rataRata(uint8_t n, float &rata, int32_t &mn, int32_t &mx) {
  int64_t jumlah = 0;
  mn = INT32_MAX; mx = INT32_MIN;
  for (uint8_t i = 0; i < n; i++) {
    int32_t r;
    if (!bacaRaw(r)) return false;
    jumlah += r;
    if (r < mn) mn = r;
    if (r > mx) mx = r;
    if (r == rawTerakhir) samaBerturut++; else samaBerturut = 0;
    rawTerakhir = r;
  }
  rata = (float)jumlah / n;
  return true;
}

void tare() {
  float r; int32_t mn, mx;
  Serial.println(F("Tare: jangan sentuh load cell..."));
  unsigned long t0 = millis();
  if (!rataRata(T_TARE_SAMPEL, r, mn, mx)) {
    Serial.println(F("NO RESPONSE saat tare (HX711_TIMEOUT). Cek catu 3V3, DT=18, SCK=19, GND."));
    return;
  }
  offsetRaw = (int32_t)r;
  Serial.printf("Offset = %ld counts (sebaran %ld), tare %lu ms\n", (long)offsetRaw, (long)(mx - mn), millis() - t0);
}

void status() {
  Serial.printf("Status: baca=%lu timeout=%lu | tunggu DT rata2 %.1f ms, maks %lu ms | offset %ld\n",
                jumlahBaca, jumlahTimeout, jumlahBaca ? (float)tungguTotal / jumlahBaca : 0.0f, tungguMaks,
                (long)offsetRaw);
}

void setup() {
  Serial.begin(115200);
  pinMode(DT_PIN, INPUT_PULLUP);   // DT terlepas terbaca HIGH -> timeout, bukan angka acak (uji X1)
  pinMode(SCK_PIN, OUTPUT);
  digitalWrite(SCK_PIN, LOW);      // SCK harus rendah supaya HX711 tidak power-down
  delay(1500);                      // tunggu sekali di setup()
  Serial.println(F("\n=== U7: UJI HX711 (DT=18, SCK=19, kanal A gain 128, 10 SPS) ==="));
  Serial.println(F("Kolom: raw rata-rata | sebaran 5 sampel | kg taksiran | tunggu DT maks"));
  tare();
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 't' || c == 'T') tare();
    if (c == 'p' || c == 'P') status();
    if (c == 'k' || c == 'K') {
      float r; int32_t mn, mx;
      if (rataRata(10, r, mn, mx)) Serial.printf("k: rata-rata 10 sampel mentah = %.1f (sebaran %ld)\n", r, (long)(mx - mn));
    }
  }

  float r; int32_t mn, mx;
  tungguMaks = 0;
  if (!rataRata(SAMPEL_RATA, r, mn, mx)) {
    Serial.println(F("NO RESPONSE (HX711_TIMEOUT)"));
    return;
  }

  float kg = (r - offsetRaw) / SKALA_COUNTS_PER_KG;
  const char *catatan = "";
  if (fabsf(r) >= SATURASI_FRAKSI * ADC_MAX) catatan = "  << ADC_SATURASI";
  else if (samaBerturut >= BATAS_MACET) catatan = "  << NILAI MACET (nilai identik berulang)";

  Serial.printf("%10.1f | %7ld | %8.3f kg | %3lu ms%s\n", r, (long)(mx - mn), kg, tungguMaks, catatan);
}

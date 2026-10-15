// uji_tombol_led.ino  -  uji U8: tombol tunggal (tap vs tahan) dan LED
// Board: "DOIT ESP32 DEVKIT V1" (atau "ESP32 Dev Module"), Arduino-ESP32 core 3.x, Serial Monitor 115200 baud.
// Mengikuti rancangan alur program: interrupt FALLING dengan debounce 50 ms di dalam ISR (§4.1, §5.3,
// pola Modul 2 GE3), lalu klasifikasi tap vs tahan dari level pin tiap loop (§4.5).
//
// Sambungan: tombol antara GPIO25 dan GND (kaki diagonal), pull-up internal. LED: GPIO27 -> R1 100 ohm -> anoda,
// katoda ke GND.
//
// Cara uji (U8, X8, dan temuan A12):
//   1. Tap 10 kali dengan jelas. Harus tercetak tepat 10 TAP dan "ISR diterima" naik 10.
//   2. Tekan-tahan 10 kali (lebih dari 1,5 s). Harus tercetak 10 TAHAN dan tidak ada TAP tambahan saat dilepas.
//   3. Tekan sangat cepat berulang. Tidak boleh ada TAP yang tidak sesuai jumlah tekanan.
//   "Tepi mentah" menunjukkan berapa tepi FALLING yang masuk ISR sebelum debounce (bukti pantulan).
// Perintah Serial: 'r' = nolkan penghitung.

#include <Arduino.h>

const uint8_t TOMBOL = 25, LED = 27;
const unsigned long T_DEBOUNCE_MS = 50;      // alur §3.1
const unsigned long T_TAHAN_MS = 1500;       // alur §3.2, nilai awal (E4)

enum Tekan { TIDAK = 0, TAP, TAHAN };

// Variabel bersama ISR dan loop: volatile (Modul 2 GE2; White bab 4)
volatile bool flag = false;
volatile unsigned long tsInt = 0;
volatile unsigned long tepiMentah = 0, isrDiterima = 0;

bool sedangTekan = false, tahanDilaporkan = false;
unsigned long tsMulaiTekan = 0;
unsigned long jumlahTap = 0, jumlahTahan = 0;

void IRAM_ATTR isrTombol() {
  unsigned long now = millis();
  tepiMentah = tepiMentah + 1;
  if (now - tsInt > T_DEBOUNCE_MS) {   // debounce di dalam ISR (alur §4.1)
    tsInt = now;
    flag = true;
    isrDiterima = isrDiterima + 1;
  }
}

// alur §5.4: membaca flag dan level pin, mengembalikan TIDAK / TAP / TAHAN.
Tekan tombolUpdate() {
  unsigned long now = millis();

  if (flag && !sedangTekan) {          // tekanan baru dari ISR
    noInterrupts();                    // salin variabel bersama secara atomik (Modul 2 GE2)
    flag = false;                      // hapus sebelum kerja (Modul 2 GE3)
    unsigned long ts = tsInt;
    interrupts();
    sedangTekan = true;
    tahanDilaporkan = false;
    tsMulaiTekan = ts;
    digitalWrite(LED, HIGH);
  }
  if (!sedangTekan) return TIDAK;

  unsigned long lama = now - tsMulaiTekan;
  if (lama < T_DEBOUNCE_MS) return TIDAK;           // level belum dipercaya selama jendela debounce

  bool dilepas = (digitalRead(TOMBOL) == HIGH);      // idle HIGH, ditekan LOW
  if (!tahanDilaporkan && lama >= T_TAHAN_MS) {      // TAHAN dilaporkan sekali, tanpa menunggu lepas
    tahanDilaporkan = true;
    return TAHAN;
  }
  if (dilepas) {
    sedangTekan = false;
    digitalWrite(LED, LOW);
    if (!tahanDilaporkan) return TAP;
  }
  return TIDAK;
}

void tombolReset() {                    // alur §5.5
  noInterrupts();
  flag = false;
  interrupts();
  sedangTekan = false;
  tahanDilaporkan = false;
  tsMulaiTekan = 0;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(TOMBOL, INPUT_PULLUP);
  delay(1500);                          // tunggu sekali di setup()
  tombolReset();
  attachInterrupt(digitalPinToInterrupt(TOMBOL), isrTombol, FALLING);
  Serial.println(F("\n=== U8: UJI TOMBOL (GPIO25) DAN LED (GPIO27) ==="));
  Serial.printf("Debounce ISR %lu ms, ambang tahan %lu ms. LED menyala selama tombol dianggap ditekan.\n",
                T_DEBOUNCE_MS, T_TAHAN_MS);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'r' || c == 'R') {
      noInterrupts();
      tepiMentah = 0; isrDiterima = 0;
      interrupts();
      jumlahTap = 0; jumlahTahan = 0;
      tombolReset();
      digitalWrite(LED, LOW);
      Serial.println(F("Penghitung dinolkan."));
    }
  }

  Tekan t = tombolUpdate();
  if (t == TIDAK) return;

  noInterrupts();
  unsigned long tepi = tepiMentah, diterima = isrDiterima;
  interrupts();

  if (t == TAP) jumlahTap++;
  else jumlahTahan++;
  Serial.printf("%-5s | TAP %lu, TAHAN %lu | ISR diterima %lu, tepi mentah %lu\n",
                t == TAP ? "TAP" : "TAHAN", jumlahTap, jumlahTahan, diterima, tepi);
}

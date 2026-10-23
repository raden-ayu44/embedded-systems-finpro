// sensor.cpp - lihat sensor.h.
// hxRead() diadaptasi dari sketsa uji_bench_hgd (sudah teruji di bench), tanpa library HX711.
// Protokol dari datasheet HX711 (Avia Semiconductor): DT LOW = data siap; 24 pulsa SCK membaca
// 24 bit (MSB dulu); pulsa ke-25 memilih kanal A gain 128 untuk konversi berikutnya;
// SCK HIGH lebih dari ~60 us membuat HX711 masuk mode daya rendah.
#include "sensor.h"

// Status sensor (dokumen: Sensor). Satu instance statis, tanpa malloc.
struct SensorState {
  int32_t  raw;           // counts terakhir
  int32_t  offset;        // counts saat beban nol (hasil tare)
  float    scale;         // counts per kg (langkah 8: perintah x mengubahnya sementara)
  float    kg;            // (raw - offset) / scale
  uint32_t lastSampleMs;  // millis() saat sampel terakhir
  uint32_t readUs;        // lama hxRead() terakhir

  float    buf[BUF_SIZE]; // buffer melingkar kg
  uint8_t  bufIdx;        // posisi tulis berikutnya
  uint8_t  bufN;          // jumlah isi (maks BUF_SIZE)

  bool     taring;        // tare sedang berjalan
  uint8_t  tareN;         // sampel tare yang sudah terkumpul
  int64_t  tareSum;       // jumlah raw selama tare (64 bit: aman dari luapan)
  int32_t  tareMin, tareMax;
};

static SensorState sen;

// Baca satu konversi. HANYA dipanggil bila DT sudah LOW (data siap).
// noInterrupts(): ISR tombol tidak boleh menyela di tengah pulsa, karena SCK yang tertahan HIGH
// > ~60 us mematikan HX711. Di ESP32 noInterrupts() = portDISABLE_INTERRUPTS() (core 2 dan 3),
// berlaku di core tempat loop() berjalan, yang juga core ISR tombol. Interrupt tertunda
// paling lama ~60 us; debounce 50 ms tidak terpengaruh.
static int32_t hxRead() {
  uint32_t v = 0;
  noInterrupts();
  for (uint8_t i = 0; i < 24; i++) {
    digitalWrite(PIN_HX_SCK, HIGH);
    delayMicroseconds(1);                              // SCK HIGH ~1 us, jauh di bawah 60 us
    v = (v << 1) | (uint32_t)digitalRead(PIN_HX_DT);   // bit berikutnya, MSB dulu
    digitalWrite(PIN_HX_SCK, LOW);
    delayMicroseconds(1);
  }
  // Pulsa ke-25: kanal A, gain 128 untuk konversi berikutnya.
  digitalWrite(PIN_HX_SCK, HIGH);
  delayMicroseconds(1);
  digitalWrite(PIN_HX_SCK, LOW);
  delayMicroseconds(1);
  interrupts();

  // Perluasan tanda: 24 bit dua-komplemen -> 32 bit. Bila bit 23 = 1, bilangan negatif.
  if (v & 0x800000UL) v |= 0xFF000000UL;
  return (int32_t)v;
}

static void bufAdd(float kg) {
  sen.buf[sen.bufIdx] = kg;
  sen.bufIdx = (sen.bufIdx + 1) % BUF_SIZE;   // kembali ke 0 setelah posisi terakhir
  if (sen.bufN < BUF_SIZE) sen.bufN++;
}

void sensorInit() {
  memset(&sen, 0, sizeof(sen));
  sen.scale = SCALE_COUNTS_PER_KG;
  // DT pakai pull-up: bila modul tercabut, DT terbaca HIGH = "tidak siap", sehingga tidak ada
  // sampel dan HX711_TIMEOUT terdeteksi (langkah 4), bukan angka acak dari pin mengambang.
  pinMode(PIN_HX_DT, INPUT_PULLUP);
  pinMode(PIN_HX_SCK, OUTPUT);
  digitalWrite(PIN_HX_SCK, LOW);   // SCK LOW = HX711 bekerja normal
  sen.lastSampleMs = millis();     // waktu tunggu HX711 dihitung sejak boot
}

bool sensorPoll() {
  if (digitalRead(PIN_HX_DT) != LOW) return false;   // data belum siap: jangan menunggu

  uint32_t t0 = micros();
  int32_t raw = hxRead();
  sen.readUs = micros() - t0;

  sen.raw = raw;
  sen.lastSampleMs = millis();
  sen.kg = (float)(raw - sen.offset) / sen.scale;
  bufAdd(sen.kg);

  // Tare: kumpulkan TARE_SAMPLES sampel, satu per sampel baru, lalu rata-ratanya jadi offset.
  if (sen.taring) {
    sen.tareSum += raw;
    if (raw < sen.tareMin) sen.tareMin = raw;
    if (raw > sen.tareMax) sen.tareMax = raw;
    sen.tareN++;
    if (sen.tareN >= TARE_SAMPLES) {
      sen.offset = (int32_t)lround((double)sen.tareSum / sen.tareN);
      sen.taring = false;
      sensorBufReset();   // isi buffer lama memakai offset lama: buang
      // Sebaran besar (mis. ratusan ribu counts) = alat tersentuh saat tare: ulangi tare.
      Serial.printf("[TARE] offset=%ld counts (%u sampel, sebaran %ld counts)\n",
                    (long)sen.offset, sen.tareN, (long)(sen.tareMax - sen.tareMin));
    }
  }
  return true;
}

void sensorStartTare() {
  sen.taring = true;
  sen.tareN = 0;
  sen.tareSum = 0;
  sen.tareMin = INT32_MAX;
  sen.tareMax = INT32_MIN;
}

bool sensorTareDone() { return !sen.taring; }

int32_t  sensorRaw()          { return sen.raw; }
int32_t  sensorOffset()       { return sen.offset; }
float    sensorKg()           { return sen.kg; }
uint32_t sensorLastSampleMs() { return sen.lastSampleMs; }
uint32_t sensorReadUs()       { return sen.readUs; }

bool sensorPlateau() {
  if (sen.bufN < BUF_SIZE) return false;   // buffer belum penuh: belum bisa dinilai
  float mn = sen.buf[0], mx = sen.buf[0];
  for (uint8_t i = 1; i < BUF_SIZE; i++) {
    if (sen.buf[i] < mn) mn = sen.buf[i];
    if (sen.buf[i] > mx) mx = sen.buf[i];
  }
  return (mx - mn) <= THRESHOLD_STABLE_KG;
}

float sensorBufAvg() {
  if (sen.bufN == 0) return 0.0f;
  float s = 0.0f;
  for (uint8_t i = 0; i < sen.bufN; i++) s += sen.buf[i];
  return s / sen.bufN;
}

void sensorBufReset() {
  sen.bufIdx = 0;
  sen.bufN = 0;
}

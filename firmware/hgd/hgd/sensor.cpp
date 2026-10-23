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
  uint8_t  sameRun;       // jumlah sampel berturut-turut dengan raw yang sama (termasuk yang terakhir)
  uint8_t  shortGapRun;   // jumlah sampel berturut-turut yang datang < SAMPLE_MIN_GAP_MS setelah sebelumnya
  uint32_t lastGapMs;     // jarak sampel terakhir ke sampel sebelumnya (ms), untuk pesan galat
  uint8_t  bootSkip;      // sisa sampel awal yang dibuang setelah boot

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
// berlaku di core tempat loop() berjalan, yang juga core ISR tombol. Interrupt tertunda selama
// satu pembacaan penuh, terukur ~93-114 us (langkah 3); debounce 50 ms tidak terpengaruh.
// Yang dibatasi 60 us adalah tiap pulsa SCK HIGH (~1-2 us di sini), bukan seluruh pembacaan.
static int32_t hxRead() {
  uint32_t v = 0;
  noInterrupts();
  for (uint8_t i = 0; i < 24; i++) {
    digitalWrite(PIN_HX_SCK, HIGH);
    delayMicroseconds(1);                              // SCK HIGH ~1-2 us, jauh di bawah 60 us
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
  sen.bootSkip = BOOT_SKIP_SAMPLES;
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

  // Sampel awal setelah boot dibuang (HX711 belum stabil, lihat BOOT_SKIP_SAMPLES). Waktu sampel
  // tetap dicatat supaya HX711_TIMEOUT tidak salah terpicu; nilainya tidak dipakai sama sekali.
  if (sen.bootSkip > 0) {
    sen.bootSkip--;
    sen.lastSampleMs = millis();
    return false;
  }

  // Hitung raw identik berturut-turut (untuk READING_INVALID). Dengan noise nyata (sigma ~38 counts)
  // dua sampel berurutan hampir tidak pernah sama persis, apalagi 10.
  if (raw == sen.raw && sen.sameRun > 0) {
    if (sen.sameRun < 255) sen.sameRun++;   // berhenti di 255 supaya tidak meluap
  } else {
    sen.sameRun = 1;
  }
  // Hitung jarak pendek berturut-turut (READING_INVALID utama). Jarak normal ~85 ms; jarak
  // berulang < 40 ms berarti DT tertahan LOW dan yang terbaca bukan hasil konversi.
  uint32_t now = millis();
  sen.lastGapMs = now - sen.lastSampleMs;
  if (sen.lastGapMs < SAMPLE_MIN_GAP_MS) {
    if (sen.shortGapRun < 255) sen.shortGapRun++;
  } else {
    sen.shortGapRun = 0;   // satu jarak normal sudah cukup untuk mengembalikan hitungan
  }

  sen.raw = raw;
  sen.lastSampleMs = now;
  sen.kg = (float)(raw - sen.offset) / sen.scale;
  bufAdd(sen.kg);

  // Tare: kumpulkan TARE_SAMPLES sampel, satu per sampel baru, lalu rata-ratanya jadi offset.
  if (sen.taring) {
    sen.tareSum += raw;
    if (raw < sen.tareMin) sen.tareMin = raw;
    if (raw > sen.tareMax) sen.tareMax = raw;
    sen.tareN++;
    if (sen.tareN >= TARE_SAMPLES) {
      int32_t spread = sen.tareMax - sen.tareMin;
      if (spread > TARE_MAX_SPREAD) {
        // Alat tersentuh atau bergerak saat tare. Offset dari sampel ini akan menggeser semua
        // hasil percobaan, jadi offset lama TIDAK diganti dan tare diulang dari awal.
        // sensorTareDone() tetap false, jadi layar READY tetap "Menyesuaikan nol".
        Serial.printf("[TARE] PERINGATAN: sebaran %ld counts > %ld, jangan sentuh alat; tare diulang\n",
                      (long)spread, (long)TARE_MAX_SPREAD);
        sensorStartTare();
      } else {
        sen.offset = (int32_t)lround((double)sen.tareSum / sen.tareN);
        sen.taring = false;
        sen.scale = SCALE_COUNTS_PER_KG;   // tare membatalkan skala demo dari perintah x
        sen.kg = (float)(raw - sen.offset) / sen.scale;   // kg terakhir dihitung ulang dengan offset baru
        sensorBufReset();   // isi buffer lama memakai offset lama: buang
        Serial.printf("[TARE] offset=%ld counts (%u sampel, sebaran %ld counts)\n",
                      (long)sen.offset, sen.tareN, (long)spread);
      }
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

void sensorDemoScale() {
  // Demo galat X3 dan uji batas 5 s (langkah 8): skala dibagi DEMO_SCALE_DIV sampai tare berikutnya,
  // jadi tekanan kecil terbaca besar (mis. 2 kg x 40 = 80 kg). Load cell tidak perlu dibebani
  // melewati kapasitasnya. kg terakhir dihitung ulang supaya layar langsung memakai skala baru.
  sen.scale = SCALE_COUNTS_PER_KG / DEMO_SCALE_DIV;
  sen.kg = (float)(sen.raw - sen.offset) / sen.scale;
}

float sensorScale() { return sen.scale; }

int32_t  sensorRaw()          { return sen.raw; }
int32_t  sensorOffset()       { return sen.offset; }
float    sensorKg()           { return sen.kg; }
uint32_t sensorLastSampleMs() { return sen.lastSampleMs; }
uint32_t sensorReadUs()       { return sen.readUs; }

FaultType sensorCheck() {
  // 1) Tidak ada sampel baru selama HX711_TIMEOUT_MS: HX711 tidak menjawab
  //    (mis. DT tercabut: pull-up membuat DT selalu HIGH = "tidak siap").
  if (millis() - sen.lastSampleMs > HX711_TIMEOUT_MS) return HX711_TIMEOUT;

  // Belum ada sampel sejak boot: nilai raw belum berarti, tunggu (timeout di atas tetap berlaku).
  if (sen.sameRun == 0) return FAULT_NONE;

  // 2) SHORT_GAP_N sampel berturut-turut datang terlalu cepat: DT tertahan LOW (mis. SCK tercabut),
  //    sehingga "sampel" terbaca di tiap putaran loop() dan isinya sampah (0, 255, 4194300, ...).
  //    Dicek sebelum saturasi: sampah itu kadang mendekati skala penuh, tetapi masalahnya bukan ADC.
  //    Selama DT tertahan, tiap putaran menambah hitungan, jadi galat tetap menyala (tidak berkedip).
  if (sen.shortGapRun >= SHORT_GAP_N) return READING_INVALID;

  // 3) Mendekati batas skala penuh ADC (mis. kabel sinyal tercabut, masukan mengambang).
  //    Dicek sebelum nilai berulang: ADC yang jenuh mengeluarkan kode maksimum yang sama terus.
  if (abs(sen.raw) >= (int32_t)(SATURATION_FRACTION * ADC_MAX)) return ADC_SATURATED;

  // 4) Cadangan: SAME_RAW_N raw identik berturut-turut pada laju normal (data tidak bergeser
  //    walau jarak sampel wajar, mis. DT terhubung ke GND).
  if (sen.sameRun >= SAME_RAW_N) return READING_INVALID;

  return FAULT_NONE;
}

uint8_t  sensorSameRun()     { return sen.sameRun; }
uint8_t  sensorShortGapRun() { return sen.shortGapRun; }
uint32_t sensorLastGapMs()   { return sen.lastGapMs; }

bool sensorBufFull() { return sen.bufN >= BUF_SIZE; }

float sensorBufSpread() {
  if (sen.bufN == 0) return 0.0f;
  float mn = sen.buf[0], mx = sen.buf[0];
  for (uint8_t i = 1; i < sen.bufN; i++) {
    if (sen.buf[i] < mn) mn = sen.buf[i];
    if (sen.buf[i] > mx) mx = sen.buf[i];
  }
  return mx - mn;
}

float sensorBufMin() {
  if (sen.bufN == 0) return 0.0f;
  float mn = sen.buf[0];
  for (uint8_t i = 1; i < sen.bufN; i++) {
    if (sen.buf[i] < mn) mn = sen.buf[i];
  }
  return mn;
}

bool sensorPlateau() {
  if (!sensorBufFull()) return false;   // buffer belum penuh: belum bisa dinilai
  return sensorBufSpread() <= THRESHOLD_STABLE_KG;
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

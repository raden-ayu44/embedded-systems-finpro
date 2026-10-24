// serial_log.cpp - lihat serial_log.h.
#include "serial_log.h"

const uint8_t LOG_LINE_MAX = 96;   // baris terpanjang = penanda M ~75 karakter; sisa untuk aman
const uint8_t LOG_QUEUE_N  = 8;    // 3 T + 1 S + beberapa E / M sudah cukup
const uint8_t IN_LINE_MAX  = 24;   // "ACK,65535" dst.

// Satu baris yang menunggu dikirim / di-ACK.
struct LogEntry {
  char     text[LOG_LINE_MAX];   // baris lengkap termasuk "*CS", tanpa '\n'
  uint16_t seq;
};

// Antrean melingkar (dialokasikan statis, tanpa malloc).
static LogEntry queue[LOG_QUEUE_N];
static uint8_t  qHead = 0;          // indeks baris terdepan (yang sedang dikirim)
static uint8_t  qCount = 0;         // jumlah baris di antrean
static uint16_t nextSeq = 1;        // seq berikutnya sejak boot (1, 2, 3, ...)
static char     bootId[5] = "0000"; // nomor boot 4 digit heksadesimal (logInit)

// Status baris terdepan.
static bool     headSent = false;   // sudah dikirim, menunggu ACK
static uint8_t  headTries = 0;      // jumlah kirim baris ini (1 = pertama, 2 = kirim ulang)
static uint32_t headSentMs = 0;     // millis() saat kiriman terakhir

static uint32_t headSentUs = 0;     // micros() saat kiriman terakhir (ukur waktu balasan ACK)
static uint8_t  batchFails = 0;     // baris gagal sejak logBatchStart()
static bool     batchActive = false; // kelompok ringkasan sedang dikirim (langkah 9b)

// Uji ACK (perintah u) dan baris rusak (perintah r), langkah 10.
const uint16_t ACK_TEST_N = 100;
static uint16_t testToQueue = 0;    // baris uji yang belum masuk antrean
static bool     testActive = false; // uji ACK sedang berjalan
static uint16_t testAcked = 0, testFailed = 0;
static uint32_t testMaxUs = 0, testSumUs = 0;
static bool     corruptNext = false; // kiriman pertama baris berikutnya diberi checksum salah

// Masukan serial yang sedang dikumpulkan (baris ACK/NAK).
static char    inLine[IN_LINE_MAX];
static uint8_t inLen = 0;

// XOR semua byte teks (dari 'H' sampai sebelum '*').
static uint8_t checksum(const char* s) {
  uint8_t cs = 0;
  while (*s) cs ^= (uint8_t)*s++;
  return cs;
}

// Jenis baris di kepala antrean ('T', 'S', 'E', 'U', 'M'). Teks selalu diawali "HGD,<jenis>".
static char headType() { return queue[qHead].text[4]; }

void logInit() {
  // random() di Arduino-ESP32 memakai pembangkit angka acak perangkat keras (esp_random), jadi
  // tiap boot hampir pasti mendapat nomor lain (peluang sama 1 : 65536).
  snprintf(bootId, sizeof(bootId), "%04lX", (unsigned long)random(0x10000));
  Serial.printf("[LOG] boot %s\n", bootId);
}

// Susun satu baris ber-ACK: "HGD,<jenis>,<seq>,<boot>,<fields>", tambah checksum, masukkan ke
// antrean. fields = kolom khusus jenis itu, sudah dipisah koma, mis. "3,2,46.82,0" untuk T.
static uint16_t enqueue(char type, const char* fields) {
  if (qCount >= LOG_QUEUE_N) {
    Serial.println("[LOG] PERINGATAN: antrean penuh, baris dibuang");
    return 0;
  }
  LogEntry& e = queue[(qHead + qCount) % LOG_QUEUE_N];
  e.seq = nextSeq++;
  if (nextSeq == 0) nextSeq = 1;   // seq 0 berarti "tanpa ACK" (baris G); lewati saat meluap
  char body[LOG_LINE_MAX];
  snprintf(body, sizeof(body), "HGD,%c,%u,%s,%s", type, e.seq, bootId, fields);
  snprintf(e.text, sizeof(e.text), "%s*%02X", body, checksum(body));
  qCount++;
  return e.seq;
}

uint16_t logTrial(uint16_t session, uint8_t trial, float kg, bool demo) {
  char f[40];
  snprintf(f, sizeof(f), "%u,%u,%.2f,%u", session, trial, kg, demo ? 1 : 0);
  return enqueue('T', f);
}

uint16_t logSummary(uint16_t session, uint8_t demoCount, float peak, float avg) {
  char f[40];
  snprintf(f, sizeof(f), "%u,%u,%.2f,%.2f", session, demoCount, peak, avg);
  return enqueue('S', f);
}

uint16_t logEvent(uint8_t code, uint16_t session, uint8_t trial, uint32_t secs) {
  char f[40];
  snprintf(f, sizeof(f), "%u,%u,%u,%lu", session, trial, code, (unsigned long)secs);
  return enqueue('E', f);
}

void logGripSample(uint16_t attempt, uint32_t ms, float kg, uint8_t phase, bool demo) {
#if LOG_RAW
  // Dicetak langsung (tanpa antrean): baris ini tidak menunggu ACK. seq = 0 memberi tahu
  // logger.py supaya tidak membalas. Checksum tetap ada, supaya baris yang rusak di jalur USB
  // dibuang logger.py, bukan digambar sebagai data.
  char body[LOG_LINE_MAX];
  snprintf(body, sizeof(body), "HGD,G,0,%s,%u,%lu,%.2f,%u,%u",
           bootId, attempt, (unsigned long)ms, kg, phase, demo ? 1 : 0);
  Serial.printf("%s*%02X\n", body, checksum(body));
#else
  (void)attempt; (void)ms; (void)kg; (void)phase; (void)demo;   // LOG_RAW = 0: tidak dikirim
#endif
}

uint16_t logGripMarker(const GripMarker& m) {
  char f[LOG_LINE_MAX];
  snprintf(f, sizeof(f), "%u,%u,%u,%u,%ld,%ld,%ld,%ld,%.2f,%.2f,%u,%u,%u",
           m.session, m.trial, m.attempt, m.exitBy,
           (long)m.onsetMs, (long)m.endMs, (long)m.winStartMs, (long)m.winEndMs,
           m.ref, m.peak, m.demo ? 1 : 0, m.fault, m.samples);
  return enqueue('M', f);
}

bool logBusy() {
  if (qCount == 0) batchActive = false;
  return qCount > 0;
}

// Buang baris terdepan (sudah ber-ACK atau gagal) dan siapkan baris berikutnya.
static void popHead() {
  qHead = (qHead + 1) % LOG_QUEUE_N;
  qCount--;
  headSent = false;
  headTries = 0;
}

void logBatchStart() {
  // Langkah 10c: penanda M tidak boleh menunda atau menggagalkan baris ringkasan T/S di
  // belakangnya. Penanda dikirim jauh sebelum RINGKASAN (paling lambat di HITUNG percobaan 3,
  // lalu istirahat >= 10 s), dan satu penanda paling lama ~1 s di antrean, jadi biasanya antrean
  // sudah kosong di sini. Bila belum, penanda yang masih di depan dibuang sekarang.
  while (qCount > 0 && headType() == 'M') {
    Serial.printf("[LOG] seq %u (penanda M) dibuang: ringkasan sesi didahulukan\n", queue[qHead].seq);
    popHead();
  }
  batchFails = 0;
  batchActive = true;
}

bool logBatchFailed() { return batchFails > 0; }

void logStartAckTest() {
  testToQueue = ACK_TEST_N;
  testActive = true;
  testAcked = testFailed = 0;
  testMaxUs = testSumUs = 0;
  Serial.printf("[UJI ACK] mulai: %u baris uji U\n", ACK_TEST_N);
}

void logSendCorruptTest() {
  corruptNext = true;
  enqueue('U', "0,0,999,0");
  Serial.println("[UJI NAK] satu baris uji dengan checksum salah pada kiriman pertama");
}

void logCancel() {
  testToQueue = 0;
  testActive = false;
  if (qCount > 0) Serial.printf("[LOG] %u baris dibatalkan\n", qCount);
  qCount = 0;
  headSent = false;
  headTries = 0;
  batchActive = false;
}

static void sendHead() {
  if (corruptNext && headTries == 0) {
    // Uji U10: ubah digit terakhir checksum pada kiriman pertama saja; kiriman ulang memakai teks asli.
    char bad[LOG_LINE_MAX];
    strncpy(bad, queue[qHead].text, sizeof(bad));
    bad[sizeof(bad) - 1] = '\0';
    size_t n = strlen(bad);
    if (n > 0) bad[n - 1] = (bad[n - 1] == '0') ? '1' : '0';
    Serial.println(bad);
    corruptNext = false;
  } else {
    Serial.println(queue[qHead].text);
  }
  headSent = true;
  headTries++;
  headSentMs = millis();
  headSentUs = micros();
}

// Kiriman baris terdepan tidak berhasil (NAK atau batas waktu): kirim ulang sekali, lalu menyerah.
static void headFailed(const char* why) {
  if (headTries < 2) {
    Serial.printf("[LOG] seq %u %s, kirim ulang\n", queue[qHead].seq, why);
    sendHead();
    return;
  }
  uint16_t failedSeq = queue[qHead].seq;   // disimpan dulu: popHead() membuang baris ini
  char failedType = headType();
  Serial.printf("[LOG] seq %u GAGAL setelah kirim ulang (%s)\n", failedSeq, why);
  if (testActive && failedType == 'U') testFailed++;
  popHead();
  // Langkah 10c: penanda M yang gagal (mis. berkas genggaman dibuka di Excel) hanya dibuang.
  // Rekaman genggaman adalah data tambahan; hasil percobaan tetap dikirim lewat baris T/S, jadi
  // penanda tidak pernah dihitung sebagai gagal simpan.
  if (failedType == 'M') {
    Serial.printf("[LOG] PERINGATAN: penanda genggaman seq %u dibuang (data sesi tidak terpengaruh)\n",
                  failedSeq);
    return;
  }
  batchFails++;
  // Langkah 9b: dalam kelompok ringkasan, satu baris yang gagal dua kali berarti logger.py tidak
  // mendengar (tanpa ACK) atau tidak bisa menulis CSV (NAK, mis. berkas dibuka di Excel); sisa
  // baris tidak dicoba lagi (hasilnya pasti sama), supaya GAGAL SIMPAN muncul dalam ~1 s, bukan
  // ~16 s. Nilainya tetap dicetak di layar GAGAL SIMPAN dan di serial.
  // Perbaikan langkah 10 (uji papan): alasan sebenarnya (NAK / tanpa ACK) ikut dicetak, supaya
  // pesan tidak menyesatkan.
  if (batchActive && qCount > 0) {
    Serial.printf("[LOG] %u baris ringkasan lain tidak dikirim (seq %u gagal: %s)\n",
                  qCount, failedSeq, why);
    qCount = 0;
  }
}

// Satu baris masukan lengkap: "ACK,<n>" atau "NAK,<n>". Yang lain diabaikan.
static void handleInputLine() {
  inLine[inLen] = '\0';
  bool isAck = strncmp(inLine, "ACK,", 4) == 0;
  bool isNak = strncmp(inLine, "NAK,", 4) == 0;
  if (!isAck && !isNak) return;
  uint16_t seq = (uint16_t)atoi(inLine + 4);
  // Balasan untuk baris lain (mis. ACK terlambat dari kiriman sebelumnya) diabaikan.
  if (qCount == 0 || !headSent || seq != queue[qHead].seq) return;
  if (isAck) {
    // Waktu kirim -> ACK (kiriman terakhir baris ini). Untuk baris uji U dikumpulkan sebagai statistik.
    uint32_t rttUs = micros() - headSentUs;
    if (testActive && headType() == 'U') {
      testAcked++;
      testSumUs += rttUs;
      if (rttUs > testMaxUs) testMaxUs = rttUs;
    }
    popHead();
  } else {
    headFailed("NAK");
  }
}

char logPoll() {
  char cmd = 0;

  // 1) Masukan: baca semua byte yang sudah ada (tanpa menunggu).
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (inLen > 0) handleInputLine();
      inLen = 0;
    } else if (inLen == 0 && c >= 'a' && c <= 'z') {
      // Perintah debug satu huruf kecil di awal baris (ACK/NAK selalu huruf besar).
      cmd = c;
    } else if (inLen < IN_LINE_MAX - 1) {
      inLine[inLen++] = c;
    } else {
      inLen = 0;   // terlalu panjang: bukan ACK/NAK, buang
    }
  }

  // 2) Uji ACK: isi antrean dengan baris uji selama masih ada tempat; ringkasan setelah selesai.
  while (testToQueue > 0 && qCount < LOG_QUEUE_N) {
    char f[24];
    snprintf(f, sizeof(f), "0,0,%u,0", (unsigned)(ACK_TEST_N - testToQueue + 1));
    enqueue('U', f);
    testToQueue--;
  }
  if (testActive && testToQueue == 0 && qCount == 0) {
    testActive = false;
    uint16_t n = testAcked + testFailed;
    Serial.printf("[UJI ACK] %u/%u ACK, terlama %.1f ms, rata-rata %.1f ms, gagal %u\n",
                  testAcked, n, testMaxUs / 1000.0f,
                  testAcked ? (testSumUs / 1000.0f) / testAcked : 0.0f, testFailed);
  }

  // 3) Keluaran: kirim baris terdepan, atau cek batas waktu ACK.
  if (qCount > 0) {
    if (!headSent) sendHead();
    else if (millis() - headSentMs >= ACK_TIMEOUT_MS) headFailed("tanpa ACK");
  }
  return cmd;
}

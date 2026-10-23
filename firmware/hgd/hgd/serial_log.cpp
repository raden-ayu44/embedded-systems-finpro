// serial_log.cpp - lihat serial_log.h.
#include "serial_log.h"

const uint8_t LOG_LINE_MAX = 64;   // baris terpanjang ~40 karakter; sisa untuk aman
const uint8_t LOG_QUEUE_N  = 8;    // 3 T + 1 S + beberapa E sudah cukup
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

// Status baris terdepan.
static bool     headSent = false;   // sudah dikirim, menunggu ACK
static uint8_t  headTries = 0;      // jumlah kirim baris ini (1 = pertama, 2 = kirim ulang)
static uint32_t headSentMs = 0;     // millis() saat kiriman terakhir

static uint8_t  batchFails = 0;     // baris gagal sejak logBatchStart()
static bool     batchActive = false; // kelompok ringkasan sedang dikirim (langkah 9b)

// Masukan serial yang sedang dikumpulkan (baris ACK/NAK).
static char    inLine[IN_LINE_MAX];
static uint8_t inLen = 0;

// XOR semua byte teks (dari 'H' sampai sebelum '*').
static uint8_t checksum(const char* s) {
  uint8_t cs = 0;
  while (*s) cs ^= (uint8_t)*s++;
  return cs;
}

// Susun satu baris dari bagian isi (tanpa "HGD," dan seq), tambah checksum, masukkan ke antrean.
// body contoh: "T", "3,2", "46.82", "0"  -> "HGD,T,<seq>,3,2,46.82,0*CS"
static uint16_t enqueue(char type, uint16_t session, uint8_t trial, const char* v1, const char* v2) {
  if (qCount >= LOG_QUEUE_N) {
    Serial.println("[LOG] PERINGATAN: antrean penuh, baris dibuang");
    return 0;
  }
  LogEntry& e = queue[(qHead + qCount) % LOG_QUEUE_N];
  e.seq = nextSeq++;
  if (nextSeq == 0) nextSeq = 1;   // seq 0 dipakai sebagai "tidak ada"; lewati saat meluap
  char body[LOG_LINE_MAX];
  snprintf(body, sizeof(body), "HGD,%c,%u,%u,%u,%s,%s", type, e.seq, session, trial, v1, v2);
  snprintf(e.text, sizeof(e.text), "%s*%02X", body, checksum(body));
  qCount++;
  return e.seq;
}

uint16_t logTrial(uint16_t session, uint8_t trial, float kg, bool demo) {
  char v1[16];
  snprintf(v1, sizeof(v1), "%.2f", kg);
  return enqueue('T', session, trial, v1, demo ? "1" : "0");
}

uint16_t logSummary(uint16_t session, uint8_t demoCount, float peak, float avg) {
  char v1[16], v2[16];
  snprintf(v1, sizeof(v1), "%.2f", peak);
  snprintf(v2, sizeof(v2), "%.2f", avg);
  return enqueue('S', session, demoCount, v1, v2);
}

uint16_t logEvent(uint8_t code, uint16_t session, uint8_t trial, uint32_t secs) {
  char v1[8], v2[12];
  snprintf(v1, sizeof(v1), "%u", code);
  snprintf(v2, sizeof(v2), "%lu", (unsigned long)secs);
  return enqueue('E', session, trial, v1, v2);
}

bool logBusy() {
  if (qCount == 0) batchActive = false;
  return qCount > 0;
}

void logBatchStart() { batchFails = 0; batchActive = true; }
bool logBatchFailed() { return batchFails > 0; }

void logCancel() {
  if (qCount > 0) Serial.printf("[LOG] %u baris dibatalkan\n", qCount);
  qCount = 0;
  headSent = false;
  headTries = 0;
  batchActive = false;
}

// Buang baris terdepan (sudah ber-ACK atau gagal) dan siapkan baris berikutnya.
static void popHead() {
  qHead = (qHead + 1) % LOG_QUEUE_N;
  qCount--;
  headSent = false;
  headTries = 0;
}

static void sendHead() {
  Serial.println(queue[qHead].text);
  headSent = true;
  headTries++;
  headSentMs = millis();
}

// Kiriman baris terdepan tidak berhasil (NAK atau batas waktu): kirim ulang sekali, lalu menyerah.
static void headFailed(const char* why) {
  if (headTries < 2) {
    Serial.printf("[LOG] seq %u %s, kirim ulang\n", queue[qHead].seq, why);
    sendHead();
  } else {
    Serial.printf("[LOG] seq %u GAGAL setelah kirim ulang (%s)\n", queue[qHead].seq, why);
    batchFails++;
    popHead();
    // Langkah 9b: dalam kelompok ringkasan, satu baris yang gagal dua kali berarti logger.py tidak
    // mendengar; sisa baris tidak dicoba lagi (hasilnya pasti sama), supaya GAGAL SIMPAN muncul
    // dalam ~1,5 s, bukan ~16 s. Nilainya tetap dicetak di layar GAGAL SIMPAN dan di serial.
    if (batchActive && qCount > 0) {
      Serial.printf("[LOG] %u baris ringkasan lain tidak dikirim (logger.py tidak menjawab)\n", qCount);
      qCount = 0;
    }
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
  if (isAck) popHead();
  else       headFailed("NAK");
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

  // 2) Keluaran: kirim baris terdepan, atau cek batas waktu ACK.
  if (qCount > 0) {
    if (!headSent) sendHead();
    else if (millis() - headSentMs >= ACK_TIMEOUT_MS) headFailed("tanpa ACK");
  }
  return cmd;
}

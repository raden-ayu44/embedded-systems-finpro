// serial_log.h - baris log HGD ke logger.py, checksum, dan ACK (langkah 9, 10c).
// Alur dan alasan: alur-program-hgd.md Bagian 4.8, 5.15, 5.16 dan 9.
//
// Semua baris (115200 baud, diakhiri '\n') diawali bagian yang sama:
//   HGD,<jenis>,<seq>,<boot>,...*<checksum>
// - seq  = nomor urut baris sejak boot (1, 2, 3, ...); logger.py membalas "ACK,<seq>" / "NAK,<seq>".
//          seq 0 = baris tanpa ACK (baris G).
// - boot = 4 digit heksadesimal acak yang dipilih sekali saat ESP32 menyala (langkah 10c). Setelah
//          reset, seq dan nomor sesi mulai lagi dari 1, jadi baris sesudah reset bisa sama persis
//          dengan baris sebelum reset. Tanpa boot, logger.py mengira baris itu kiriman ulang
//          (duplikat): membalas ACK tetapi TIDAK menulisnya. Dengan boot, keduanya berbeda.
// - checksum = XOR semua byte dari 'H' sampai tepat sebelum '*', dua digit heksadesimal huruf besar.
//
// Jenis baris dan kolom sesudah boot:
// - T  sesi,percobaan,kg,demo           satu percobaan (demo 1 = diukur dengan skala demo x)
// - S  sesi,jumlah_demo,puncak,rerata   ringkasan sesi (jumlah_demo 0 = semua data nyata)
// - E  sesi,percobaan,kode,detik        galat 1-4 atau reset total 5; detik sejak sesi mulai
// - U  0,0,nomor,0                      baris uji ACK (perintah u / r), tidak ditulis ke CSV
// - G  upaya,ms,kg,fase,demo            satu sampel HX711 selama GRIP, TANPA ACK (bila LOG_RAW 1)
// - M  sesi,percobaan,upaya,keluar,onset_ms,akhir_ms,jendela_awal_ms,jendela_akhir_ms,ref,puncak,
//      demo,galat,n                     penanda satu upaya genggam, DENGAN ACK
// Arti tiap kolom G dan M: lihat logGripSample() dan logGripMarker() di bawah.
//
// Pengiriman baris ber-ACK: masuk antrean dan dikirim SATU per SATU. Tiap baris menunggu
// "ACK,<seq>" paling lama ACK_TIMEOUT_MS (500 ms). NAK atau tanpa balasan -> kirim ulang sekali;
// gagal lagi -> baris itu dihitung gagal. Di dalam kelompok ringkasan, sisa baris lalu dibuang
// (logger.py jelas tidak menjawab), jadi GAGAL SIMPAN muncul dalam ~1 s. Penanda M yang gagal
// hanya dibuang dengan peringatan: tidak pernah menyebabkan GAGAL SIMPAN. Tidak ada penungguan di
// dalam fungsi: logPoll() dipanggil tiap loop() dan langsung kembali.
//
// Teks debug lain (mis. "[FSM] ...") tidak diawali "HGD", jadi diabaikan logger.py.
#pragma once
#include "types.h"

// Pilih nomor boot acak dan cetak "[LOG] boot XXXX". Panggil sekali di setup(), sebelum baris
// log pertama.
void logInit();

// Panggil sekali per loop(). Membaca masukan serial tanpa menunggu:
// - baris "ACK,<n>" / "NAK,<n>" dipakai untuk antrean;
// - satu huruf kecil perintah debug (t, p, x, s, u, r) dikembalikan ke pemanggil;
// lalu mengurus kirim / tunggu / kirim ulang baris di kepala antrean.
// Mengembalikan huruf perintah, atau 0 bila tidak ada.
char logPoll();

// Masukkan baris ke antrean. Mengembalikan seq yang diberikan (0 bila antrean penuh).
uint16_t logTrial(uint16_t session, uint8_t trial, float kg, bool demo);
uint16_t logSummary(uint16_t session, uint8_t demoCount, float peak, float avg);
uint16_t logEvent(uint8_t code, uint16_t session, uint8_t trial, uint32_t secs);

// Baris G (langkah 10c): satu sampel HX711 selama GRIP, langsung dicetak tanpa antrean dan tanpa
// ACK (~11,6 baris/s; menunggu ACK untuk tiap sampel terlalu lambat, dan satu sampel yang hilang
// tidak merusak hasil percobaan). Kolom:
// - attempt = nomor upaya genggam sejak boot (sama dengan kolom upaya pada penanda M);
// - ms      = waktu sampel sejak upaya mulai (masuk GRIP atau sesudah dilepas terlalu awal);
// - kg      = gaya sampel itu;
// - phase   = 0 tare sedang berjalan (nol belum siap), 1 menunggu onset, 2 mengukur;
// - demo    = 1 bila skala demo x aktif saat sampel ini (bukan data nyata).
// Hanya mencetak bila LOG_RAW = 1 di config.h.
void logGripSample(uint16_t attempt, uint32_t ms, float kg, uint8_t phase, bool demo);

// Penanda M (langkah 10c): ringkasan satu upaya genggam, masuk antrean DENGAN ACK, supaya
// logger.py tahu kapan semua baris G upaya itu lengkap dan boleh ditulis. Semua waktu dalam ms
// sejak upaya mulai (sama dengan kolom ms baris G); -1 = tidak ada (mis. belum onset).
struct GripMarker {
  uint16_t session, attempt;
  uint8_t  trial;
  uint8_t  exitBy;        // GripExit (1-6), types.h
  int32_t  onsetMs;       // sampel pertama di atas onset (waktu nol grafik)
  int32_t  endMs;         // saat upaya berakhir
  int32_t  winStartMs;    // jendela 5 sampel yang dipakai untuk ref: sampel pertama dan terakhir
  int32_t  winEndMs;
  float    ref;           // hasil percobaan (kg); -1 bila upaya tidak menghasilkan ref
  float    peak;          // sampel tertinggi di fase 2 (kg); -1 bila belum onset
  bool     demo;          // ada sampel upaya ini yang diukur dengan skala demo x
  uint8_t  fault;         // FaultType 1-4 bila upaya diputus galat (termasuk ref di luar rentang)
  uint16_t samples;       // jumlah baris G yang dikirim untuk upaya ini (0 bila LOG_RAW = 0)
};
uint16_t logGripMarker(const GripMarker& m);

// true selama masih ada baris yang dikirim atau menunggu ACK.
bool logBusy();

// Kelompok kiriman (ringkasan sesi): logBatchStart() menolkan hitungan gagal; setelah logBusy()
// false, logBatchFailed() = true bila ada baris kelompok itu yang gagal setelah kirim ulang.
void logBatchStart();
bool logBatchFailed();

// Batalkan semua baris yang belum ber-ACK (reset total, Bagian 9.5 butir 5).
void logCancel();

// Uji U9 / E5 (langkah 10, perintah u): kirim 100 baris uji jenis U (logger.py membalas ACK tetapi
// tidak menulisnya ke CSV), ukur waktu kirim -> ACK tiap baris, lalu cetak ringkasan
// "[UJI ACK] n/100 ACK, terlama .. ms, rata-rata .. ms".
void logStartAckTest();

// Uji U10 (langkah 10, perintah r): kirim satu baris uji U yang checksum-nya sengaja salah pada
// kiriman pertama; logger.py harus membalas NAK, lalu kiriman ulang (benar) di-ACK.
void logSendCorruptTest();

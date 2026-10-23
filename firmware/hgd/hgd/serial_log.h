// serial_log.h - baris log HGD ke logger.py, checksum, dan ACK (langkah 9).
// Alur dan alasan: alur-program-hgd.md Bagian 4.8, 5.15, 5.16 dan 9.
//
// Format satu baris (115200 baud, diakhiri '\n'):
//   HGD,<jenis>,<seq>,<sesi>,<percobaan>,<nilai1>,<nilai2>*<checksum>
// - jenis T = satu percobaan (nilai1 = kg; nilai2 = 1 bila diukur dengan skala demo x, selain itu 0)
// - jenis S = ringkasan sesi (nilai1 = puncak, nilai2 = rerata; percobaan = jumlah percobaan demo,
//   0 = semua data nyata)
// - jenis E = galat atau kejadian (nilai1 = kode 1-5, nilai2 = detik sejak sesi mulai)
// - checksum = XOR semua byte dari 'H' sampai tepat sebelum '*', dua digit heksadesimal huruf besar.
//
// Pengiriman: baris masuk antrean dan dikirim SATU per SATU. Tiap baris menunggu "ACK,<seq>"
// paling lama ACK_TIMEOUT_MS (750 ms). NAK atau tanpa balasan -> kirim ulang sekali; gagal lagi ->
// baris itu dihitung gagal. Di dalam kelompok ringkasan, sisa baris lalu dibuang (logger.py jelas
// tidak menjawab), jadi GAGAL SIMPAN muncul dalam ~1,5 s. Tidak ada penungguan di dalam fungsi:
// logPoll() dipanggil tiap loop() dan langsung kembali.
//
// Teks debug lain (mis. "[FSM] ...") tidak diawali "HGD", jadi diabaikan logger.py.
#pragma once
#include "types.h"

// Panggil sekali per loop(). Membaca masukan serial tanpa menunggu:
// - baris "ACK,<n>" / "NAK,<n>" dipakai untuk antrean;
// - satu huruf kecil perintah debug (t, p, x, s) dikembalikan ke pemanggil;
// lalu mengurus kirim / tunggu / kirim ulang baris di kepala antrean.
// Mengembalikan huruf perintah, atau 0 bila tidak ada.
char logPoll();

// Masukkan baris ke antrean. Mengembalikan seq yang diberikan (0 bila antrean penuh).
uint16_t logTrial(uint16_t session, uint8_t trial, float kg, bool demo);
uint16_t logSummary(uint16_t session, uint8_t demoCount, float peak, float avg);
uint16_t logEvent(uint8_t code, uint16_t session, uint8_t trial, uint32_t secs);

// true selama masih ada baris yang dikirim atau menunggu ACK.
bool logBusy();

// Kelompok kiriman (ringkasan sesi): logBatchStart() menolkan hitungan gagal; setelah logBusy()
// false, logBatchFailed() = true bila ada baris kelompok itu yang gagal setelah kirim ulang.
void logBatchStart();
bool logBatchFailed();

// Batalkan semua baris yang belum ber-ACK (reset total, Bagian 9.5 butir 5).
void logCancel();

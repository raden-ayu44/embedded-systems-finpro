// display.h - LCD 16x2 lewat backpack I2C (PCF8574T, alamat 0x27).
// Alur dan alasan: alur-program-hgd.md Bagian 5.3, 5.18 dan 10.1-10.2.
// Langkah 2 berisi fungsi dasar saja; layar per state (displayShow) ditambahkan di langkah 5.
//
// Cara kerja singkat:
// - lcdLine() TIDAK menulis ke LCD; ia hanya mengisi buffer "isi yang diinginkan".
// - displayUpdate() (sekali per loop) membandingkan buffer itu dengan buffer "isi yang tampil"
//   dan hanya mengirim karakter yang berbeda, paling banyak SATU baris per panggilan.
//   Alasan: satu karakter lewat backpack I2C butuh ~1,7 ms (diukur: satu baris penuh 27,7 ms).
//   Dengan cara ini satu putaran loop() tidak pernah memuat lebih dari satu baris, jadi
//   tombol tetap terbaca (lihat Bagian 5.3).
#pragma once
#include "types.h"

// Inisialisasi LCD. Panggil sekali di setup(), SETELAH Wire.begin(21, 22) dan Wire.setClock().
// Mengembalikan true bila backpack menjawab di LCD_ADDR. Bila false, program tetap jalan
// (tulisan ke LCD gagal tanpa suara); pesan peringatan dicetak ke Serial.
bool displayInit();

// Siapkan isi satu baris (row 0 atau 1). Teks dipotong di 16 karakter dan diisi spasi sampai
// tepat 16, jadi sisa teks lama selalu tertimpa. Belum ada yang dikirim ke LCD.
void lcdLine(uint8_t row, const char* text);

// Panggil sekali per loop(). Mengirim karakter yang berubah dari paling banyak satu baris.
// Tiap LCD_HEAL_MS satu baris (bergantian) dikirim ulang penuh untuk pulih dari gangguan I2C.
// Mengembalikan jumlah karakter yang dikirim (0 = layar sudah sesuai, tidak ada transaksi I2C).
uint8_t displayUpdate();

// Pembatas refresh: true bila sudah >= LCD_REFRESH_MS sejak redraw terakhir, atau bila
// displayRequest() dipanggil (layar berganti). Saat true, waktu redraw dicatat ulang.
// Pola pakai: if (displayDue()) { lcdLine(0, ...); lcdLine(1, ...); }
bool displayDue();

// Minta redraw pada panggilan displayDue() berikutnya tanpa menunggu LCD_REFRESH_MS.
// Dipakai saat layar berganti (mis. masuk state baru), supaya pengguna tidak menunggu 0,5 s.
void displayRequest();

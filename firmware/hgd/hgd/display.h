// display.h - LCD 16x2 lewat backpack I2C (PCF8574T, alamat 0x27).
// Alur dan alasan: alur-program-hgd.md Bagian 5.18 dan 10.1-10.2.
// Langkah 2 berisi fungsi dasar saja; layar per state (displayShow) ditambahkan di langkah 5.
#pragma once
#include "types.h"

// Inisialisasi LCD. Panggil sekali di setup(), SETELAH Wire.begin(21, 22) dan Wire.setClock().
// Mengembalikan true bila backpack menjawab di LCD_ADDR. Bila false, program tetap jalan
// (tulisan ke LCD gagal tanpa suara); pesan peringatan dicetak ke Serial.
bool displayInit();

// Tulis satu baris (row 0 atau 1). Teks dipotong di 16 karakter dan diisi spasi sampai tepat 16,
// jadi sisa teks lama selalu tertimpa. Bila isi baris sama dengan yang terakhir ditulis,
// tidak ada yang dikirim ke LCD (menghemat waktu bus I2C).
void lcdLine(uint8_t row, const char* text);

// Pembatas refresh: true bila sudah >= LCD_REFRESH_MS sejak redraw terakhir, atau bila
// displayRequest() dipanggil (layar berganti). Saat true, waktu redraw dicatat ulang.
// Pola pakai: if (displayDue()) { lcdLine(0, ...); lcdLine(1, ...); }
bool displayDue();

// Minta redraw pada panggilan displayDue() berikutnya tanpa menunggu LCD_REFRESH_MS.
// Dipakai saat layar berganti (mis. masuk state baru), supaya pengguna tidak menunggu 0,5 s.
void displayRequest();

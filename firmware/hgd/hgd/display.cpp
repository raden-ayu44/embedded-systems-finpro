// display.cpp - lihat display.h.
//
// Fakta LCD dari datasheet HS1602A (Shenzhen Hansheng Industrial Co., Ltd., Ver. 20110521),
// pengontrol SPLC780 yang kompatibel HD44780:
// - 16 karakter tampak per baris. Baris 1 = DDRAM 0x00-0x0F, baris 2 mulai di DDRAM 0x40.
//   DDRAM tiap baris lebih panjang dari 16, jadi karakter ke-17 dst. ditulis ke alamat yang
//   tidak tampak, BUKAN pindah ke baris 2. Karena itu teks selalu dipotong di 16 karakter.
// - Font tetap 5x8 titik per karakter (tidak ada ukuran lain).
// - Perintah Clear butuh 1,64 ms (perintah biasa 42 us). Clear juga mengosongkan layar sesaat
//   sehingga tampak berkedip. Karena itu redraw TIDAK memakai lcd.clear(); baris selalu
//   ditimpa penuh 16 karakter (diisi spasi), yang juga menghapus digit sisa angka yang memendek.
// - Hanya ASCII cetak (0x20-0x7E) yang dikirim. Karakter lain (mis. huruf UTF-8 seperti derajat)
//   terdiri dari beberapa byte dan tampil sebagai simbol acak di ROM LCD. Hindari juga '\' dan '~':
//   pada ROM HD44780 versi Jepang (A00) keduanya tampil sebagai simbol yen dan panah.
//
// Library: LiquidCrystal_I2C 1.1.4 (Frank de Brabander, dirawat Marco Schwartz), API init().

#include "display.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;

// Objek LCD global dengan alamat tetap 0x27 (dialokasikan statis, tanpa new).
static LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// Isi terakhir tiap baris yang sudah dikirim ke LCD (+1 untuk penutup string '\0').
// Dipakai lcdLine() untuk melewati baris yang isinya tidak berubah.
static char shown[LCD_ROWS][LCD_COLS + 1];

static uint32_t lastDrawMs = 0;     // millis() saat redraw terakhir yang diizinkan displayDue()
static bool     drawRequested = true;   // true = redraw berikutnya tidak perlu menunggu

bool displayInit() {
  // Cek dulu apakah backpack menjawab di alamatnya (alamat diuji di U4).
  Wire.beginTransmission(LCD_ADDR);
  bool found = (Wire.endTransmission() == 0);
  if (!found) {
    Serial.printf("[LCD] PERINGATAN: tidak ada jawaban di 0x%02X. Cek SDA/SCL, catu VIN 5 V, GND.\n",
                  LCD_ADDR);
  }

  // init() memanggil Wire.begin() lagi; karena bus sudah dimulai di setup() dengan pin 21/22,
  // panggilan itu diabaikan core (pin dan clock tetap). init() memblokir ~1 s: hanya di setup().
  lcd.init();
  lcd.backlight();

  // init() sudah mengosongkan layar. Cache dikosongkan supaya tulisan pertama pasti dikirim.
  memset(shown, 0, sizeof(shown));
  drawRequested = true;
  return found;
}

void lcdLine(uint8_t row, const char* text) {
  if (row >= LCD_ROWS) return;   // hanya baris 0 dan 1

  // Susun tepat 16 karakter: salin sampai 16 karakter, ganti non-ASCII dengan '?', isi spasi.
  char out[LCD_COLS + 1];
  uint8_t i = 0;
  for (; i < LCD_COLS && text[i] != '\0'; i++) {
    char c = text[i];
    out[i] = (c >= 0x20 && c <= 0x7E) ? c : '?';
  }
  for (; i < LCD_COLS; i++) out[i] = ' ';
  out[LCD_COLS] = '\0';

  // Isi sama dengan yang sudah tampil: tidak perlu dikirim. Satu baris penuh lewat backpack
  // I2C butuh puluhan milidetik (diukur di langkah 2), jadi ini menghemat waktu loop().
  if (strcmp(out, shown[row]) == 0) return;

  lcd.setCursor(0, row);   // selalu mulai dari kolom 0 baris ini
  lcd.print(out);
  strcpy(shown[row], out);
}

bool displayDue() {
  uint32_t now = millis();
  if (drawRequested || (now - lastDrawMs >= LCD_REFRESH_MS)) {
    drawRequested = false;
    lastDrawMs = now;
    return true;
  }
  return false;
}

void displayRequest() {
  drawRequested = true;
}

// display.cpp - lihat display.h.
//
// Fakta LCD dari datasheet HS1602A (Shenzhen Hansheng Industrial Co., Ltd., Ver. 20110521),
// pengontrol SPLC780 yang kompatibel HD44780:
// - 16 karakter tampak per baris. Baris 1 = DDRAM 0x00-0x0F, baris 2 mulai di DDRAM 0x40.
//   DDRAM tiap baris lebih panjang dari 16, jadi karakter ke-17 dst. ditulis ke alamat yang
//   tidak tampak, BUKAN pindah ke baris 2. Karena itu teks selalu dipotong di 16 karakter.
// - Font tetap 5x8 titik per karakter (tidak ada ukuran lain).
// - Perintah Clear butuh 1,64 ms (perintah biasa 42 us). Clear juga mengosongkan layar sesaat
//   sehingga tampak berkedip. Karena itu redraw TIDAK memakai lcd.clear(); karakter lama
//   selalu ditimpa (baris diisi spasi sampai 16), yang juga menghapus digit sisa angka yang memendek.
// - Hanya ASCII cetak (0x20-0x7E) yang dikirim. Karakter lain (mis. huruf UTF-8 seperti derajat)
//   terdiri dari beberapa byte dan tampil sebagai simbol acak di ROM LCD. Hindari juga '\' dan '~':
//   pada ROM HD44780 versi Jepang (A00) keduanya tampil sebagai simbol yen dan panah.
// - Setelah satu karakter ditulis, alamat DDRAM naik sendiri (entry mode). Jadi satu setCursor()
//   cukup untuk satu deretan karakter berurutan.
//
// Library: LiquidCrystal_I2C 1.1.4 (Frank de Brabander, dirawat Marco Schwartz), API init().

#include "display.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;

// Objek LCD global dengan alamat tetap 0x27 (dialokasikan statis, tanpa new).
static LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// Dua buffer 2 x 16 karakter (+1 untuk penutup string '\0'):
// - wanted: isi yang diminta lcdLine();
// - shown : isi yang sudah benar-benar ada di layar.
// displayUpdate() mengirim hanya posisi yang berbeda di antara keduanya.
static char wanted[LCD_ROWS][LCD_COLS + 1];
static char shown[LCD_ROWS][LCD_COLS + 1];
static uint8_t nextRow = 0;   // baris yang diperiksa lebih dulu (bergantian, supaya adil)

static uint32_t lastDrawMs = 0;         // millis() saat redraw terakhir yang diizinkan displayDue()
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

  // init() mengosongkan layar = semua DDRAM berisi spasi. Kedua buffer diisi spasi supaya
  // sama dengan layar; tulisan pertama lalu hanya mengirim karakter yang bukan spasi.
  for (uint8_t r = 0; r < LCD_ROWS; r++) {
    memset(wanted[r], ' ', LCD_COLS);
    memset(shown[r], ' ', LCD_COLS);
    wanted[r][LCD_COLS] = '\0';
    shown[r][LCD_COLS] = '\0';
  }
  drawRequested = true;
  return found;
}

void lcdLine(uint8_t row, const char* text) {
  if (row >= LCD_ROWS) return;   // hanya baris 0 dan 1

  // Susun tepat 16 karakter: salin sampai 16 karakter, ganti non-ASCII dengan '?', isi spasi.
  uint8_t i = 0;
  for (; i < LCD_COLS && text[i] != '\0'; i++) {
    char c = text[i];
    wanted[row][i] = (c >= 0x20 && c <= 0x7E) ? c : '?';
  }
  for (; i < LCD_COLS; i++) wanted[row][i] = ' ';
}

// Kirim karakter yang berbeda pada satu baris. Tiap deretan karakter berbeda yang berurutan
// diawali setCursor(kolom, baris) yang eksplisit (posisi tidak pernah ditebak), lalu
// karakternya ditulis berurutan. Mengembalikan jumlah karakter yang dikirim.
static uint8_t sendRowChanges(uint8_t row) {
  uint8_t sent = 0;
  uint8_t col = 0;
  while (col < LCD_COLS) {
    if (wanted[row][col] == shown[row][col]) {   // posisi ini sudah benar
      col++;
      continue;
    }
    uint8_t start = col;   // awal deretan yang berbeda
    while (col < LCD_COLS && wanted[row][col] != shown[row][col]) col++;
    lcd.setCursor(start, row);
    for (uint8_t c = start; c < col; c++) {
      lcd.write((uint8_t)wanted[row][c]);
      shown[row][c] = wanted[row][c];
      sent++;
    }
  }
  return sent;
}

uint8_t displayUpdate() {
  // Pemulihan sendiri tanpa delay(): tiap LCD_HEAL_MS (2 s) satu baris buffer "tampil" ditandai
  // "tidak diketahui" (diisi 0x00, tidak pernah sama dengan karakter ASCII cetak), bergantian
  // baris 0 dan 1. Akibatnya baris itu dikirim ulang penuh pada putaran berikutnya (tetap satu
  // baris per putaran, <= 28 ms). Alasan: bila gangguan I2C mengacaukan isi layar, karakter yang
  // "tidak berubah" menurut buffer tidak akan pernah dikirim lagi dan tetap salah selamanya.
  // Menulis ulang isi yang sama tidak membuat layar berkedip (tanpa Clear).
  // Batas: ini memperbaiki isi DDRAM, bukan pengontrol LCD yang kehilangan sinkron 4-bit.
  static uint32_t lastHealMs = 0;
  static uint8_t  healRow = 0;
  uint32_t now = millis();
  if (now - lastHealMs >= LCD_HEAL_MS) {
    lastHealMs = now;
    memset(shown[healRow], 0x00, LCD_COLS);
    healRow = (healRow + 1) % LCD_ROWS;
  }

  // Periksa kedua baris mulai dari nextRow; kirim baris pertama yang berbeda, lalu berhenti.
  // Perubahan dua baris karena itu selesai dalam dua putaran loop().
  for (uint8_t k = 0; k < LCD_ROWS; k++) {
    uint8_t row = (nextRow + k) % LCD_ROWS;
    if (memcmp(wanted[row], shown[row], LCD_COLS) != 0) {
      nextRow = (row + 1) % LCD_ROWS;   // putaran berikutnya mulai dari baris lain
      return sendRowChanges(row);
    }
  }
  return 0;   // layar sudah sesuai
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

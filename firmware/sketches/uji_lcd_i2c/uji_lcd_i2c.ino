// uji_lcd_i2c.ino  -  uji U4 (pemindai I2C) dan tampilan LCD 16x2 + backpack PCF8574
// Board: "DOIT ESP32 DEVKIT V1" (atau "ESP32 Dev Module"), Arduino-ESP32 core 3.x, Serial Monitor 115200 baud.
// Library: "LiquidCrystal I2C" (header LiquidCrystal_I2C.h), sama dengan Modul 3 GE3.
//
// SEBELUM MENYAMBUNG (U5, Bagian 6 desain PCB):
//   1. Backpack dicatu 3V3, BUKAN 5 V/VIN, sampai SDA dan SCL diukur.
//   2. Dengan backpack dicatu tetapi SDA/SCL belum disambung ke ESP32, ukur SDA-GND dan SCL-GND
//      pakai multimeter. Bila > 3,6 V, jangan sambungkan ke GPIO21/22.
// Sambungan: GND-GND, VCC-3V3, SDA-GPIO21, SCL-GPIO22.
//
// Perintah Serial: 's' = pindai ulang, 'r' = mulai ulang uji refresh.

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const uint8_t SDA_PIN = 21, SCL_PIN = 22;
const uint32_t I2C_HZ = 100000;              // alur §3.1
const unsigned long T_REFRESH_LCD_MS = 250;  // alur §3.2, nilai awal; disetel di E3 (50-500 ms)
const unsigned long T_HITUNG_MS = 100;       // laju perubahan nilai uji (meniru update hitung mundur S2)

LiquidCrystal_I2C *lcd = nullptr;
uint8_t alamatLcd = 0;

float nilai = 0.0f;        // angka uji yang berubah, untuk melihat keterbacaan dan sisa digit
int8_t arahNilai = 1;
unsigned long tNilai = 0, tLcd = 0;
uint32_t redraw = 0, tRedrawMulai = 0;

uint8_t pindai() {
  Serial.println(F("\n=== U4: PINDAI I2C (SDA=21, SCL=22) ==="));
  uint8_t ditemukan = 0, n = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  perangkat di 0x%02X\n", addr);
      if (addr == 0x27 || addr == 0x3F) ditemukan = addr;
      n++;
    }
  }
  if (n == 0) Serial.println(F("  Tidak ada perangkat. Ini kesalahan kabel, bukan kode: cek SDA, SCL, GND, catu."));
  else if (!ditemukan) Serial.println(F("  Ada perangkat, tetapi bukan 0x27/0x3F. Catat alamatnya."));
  return ditemukan;
}

void mulaiLcd() {
  alamatLcd = pindai();
  if (!alamatLcd) return;
  delete lcd;
  lcd = new LiquidCrystal_I2C(alamatLcd, 16, 2);
  lcd->init();                 // Modul 3 GE3: init(), lalu backlight()
  lcd->backlight();
  lcd->setCursor(0, 0);
  lcd->print("UKUR 1/3        ");
  lcd->setCursor(0, 1);
  lcd->print("Uji LCD 0x");
  lcd->print(alamatLcd, HEX);
  Serial.printf("LCD dimulai di 0x%02X. Bila layar kosong atau kotak penuh: putar potensiometer kontras di backpack.\n",
                alamatLcd);
  Serial.printf("Uji refresh: nilai berubah tiap %lu ms, layar digambar ulang tiap %lu ms.\n",
                T_HITUNG_MS, T_REFRESH_LCD_MS);
  tRedrawMulai = millis();
  redraw = 0;
}

void setup() {
  Serial.begin(115200);
  delay(1500);                 // tunggu sekali di setup() (Modul 2: boleh, belum ada tugas lain)
  Wire.begin(SDA_PIN, SCL_PIN, I2C_HZ);
  mulaiLcd();
}

void gambar() {
  // Baris 1 meniru tampilan GENGGAM fase 2 (alur §10.2): "UKUR 1/3  25.3kg".
  // Lebar bidang tetap 5 karakter supaya angka yang memendek tidak meninggalkan digit lama (Modul 3 GE3).
  char baris[17];
  snprintf(baris, sizeof(baris), "UKUR 1/3 %5.1fkg", nilai);
  lcd->setCursor(0, 0);
  lcd->print(baris);
  redraw++;
}

void loop() {
  unsigned long now = millis();

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 's' || c == 'S') mulaiLcd();
    if (c == 'r' || c == 'R') { tRedrawMulai = now; redraw = 0; }
  }
  if (!lcd) return;

  if (now - tNilai >= T_HITUNG_MS) {           // nilai berubah cepat, terpisah dari refresh layar
    tNilai = now;
    nilai += arahNilai * 0.7f;
    if (nilai > 99.0f) arahNilai = -1;
    if (nilai < 0.5f) arahNilai = 1;
  }

  if (now - tLcd >= T_REFRESH_LCD_MS) {        // refresh layar punya jadwal sendiri
    tLcd = now;
    unsigned long t0 = micros();
    gambar();
    unsigned long durasi = micros() - t0;
    if (redraw % 20 == 0) {
      Serial.printf("redraw #%lu: %lu us per gambar (I2C), %.1f redraw/detik\n", redraw, durasi,
                    1000.0f * redraw / (float)(now - tRedrawMulai + 1));
    }
  }
}

/*
  HGD - Hand Grip Dynamometer, firmware utama (embedded system)
  ------------------------------------------------------------
  Papan   : ESP32 DevKit V1 (DOIT ESP32 DEVKIT V1)
  Core    : Arduino-ESP32 3.3.11 (build akhir dan flash demo, Arduino IDE)
            Pengembangan juga dikompilasi dengan PlatformIO (core 2.0.17).
  Library : LiquidCrystal_I2C 1.1.4 (Frank de Brabander, dirawat Marco Schwartz)
            Wire (bawaan core Arduino-ESP32)

  Langkah 2 (uji LCD, uji tombol langkah 1 tetap jalan):
  - Baris 1: "HGD siap". Tiap TAHAN menukar baris 1 dengan teks 24 karakter untuk menguji
    pemotongan di 16 karakter (harus tampil "Uji potong 16 ka").
  - Baris 2: waktu sejak TAP terakhir (ms), digambar ulang tiap 500 ms. TAP mengembalikannya ke 0:
    angka yang memendek tidak boleh meninggalkan digit lama. TAP juga membalik LED.
  - LCD hanya menerima karakter yang berubah, paling banyak satu baris per putaran loop().
  - Serial: jumlah karakter dan lama tiap tulisan ke LCD (ms), putaran loop() terlama tiap 5 s.

  Serial Monitor: 115200 baud.
*/

#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "types.h"
#include "button.h"
#include "display.h"

static uint32_t tapCount = 0;
static uint32_t holdCount = 0;
static bool ledOn = false;

// Variabel uji langkah 2.
static uint32_t lastTapMs = 0;       // millis() saat TAP terakhir (baris 2 LCD)
static bool longTextOn = false;      // true = baris 1 menampilkan teks uji 24 karakter
static uint32_t redrawCount = 0;
static uint32_t loopMaxUs = 0;       // putaran loop() terlama dalam jendela 5 s
static uint32_t loopReportMs = 0;

void setup() {
  Serial.begin(BAUD);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  buttonInit();

  // I2C: pin ditulis eksplisit (21 = SDA, 22 = SCL), keduanya hanya ke sisi LV level shifter.
  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.setClock(I2C_HZ);
  displayInit();

  // Cetak versi core supaya jelas build mana yang sedang berjalan (PlatformIO atau Arduino IDE).
  Serial.println();
  Serial.printf("HGD firmware, core %d.%d.%d\n",
                ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
  Serial.println("Langkah 2: LCD. TAP = LED balik + waktu ke 0, TAHAN = uji teks panjang.");
  lastTapMs = millis();
}

void loop() {
  uint32_t loopStartUs = micros();

  Press p = buttonUpdate();

  // Uji: lama tekanan terakhir. Saat TAP keluar, buttonHeldMs() sudah 0, jadi dipakai nilai
  // dari putaran sebelumnya (termasuk 50 ms debounce saat lepas).
  static uint32_t pressMs = 0;
  uint32_t heldNow = buttonHeldMs();
  if (heldNow > 0) pressMs = heldNow;

  if (p == PRESS_TAP) {
    tapCount++;
    ledOn = !ledOn;
    digitalWrite(PIN_LED, ledOn ? HIGH : LOW);
    lastTapMs = millis();
    displayRequest();   // tampilkan angka 0 segera, tanpa menunggu 500 ms
    Serial.printf("[BTN] t=%lu TAP #%lu (tekan %lu ms) -> LED %s\n", (unsigned long)millis(),
                  (unsigned long)tapCount, (unsigned long)pressMs, ledOn ? "ON" : "OFF");
  } else if (p == PRESS_HOLD) {
    holdCount++;
    longTextOn = !longTextOn;
    displayRequest();
    Serial.printf("[BTN] t=%lu HOLD #%lu\n", (unsigned long)millis(), (unsigned long)holdCount);
  }

  // Waktu tahan untuk reset total (uji langkah 1): cetak sisa detik sekali per detik setelah 3 s.
  static uint32_t lastShownS = 0;
  static bool forceFired = false;
  uint32_t held = buttonHeldMs();
  if (held == 0) {
    lastShownS = 0;
    forceFired = false;
  } else if (!forceFired && held >= FORCE_RESET_MS) {
    forceFired = true;   // sekali per tekanan
    buttonReset();       // pelepasan setelah ini tidak boleh menjadi TAP
    Serial.println("[BTN] tahan 15 s -> RESET TOTAL (langkah 5+ menjalankannya)");
  } else if (!forceFired && held >= FORCE_RESET_SHOW_MS) {
    uint32_t leftS = (FORCE_RESET_MS - held + 999) / 1000;   // dibulatkan ke atas
    if (leftS != lastShownS) {
      lastShownS = leftS;
      Serial.printf("[BTN] Reset total %lu (lepas = batal)\n", (unsigned long)leftS);
    }
  }

  // LCD: isi baru disusun paling cepat tiap LCD_REFRESH_MS (atau segera setelah displayRequest()).
  if (displayDue()) {
    char line[24];   // sengaja lebih dari 16: lcdLine() yang memotong
    lcdLine(0, longTextOn ? "Uji potong 16 karakter!!" : "HGD siap");
    snprintf(line, sizeof(line), "t=%lu ms", (unsigned long)(millis() - lastTapMs));
    lcdLine(1, line);
  }

  // Kirim perubahan ke LCD: paling banyak satu baris per putaran.
  uint32_t t0 = micros();
  uint8_t sent = displayUpdate();
  if (sent > 0) {
    uint32_t dt = micros() - t0;
    redrawCount++;
    Serial.printf("[LCD] tulis #%lu: %u karakter, %.2f ms\n",
                  (unsigned long)redrawCount, sent, dt / 1000.0f);
  }

  // Putaran loop() terlama: penting untuk tombol (tap harus terbaca LOW minimal sekali).
  uint32_t loopUs = micros() - loopStartUs;
  if (loopUs > loopMaxUs) loopMaxUs = loopUs;
  if (millis() - loopReportMs >= 5000) {
    loopReportMs = millis();
    Serial.printf("[LOOP] terlama 5 s terakhir: %.2f ms\n", loopMaxUs / 1000.0f);
    loopMaxUs = 0;
  }
}

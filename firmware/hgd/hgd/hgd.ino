/*
  HGD - Hand Grip Dynamometer, firmware utama (embedded system)
  ------------------------------------------------------------
  Papan   : ESP32 DevKit V1 (DOIT ESP32 DEVKIT V1)
  Core    : Arduino-ESP32 3.3.11 (build akhir dan flash demo, Arduino IDE)
            Pengembangan juga dikompilasi dengan PlatformIO (core 2.0.17).
  Library : belum ada pada langkah 1 (LiquidCrystal_I2C 1.1.4 ditambahkan di langkah 2).

  Langkah 1 (uji tombol): TAP membalik LED, TAHAN 1,5 s dilaporkan sekali,
  tahan >= 3 s mencetak sisa detik menuju reset total (15 s). Belum ada state machine.

  Serial Monitor: 115200 baud.
*/

#include <Arduino.h>
#include "config.h"
#include "types.h"
#include "button.h"

static uint32_t tapCount = 0;
static uint32_t holdCount = 0;
static bool ledOn = false;

void setup() {
  Serial.begin(BAUD);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  buttonInit();

  // Cetak versi core supaya jelas build mana yang sedang berjalan (PlatformIO atau Arduino IDE).
  Serial.println();
  Serial.printf("HGD firmware, core %d.%d.%d\n",
                ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
  Serial.println("Langkah 1: TAP = LED balik, TAHAN 1,5 s, tahan 15 s = reset total (uji).");
}

void loop() {
  Press p = buttonUpdate();
  if (p == PRESS_TAP) {
    tapCount++;
    ledOn = !ledOn;
    digitalWrite(PIN_LED, ledOn ? HIGH : LOW);
    Serial.printf("[BTN] TAP #%lu -> LED %s\n", (unsigned long)tapCount, ledOn ? "ON" : "OFF");
  } else if (p == PRESS_HOLD) {
    holdCount++;
    Serial.printf("[BTN] HOLD #%lu\n", (unsigned long)holdCount);
  }

  // Waktu tahan untuk reset total: cetak sisa detik sekali per detik setelah 3 s.
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
}

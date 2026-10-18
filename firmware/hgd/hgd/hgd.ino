/*
  HGD - Hand Grip Dynamometer, firmware utama (embedded system)
  ------------------------------------------------------------
  Papan   : ESP32 DevKit V1 (DOIT ESP32 DEVKIT V1)
  Core    : Arduino-ESP32 3.3.11 (build akhir dan flash demo, Arduino IDE)
            Pengembangan juga dikompilasi dengan PlatformIO (core 2.0.17).
  Library : belum ada pada langkah 0.

  Langkah 0 (kerangka): cetak banner dan versi core, LED berkedip 1 Hz.
  Kedipan memakai millis(), bukan delay(), supaya loop() tidak pernah tertahan.

  Serial Monitor: 115200 baud.
*/

#include <Arduino.h>

// Pin LED status (D1 lewat R1 220 ohm). Akan dipindah ke config.h pada langkah 1.
const uint8_t PIN_LED = 27;

// Setengah periode kedip: 500 ms nyala + 500 ms mati = 1 Hz.
const uint32_t BLINK_HALF_MS = 500;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // Cetak versi core supaya jelas build mana yang sedang berjalan (PlatformIO atau Arduino IDE).
  Serial.println();
  Serial.printf("HGD firmware, core %d.%d.%d\n",
                ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
}

void loop() {
  // Pola non-blocking: cek apakah sudah waktunya, kalau belum langsung keluar.
  static uint32_t lastToggleMs = 0;
  static bool ledOn = false;

  uint32_t now = millis();
  if (now - lastToggleMs >= BLINK_HALF_MS) {   // pengurangan tetap benar saat millis() melimpah (~49 hari)
    lastToggleMs = now;
    ledOn = !ledOn;
    digitalWrite(PIN_LED, ledOn ? HIGH : LOW);
  }
}

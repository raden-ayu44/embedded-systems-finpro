/*
  HGD - Hand Grip Dynamometer, firmware utama (embedded system)
  ------------------------------------------------------------
  Papan   : ESP32 DevKit V1 (DOIT ESP32 DEVKIT V1)
  Core    : Arduino-ESP32 3.3.11 (build akhir dan flash demo, Arduino IDE)
            Pengembangan juga dikompilasi dengan PlatformIO (core 2.0.17).
  Library : LiquidCrystal_I2C 1.1.4 (Frank de Brabander, dirawat Marco Schwartz)
            Wire (bawaan core Arduino-ESP32)
            HX711 tidak memakai library (bit-bang sendiri, sensor.cpp).

  Langkah 3 (uji baca sensor dan tare):
  - Tare otomatis saat boot (~1 s, tanpa memblokir). Jangan sentuh alat saat "Nol-kan...".
  - Serial: satu baris per sampel (~10-12 baris/s): raw, kg, lama hxRead (us), STBL bila plateau.
  - LCD baris 1: kg, baris 2: raw (digambar ulang tiap 500 ms).
  - TAP atau ketik 't' = tare ulang. Ketik 'p' = cetak raw, offset, kg.
  - Tiap 5 s: jumlah sampel per detik dan putaran loop() terlama.

  Serial Monitor: 115200 baud.
*/

#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "types.h"
#include "button.h"
#include "display.h"
#include "sensor.h"

// Variabel uji langkah 3.
static uint32_t sampleCount = 0;     // sampel dalam jendela 5 s
static uint32_t loopMaxUs = 0;       // putaran loop() terlama dalam jendela 5 s
static uint32_t reportMs = 0;

// Perintah serial satu huruf (hanya bila DEBUG_CMD = 1 di config.h). Tidak menunggu:
// bila tidak ada karakter masuk, langsung kembali.
static void serialCmd() {
#if DEBUG_CMD
  if (Serial.available() == 0) return;
  char c = (char)Serial.read();
  if (c == 't') {
    sensorStartTare();
    Serial.println("[CMD] t: tare ulang");
  } else if (c == 'p') {
    Serial.printf("[CMD] p: raw=%ld offset=%ld kg=%+.3f\n",
                  (long)sensorRaw(), (long)sensorOffset(), sensorKg());
  }
  // Karakter lain (mis. '\n' dari Serial Monitor) diabaikan.
#endif
}

void setup() {
  Serial.begin(BAUD);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  buttonInit();

  // I2C: pin ditulis eksplisit (21 = SDA, 22 = SCL), keduanya hanya ke sisi LV level shifter.
  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.setClock(I2C_HZ);
  displayInit();

  sensorInit();
  sensorStartTare();   // tare pertama; selesai sendiri di sensorPoll() setelah 10 sampel

  // Cetak versi core supaya jelas build mana yang sedang berjalan (PlatformIO atau Arduino IDE).
  Serial.println();
  Serial.printf("HGD firmware, core %d.%d.%d\n",
                ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
  Serial.println("Langkah 3: sensor. TAP atau 't' = tare ulang, 'p' = cetak nilai.");
}

void loop() {
  uint32_t loopStartUs = micros();

  Press p = buttonUpdate();
  if (p == PRESS_TAP) {
    sensorStartTare();
    displayRequest();
    Serial.println("[BTN] TAP: tare ulang");
  }

  if (sensorPoll()) {
    sampleCount++;
    Serial.printf("raw=%ld kg=%+.3f baca=%lu us%s%s\n",
                  (long)sensorRaw(), sensorKg(), (unsigned long)sensorReadUs(),
                  sensorPlateau() ? " STBL" : "",
                  sensorTareDone() ? "" : " (tare)");
  }

  serialCmd();

  // LCD: kg dan raw, paling cepat tiap LCD_REFRESH_MS.
  if (displayDue()) {
    char line[24];
    if (!sensorTareDone()) {
      lcdLine(0, "Nol-kan...");
    } else {
      snprintf(line, sizeof(line), "%+8.2f kg", sensorKg());
      lcdLine(0, line);
    }
    snprintf(line, sizeof(line), "RAW %ld", (long)sensorRaw());
    lcdLine(1, line);
  }
  displayUpdate();

  // Laporan tiap 5 s: laju sampel dan putaran loop() terlama.
  uint32_t loopUs = micros() - loopStartUs;
  if (loopUs > loopMaxUs) loopMaxUs = loopUs;
  if (millis() - reportMs >= 5000) {
    reportMs = millis();
    Serial.printf("[INFO] %.1f sampel/s, loop terlama %.2f ms\n",
                  sampleCount / 5.0f, loopMaxUs / 1000.0f);
    sampleCount = 0;
    loopMaxUs = 0;
  }
}

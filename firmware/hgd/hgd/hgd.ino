/*
  HGD - Hand Grip Dynamometer, firmware utama (embedded system)
  ------------------------------------------------------------
  Papan   : ESP32 DevKit V1 (DOIT ESP32 DEVKIT V1)
  Core    : Arduino-ESP32 3.3.11 (build akhir dan flash demo, Arduino IDE)
            Pengembangan juga dikompilasi dengan PlatformIO (core 2.0.17).
  Library : LiquidCrystal_I2C 1.1.4 (Frank de Brabander, dirawat Marco Schwartz)
            Wire (bawaan core Arduino-ESP32)
            HX711 tidak memakai library (bit-bang sendiri, sensor.cpp).

  Langkah 5 (kerangka state machine + LED):
  - Boot: READY, LED kedip 5 s, "Menyesuaikan nol" lalu "Tekan tmbl START". TAP = START
    (dan hentikan kedip). TAHAN 1,5 s = tare ulang manual. Langkah 5c: setelah tare, kg > +1 ->
    "Lepas genggaman" (START diabaikan); kg < -1 -> tare ulang otomatis (nol diambil saat dibebani).
    5 sampel HX711 pertama setelah boot dibuang.
  - GRIP: "Menunggu genggam" + kg live; tekan > 1 kg = onset, "Tahan 3-5 detik" + kg + detik;
    langkah 6: dilepas sebelum 3 s -> kembali "Menunggu genggam"; 5 s tanpa plateau -> COMPUTE dengan
    jendela buffer terkuat (LED mati); plateau hanya dihitung bila semua sampel > 1 kg;
    TAHAN 1,5 s di "Menunggu genggam" = kembali ke READY dengan tare ulang (langkah 6c);
    LED menyala selama genggaman stabil; stabil pada >= 3 s -> COMPUTE -> REST
    ("Hasil xx.xx kg" / "Lepas>Rest 60s", LED tetap menyala sampai dilepas).
  - REST: setelah dilepas "NEXT > lanjut"; TAP -> percobaan berikutnya. Setelah 3 -> SUMMARY
    ("Mx.. Av.. kg" / "Tekan: RESTART"); TAP -> sesi baru (kedip).
  - Galat sensor di READY/GRIP/COMPUTE/REST -> FAULT (LED mati, ERR + raw); TAHAN 1,5 s -> READY.
  - Tahan 15 s di state apa pun = reset total (hitung mundur di LCD mulai detik ke-3).
  - Serial: [FSM] A -> B, [GRIP], [COMPUTE], [REST], [SUMMARY], [FAULT]; 't' tare, 'p' nilai.

  Serial Monitor: 115200 baud.
*/

#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "types.h"
#include "button.h"
#include "display.h"
#include "sensor.h"
#include "led.h"
#include "fsm.h"

// Variabel uji: putaran loop() terlama dalam jendela 5 s.
static uint32_t loopMaxUs = 0;
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
    Serial.printf("[CMD] p: raw=%ld offset=%ld kg=%+.3f state=%s\n",
                  (long)sensorRaw(), (long)sensorOffset(), sensorKg(), stateName(fsmState()));
  }
  // Karakter lain (mis. '\n' dari Serial Monitor) diabaikan.
#endif
}

void setup() {
  Serial.begin(BAUD);
  ledInit();
  buttonInit();

  // I2C: pin ditulis eksplisit (21 = SDA, 22 = SCL), keduanya hanya ke sisi LV level shifter.
  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.setClock(I2C_HZ);
  displayInit();

  sensorInit();

  // Cetak versi core supaya jelas build mana yang sedang berjalan (PlatformIO atau Arduino IDE).
  Serial.println();
  Serial.printf("HGD firmware, core %d.%d.%d\n",
                ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
  Serial.println("Langkah 6: logika GRIP.");

  fsmInit();   // READY dengan kedip restart dan tare pertama
}

void loop() {
  uint32_t loopStartUs = micros();

  Press press = buttonUpdate();   // TAP / HOLD / tidak ada, satu kali per putaran
  sensorPoll();                   // baca HX711 bila data siap (tidak menunggu)
  serialCmd();                    // perintah t / p
  fsmRun(press);                  // reset total -> cek galat -> handler state -> layar
  ledUpdate();                    // kedip restart (millis)
  displayUpdate();                // kirim perubahan LCD, paling banyak satu baris

  // Uji: putaran loop() terlama tiap 5 s (harus < 50 ms supaya tap >= 50 ms pasti terbaca).
  uint32_t loopUs = micros() - loopStartUs;
  if (loopUs > loopMaxUs) loopMaxUs = loopUs;
  if (millis() - reportMs >= 5000) {
    reportMs = millis();
    Serial.printf("[INFO] loop terlama %.2f ms\n", loopMaxUs / 1000.0f);
    loopMaxUs = 0;
  }
}

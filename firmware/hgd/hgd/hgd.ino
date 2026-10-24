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
    langkah 6/6d: dilepas sebelum 3 s -> kembali "Menunggu genggam"; antara 3 dan 5 s pengguna
    memutuskan kapan lepas (dilepas -> COMPUTE), pada 5 s -> COMPUTE; ref = jendela stabil terkuat
    (atau terkuat bila tidak pernah stabil); plateau hanya dihitung bila semua sampel > 1 kg;
    TAHAN 1,5 s di "Menunggu genggam" = tare ulang di tempat ("Menyesuaikan nol" ~1 s, langkah 6c);
    LED menyala selama genggaman stabil (tanda boleh lepas setelah 3 s) -> COMPUTE -> REST
    ("Hasil xx.xx kg" / "Lepas>Rest 60s", LED tetap menyala sampai dilepas).
  - REST: setelah dilepas hitung mundur "Lepas>Rest 60s" ... " 1s" (langkah 7, TAP diabaikan;
    digenggam lagi = hitung mundur dijeda sampai dilepas), lalu
    "NEXT > lanjut"; TAP -> percobaan berikutnya. Percobaan 3 tanpa istirahat: "Lepas>ringkasan"
    sampai dilepas, lalu langsung "NEXT > ringkasan". Setelah 3 -> SUMMARY
    ("Mx.. Av.." tanpa kg / "Menyimpan...", lalu "Data tersimpan" dan "Tekan: RESTART" bergantian
    tiap 2 s); TAP -> sesi baru (kedip).
  - Galat sensor di READY/GRIP/COMPUTE/REST -> FAULT (LED mati, ERR + raw); TAHAN 1,5 s -> READY.
  - Tahan 15 s di state apa pun = reset total (hitung mundur di LCD mulai detik ke-3).
  - Langkah 8: kg > 80 kg live di GRIP, atau ref <= 0 / > 80 kg di COMPUTE -> FAULT OUT_OF_RANGE
    ("ERR:DI LUAR RNTG"). Keadaan aman, layar galat dan baris [E] di fault.cpp.
  - Langkah 9: baris HGD,T/S/E,seq,boot,sesi,percobaan,nilai1,nilai2*CS (serial_log.cpp), tiap
    baris menunggu "ACK,<seq>" 500 ms (ACK_TIMEOUT_MS, langkah 10), NAK/tanpa balasan -> kirim
    ulang sekali; ringkasan gagal -> GAGAL SIMPAN.
  - Langkah 10c: boot = 4 digit acak per nyala, di semua baris (logger.py membedakan baris
    sebelum dan sesudah reset). Selama GRIP tiap sampel = baris G tanpa ACK (LOG_RAW di config.h);
    tiap upaya genggam (termasuk dilepas terlalu awal) diakhiri satu penanda M ber-ACK. Penanda yang
    gagal hanya dibuang dengan peringatan, tidak pernah GAGAL SIMPAN.
  - Serial: [FSM] A -> B, [GRIP], [COMPUTE], [REST], [SUMMARY], [FAULT], [LOG]; 't' tare, 'p' nilai,
    'x' skala demo /40 bolak-balik (bertahan sampai sesi baru), 's' simulasi saturasi bolak-balik,
    'u' uji ACK 100 baris (U9/E5), 'r' satu baris rusak (U10), 'w' uji watchdog (langkah 11).
  - Langkah 11: watchdog loop() (enableLoopWDT), sinkron ulang LCD tanpa blocking tiap 2 s,
    kg di LCD dibatasi -9.99..99.99 dan tanpa "-0.00", log [TIMING] latensi LED (S5).
  - Langkah 8c/8d: offset tare di luar -39000 +- 20000 counts -> tare diulang ("Nol ulang: lepas");
    terus-menerus 10 s -> READING_INVALID (load cell terputus).

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
#include "serial_log.h"

// Variabel uji: putaran loop() terlama dalam jendela 5 s.
static uint32_t loopMaxUs = 0;
static uint32_t reportMs = 0;

// Perintah serial satu huruf (hanya bila DEBUG_CMD = 1 di config.h). Hurufnya dibaca oleh
// logPoll() (serial_log.cpp), yang juga membaca baris ACK/NAK dari logger.py; c = 0 = tidak ada.
static void serialCmd(char c) {
#if DEBUG_CMD
  if (c == 0) return;
  if (c == 't') {
    sensorStartTare();
    Serial.println("[CMD] t: tare ulang");
  } else if (c == 'x') {
    // Demo galat X3 / uji batas 5 s: skala dibagi DEMO_SCALE_DIV. Tombol bolak-balik; bertahan
    // melewati tare, dimatikan juga oleh sesi baru atau reset total. READY tampil "SIAP x40 n/3".
    sensorDemoSet(!sensorDemoActive());
    Serial.printf("[CMD] x: skala demo %s (%.1f counts/kg)\n",
                  sensorDemoActive() ? "AKTIF, percobaan ditandai DEMO" : "mati", sensorScale());
  } else if (c == 's') {
    // Simulasi X2 (kabel sinyal tercabut): raw dipaksa skala penuh -> ADC_SATURATED -> FAULT.
    // Tombol bolak-balik: s lagi = "kabel dipasang kembali", lalu TAHAN 1,5 s untuk keluar FAULT.
    sensorSimSaturateSet(!sensorSimSaturateActive());
    Serial.printf("[CMD] s: simulasi saturasi %s\n", sensorSimSaturateActive() ? "AKTIF" : "mati");
  } else if (c == 'u') {
    // Uji U9 / E5: 100 baris uji ke logger.py, cetak waktu balasan ACK terlama dan rata-rata.
    logStartAckTest();
  } else if (c == 'r') {
    // Uji U10: satu baris dengan checksum salah -> logger.py harus membalas NAK, lalu ACK.
    logSendCorruptTest();
  } else if (c == 'w') {
    // Uji watchdog (langkah 11): loop() sengaja macet. Tanpa watchdog alat akan diam selamanya;
    // dengan watchdog ESP32 reset sendiri setelah ~5 s (boot baru = nomor boot baru).
    Serial.println("[CMD] w: uji watchdog, loop() sengaja macet; ESP32 harus reset dalam ~5 s");
    Serial.flush();
    while (true) {
      // sengaja kosong: watchdog tidak diberi makan
    }
  } else if (c == 'p') {
    Serial.printf("[CMD] p: boot=%s raw=%ld offset=%ld kg=%+.3f state=%s skala=%.1f%s%s\n",
                  logBootId(), (long)sensorRaw(), (long)sensorOffset(), sensorKg(), stateName(fsmState()),
                  sensorScale(), sensorDemoActive() ? " DEMO" : "",
                  sensorSimSaturateActive() ? " SIM-SAT" : "");
  }
  // Huruf lain diabaikan.
#else
  (void)c;   // DEBUG_CMD = 0: perintah dimatikan
#endif
}

void setup() {
  // Langkah 10c: buffer kirim 1024 byte (config.h SERIAL_TX_BUF), HARUS sebelum Serial.begin().
  Serial.setTxBufferSize(SERIAL_TX_BUF);
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
  Serial.printf("Langkah 10c: baris log HGD + ACK, rekaman genggaman %s.\n",
                LOG_RAW ? "aktif (LOG_RAW 1)" : "mati (LOG_RAW 0)");
  logInit();   // nomor boot acak, sebelum baris log pertama

  fsmInit();   // READY dengan kedip restart dan tare pertama

  // Watchdog (Bagian 4.9, langkah 11): task watchdog untuk task loop(). Core Arduino-ESP32 memberi
  // makan watchdog SATU kali setelah tiap putaran loop() selesai (loopTask di main.cpp), jadi
  // tidak ada feedLoopWDT() tambahan di kode ini. loop() yang macet > 5 s (batas core) membuat
  // ESP32 reset sendiri, bukan diam selamanya. Diaktifkan paling akhir: init LCD (~1 s) tidak ikut
  // diawasi.
  enableLoopWDT();
  Serial.println("[WDT] watchdog loop() aktif (batas 5 s)");
}

void loop() {
  uint32_t loopStartUs = micros();

  Press press = buttonUpdate();   // TAP / HOLD / tidak ada, satu kali per putaran
  sensorPoll();                   // baca HX711 bila data siap (tidak menunggu)
  serialCmd(logPoll());           // ACK/NAK + kirim ulang baris log; perintah t / p / x / s
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

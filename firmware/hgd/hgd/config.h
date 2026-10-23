// config.h - pin, konstanta waktu, dan threshold firmware HGD.
// Nilai dan alasannya ada di alur-program-hgd.md Bagian 3 (nama dokumen -> kode: Bagian 1.4a).
// Ubah nilai di sini saja; berkas lain hanya membaca.
#pragma once
#include <Arduino.h>

// ---------------- Pin (desain-pcb-hgd.md Bagian 2) ----------------
const uint8_t PIN_BUTTON = 25;   // tombol ke GND, INPUT_PULLUP, interrupt FALLING
const uint8_t PIN_LED    = 27;   // LED status D1 lewat R1 220 ohm
const uint8_t PIN_HX_DT  = 18;   // HX711 DT (data), masukan
const uint8_t PIN_HX_SCK = 19;   // HX711 SCK (clock), keluaran bit-bang
const uint8_t PIN_SDA    = 21;   // I2C ke sisi LV level shifter SAJA
const uint8_t PIN_SCL    = 22;   // PERINGATAN: 5 V langsung ke GPIO21/22 dapat merusak ESP32

// ---------------- Komunikasi ----------------
const uint32_t BAUD   = 115200;
const uint32_t I2C_HZ = 100000;
const uint8_t  LCD_ADDR = 0x27;  // alamat backpack PCF8574T (uji U4)

// ---------------- Tombol ----------------
const uint32_t DEBOUNCE_MS         = 50;     // abaikan tepi berikutnya selama ini (debounce di ISR)
const uint32_t HOLD_MS             = 1500;   // tekan selama ini = TAHAN (RESTART pengawas)
const uint32_t FORCE_RESET_MS      = 15000;  // tahan selama ini di state apa pun = reset total
const uint32_t FORCE_RESET_SHOW_MS = 3000;   // mulai tampilkan hitung mundur reset total

// ---------------- Percobaan dan sesi ----------------
const uint32_t GRIP_MIN_MS        = 3000;  // dilepas sebelum ini = dilepas terlalu awal
const uint32_t GRIP_MAX_MS        = 5000;  // batas paksa (force-stop)
const uint8_t  TRIALS_PER_SESSION = 3;
const uint32_t REST_S             = 60;    // istirahat antar percobaan (detik)

// ---------------- Sensor HX711 ----------------
const float    THRESHOLD_ONSET_KG  = 1.0f;     // E1: onset, dilepas-awal, lepas di REST, dan penjaga nol
                                                // di READY/GRIP fase 1 (+1 kg = masih digenggam, -1 kg = nol salah)
const float    THRESHOLD_STABLE_KG = 0.5f;     // E2: selisih maks-min buffer untuk plateau
const uint8_t  BUF_SIZE            = 5;        // sampel buffer plateau
const uint8_t  TARE_SAMPLES        = 10;       // tare = rata-rata 10 sampel (~1 s pada 10 SPS)
// Sebaran (maks - min) 10 sampel tare lebih dari ini = alat tersentuh/bergerak saat tare:
// tare diulang otomatis. 400 counts ~ 7 g. Sebaran normal terukur 48-136 counts, sampai 192 tepat
// setelah percobaan (langkah 3-7). Langkah 8: diturunkan dari 1000 karena tare bersebaran 585 counts
// (tangan masih dekat alat) lolos dan menggeser nol ~0,05 kg.
const int32_t  TARE_MAX_SPREAD     = 400;
// Sampel pertama setelah HX711 menyala belum stabil: tare pertama gagal di tiga boot berturut-turut
// (sebaran 941-2019 counts, ulangannya 62-97). Sebanyak ini sampel awal dibuang (~0,5 s).
const uint8_t  BOOT_SKIP_SAMPLES   = 5;
// Jendela offset tare (langkah 8c): |offset - OFFSET_EXPECTED| > OFFSET_WINDOW = READING_INVALID
// (load cell tidak terhubung dengan benar). KEDUANYA KHUSUS UNIT INI (ketidakseimbangan jembatan
// CZL 601 yang terpasang, terukur -38700 s.d. -40800 counts) dan harus diukur ulang saat kalibrasi
// atau bila load cell, kabel, atau HX711 diganti. Dengan E- terputus offset terukur ~+500.
const int32_t  OFFSET_EXPECTED     = -39000;
const int32_t  OFFSET_WINDOW       = 20000;
// Langkah 8d: offset di luar jendela mula-mula diperlakukan seperti tare terganggu (tangan di load
// cell: tare diulang, layar "Lepas genggaman"). Baru bila di luar jendela TERUS-MENERUS selama ini
// -> READING_INVALID (E- terputus tetap ~+500 di setiap ulangan, jadi tetap tertangkap). Berbasis
// waktu: 3 tare hanya ~1,7 s, terlalu singkat untuk tangan yang bertumpu sebentar.
const uint32_t OFFSET_BAD_MS       = 10000;
const float    SCALE_COUNTS_PER_KG = 53687.0f; // nilai awal; diganti hasil kalibrasi (Bagian 8)
const int32_t  ADC_MAX             = 8388607;  // skala penuh ADC 24 bit bertanda
const float    SATURATION_FRACTION = 0.95f;    // |raw| >= 0,95 x ADC_MAX = saturasi
const uint32_t HX711_TIMEOUT_MS    = 1000;     // tanpa sampel baru selama ini = HX711_TIMEOUT
const uint8_t  SAME_RAW_N          = 10;       // 10 raw identik berturut-turut = READING_INVALID (cadangan)
// Jarak antarsampel terlalu pendek = DT tertahan LOW (mis. SCK tercabut), bukan data asli.
// HANYA berlaku pada 10 SPS: HX711 sehat memberi sampel tiap ~85 ms (terukur 11,6 sampel/s), dan
// setelah dibaca DT naik HIGH sampai konversi berikutnya. Laju modul tetap 10 SPS (pin RATE di
// papan; 80 SPS butuh ubah resistor), jadi nilai ini tidak berubah tanpa ubah perangkat keras.
// Butuh SHORT_GAP_N jarak pendek BERTURUT-TURUT, bukan satu: satu baca yang terlambat (loop sibuk
// menulis LCD, maks. ~28 ms) memperpendek jarak berikutnya, dan mulai langkah 8 galat palsu terkunci.
const uint32_t SAMPLE_MIN_GAP_MS   = 40;       // jarak < ini dihitung "pendek" (normal ~85 ms)
const uint8_t  SHORT_GAP_N         = 3;        // 3 jarak pendek berturut-turut = READING_INVALID
const float    MIN_FORCE_KG        = 0.0f;     // ref harus > nilai ini
const float    MAX_FORCE_KG        = 80.0f;    // kapasitas nominal CZL 601; dicek live di GRIP
// Perintah x: skala dibagi ini sampai tare berikutnya. 40 (dokumen semula 20) supaya ~2 kg sudah
// terbaca > 80 kg di load cell tanpa housing (tekanan jari terkuat terukur ~2,8 kg).
const float    DEMO_SCALE_DIV      = 40.0f;

// ---------------- Tampilan dan LED ----------------
const uint32_t LCD_REFRESH_MS      = 500;   // aturan modul praktikum: maks. 2 redraw per detik
const uint32_t LCD_HEAL_MS         = 2000;  // tiap 2 s satu baris dikirim ulang penuh (pulih dari gangguan I2C)
const uint32_t SAVE_FAILED_PAGE_MS = 3000;  // ganti halaman layar SAVE_FAILED
const uint32_t LED_BLINK_MS        = 250;   // selang ganti nyala/mati kedip restart (2 Hz)
const uint32_t LED_BLINK_TOTAL_MS  = 5000;  // lama kedip restart di READY

// ---------------- Logging serial ----------------
// E5: batas tunggu ACK dari logger.py. Langkah 9b: 750 ms (semula 2000). ACK terukur ~30 ms, jadi
// masih longgar; dengan kirim ulang sekali, logger.py yang mati terdeteksi dalam ~1,5 s (lihat
// serial_log.cpp: kelompok berhenti setelah baris pertama yang gagal).
const uint32_t ACK_TIMEOUT_MS = 750;

// ---------------- Saklar build ----------------
// 1 = perintah serial t/k/p/x aktif (kalibrasi dan demo galat X3). Lihat Bagian 5.21.
#define DEBUG_CMD 1

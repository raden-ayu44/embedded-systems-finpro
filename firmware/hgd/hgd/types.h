// types.h - enum dan struct bersama (alur-program-hgd.md Bagian 2).
// Struct di-reset dengan memset(..., 0, ...), jadi arti nilai 0 tiap enum dipilih dengan sengaja:
// - FaultType, GripExit, Press: 0 = "tidak ada" (FAULT_NONE, EXIT_NONE, PRESS_NONE).
// - State: 0 = READY, supaya memset meninggalkan mesin di READY (titik awal yang aman).
// - GripPhase, RestPhase: mulai dari 1, sehingga 0 berarti "belum dimulai".
#pragma once
#include <Arduino.h>
#include "config.h"

// Tujuh state mesin (dokumen: SIAP, GENGGAM, HITUNG, RESPON_ISTIRAHAT, RINGKASAN, ERROR, GAGALSIMPAN).
enum State {
  READY = 0,
  GRIP,
  COMPUTE,
  REST,
  SUMMARY,
  FAULT,
  SAVE_FAILED
};

// Dua fase di dalam GRIP.
enum GripPhase {
  WAIT_ONSET = 1,   // menunggu gaya melewati onset; timer belum jalan
  MEASURING  = 2    // timer 3-5 s berjalan; buffer dan plateau
};

// Dua sub-fase di dalam REST.
enum RestPhase {
  RELEASE_REST = 1, // tunggu lepas, lalu hitung mundur istirahat
  WAIT_NEXT    = 2  // tunggu TAP (NEXT)
};

// Galat sensor. Angka = kode nilai1 pada baris E (Bagian 9.2).
enum FaultType {
  FAULT_NONE = 0,
  HX711_TIMEOUT = 1,
  ADC_SATURATED = 2,
  OUT_OF_RANGE = 3,
  READING_INVALID = 4
};

// Nama galat untuk pesan serial. "inline" = fungsi kecil di header yang boleh di-include
// banyak berkas tanpa galat "didefinisikan dua kali" saat linking.
inline const char* faultName(FaultType f) {
  switch (f) {
    case HX711_TIMEOUT:   return "HX711_TIMEOUT";
    case ADC_SATURATED:   return "ADC_SATURATED";
    case OUT_OF_RANGE:    return "OUT_OF_RANGE";
    case READING_INVALID: return "READING_INVALID";
    default:              return "NONE";
  }
}

// Jalur keluar GRIP.
// Langkah 6d: genggaman TIDAK lagi berakhir otomatis saat plateau; pengguna yang memutuskan lepas.
enum GripExit {
  EXIT_NONE = 0,
  EXIT_EARLY_RELEASE,   // dilepas sebelum 3 s: kembali ke fase 1, tidak dihitung
  EXIT_RELEASE,         // dilepas antara 3 dan 5 s: percobaan selesai dan dihitung
  EXIT_TIMEOUT          // masih digenggam pada 5 s: batas paksa
};

// Hasil klasifikasi tombol, satu per putaran loop().
enum Press {
  PRESS_NONE = 0,
  PRESS_TAP,
  PRESS_HOLD
};

// Hasil satu percobaan.
struct Trial {
  float    ref;      // nilai akhir (kg, presisi penuh, sebelum dibulatkan untuk LCD)
  bool     valid;    // lolos validasi rentang
  uint32_t durMs;    // lama genggam sampai keluar
  GripExit exitBy;   // EXIT_RELEASE atau EXIT_TIMEOUT
  bool     steady;   // ref dari jendela stabil (plateau); false = jendela terkuat tanpa plateau
  bool     demo;     // diukur saat skala demo (x) aktif: BUKAN data nyata; langkah 9 menandainya
};

// Satu sesi = 3 percobaan + ringkasan.
struct Session {
  uint16_t number;                    // naik 1 tiap sesi baru sejak boot, tidak kembali ke 1
  uint8_t  trialNo;                   // 1..3
  Trial    trials[TRIALS_PER_SESSION];
  float    peak, avg;                 // dihitung sekali di SUMMARY
  uint32_t t0Ms;                      // millis() saat sesi mulai (penanda T+mm:ss)
};

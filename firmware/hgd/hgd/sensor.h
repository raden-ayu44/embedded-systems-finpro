// sensor.h - HX711 (bit-bang, tanpa library) + load cell: baca non-blocking, tare, kg, buffer.
// Alur dan alasan: alur-program-hgd.md Bagian 5.6-5.9 dan 7.1-7.5.
#pragma once
#include "types.h"

// Atur pin HX711. Panggil sekali di setup(). Tidak menunggu HX711.
void sensorInit();

// Panggil sekali per loop(). Bila DT sudah LOW (data siap), baca satu sampel (terukur ~93-114 us),
// hitung kg, isi buffer, dan suapi tare bila sedang berjalan. Bila DT masih HIGH, langsung kembali.
// Mengembalikan true bila ada sampel baru pada putaran ini.
bool sensorPoll();

// Mulai tare: TARE_SAMPLES sampel berikutnya dirata-rata menjadi offset baru (~1 s pada 10 SPS),
// dikumpulkan oleh sensorPoll() lintas putaran loop(), jadi tidak memblokir.
// Bila sebaran 10 sampel > TARE_MAX_SPREAD (alat tersentuh), offset lama dipertahankan dan
// tare diulang otomatis; sensorTareDone() tetap false sampai tare berhasil.
void sensorStartTare();

// true bila tidak ada tare yang sedang berjalan (offset siap dipakai).
bool sensorTareDone();

// true bila tare sedang diulang karena offset terakhir di luar jendela (kemungkinan tangan di
// load cell): layar menampilkan "Lepas genggaman" alih-alih "Menyesuaikan nol" (langkah 8d).
bool sensorTareWindowRetry();

// Perintah x (langkah 8c): skala dibagi DEMO_SCALE_DIV selama aktif (demo X3, uji batas 5 s).
// Bertahan melewati tare; dimatikan oleh sesi baru (termasuk reset total) atau x lagi.
void  sensorDemoSet(bool on);
bool  sensorDemoActive();
float sensorScale();     // counts per kg yang sedang dipakai

// Perintah s (langkah 8c): simulasi X2, raw dipaksa skala penuh -> ADC_SATURATED lewat jalur normal.
void sensorSimSaturateSet(bool on);
bool sensorSimSaturateActive();

// Nilai terakhir.
int32_t  sensorRaw();            // counts mentah (24 bit bertanda)
int32_t  sensorOffset();         // counts hasil tare terakhir
float    sensorKg();             // (raw - offset) / skala (skala demo bila perintah x aktif)
float    sensorKgReal();         // (raw - offset) / SCALE_COUNTS_PER_KG, selalu skala normal;
                                 // hanya untuk penjaga nol (READY, GRIP fase 1)
uint32_t sensorLastSampleMs();   // millis() saat sampel terakhir diterima
uint32_t sensorReadUs();         // lama hxRead() terakhir (us), untuk uji waktu

// Galat sensor saat ini (alur-program-hgd.md Bagian 7.4), dihitung ulang dari data terakhir tiap
// kali dipanggil, jadi kembali FAULT_NONE sendiri setelah kabel dibetulkan. Urutan prioritas:
// HX711_TIMEOUT (tanpa sampel 1 s) > READING_INVALID (SHORT_GAP_N sampel berturut-turut datang
// < SAMPLE_MIN_GAP_MS setelah sebelumnya) > ADC_SATURATED (|raw| >= 0,95 x ADC_MAX) >
// READING_INVALID cadangan (SAME_RAW_N raw identik). OUT_OF_RANGE bukan di sini: dicek state
// machine (live di GRIP dan saat COMPUTE), karena bergantung pada kg dan state.
FaultType sensorCheck();

// Untuk pesan galat dan uji.
uint8_t  sensorSameRun();       // jumlah raw identik berturut-turut terakhir
uint8_t  sensorShortGapRun();   // jumlah jarak pendek berturut-turut terakhir
uint32_t sensorLastGapMs();     // jarak sampel terakhir ke sebelumnya (ms)

// Buffer melingkar BUF_SIZE sampel kg terakhir.
bool  sensorPlateau();   // buffer penuh dan (maks - min) <= THRESHOLD_STABLE_KG
bool  sensorBufFull();   // buffer sudah berisi BUF_SIZE sampel
float sensorBufSpread(); // maks - min isi buffer (0 bila kosong)
float sensorBufMin();    // nilai terkecil di buffer (0 bila kosong)
float sensorBufAvg();    // rata-rata isi buffer (0 bila kosong)
void  sensorBufReset();  // kosongkan buffer (mis. saat mulai mengukur)

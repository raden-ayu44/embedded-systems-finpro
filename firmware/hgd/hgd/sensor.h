// sensor.h - HX711 (bit-bang, tanpa library) + load cell: baca non-blocking, tare, kg, buffer.
// Alur dan alasan: alur-program-hgd.md Bagian 5.6-5.9 dan 7.1-7.5.
// Cek galat sensor (sensorCheck) ditambahkan di langkah 4.
#pragma once
#include "types.h"

// Atur pin HX711. Panggil sekali di setup(). Tidak menunggu HX711.
void sensorInit();

// Panggil sekali per loop(). Bila DT sudah LOW (data siap), baca satu sampel (~50-60 us),
// hitung kg, isi buffer, dan suapi tare bila sedang berjalan. Bila DT masih HIGH, langsung kembali.
// Mengembalikan true bila ada sampel baru pada putaran ini.
bool sensorPoll();

// Mulai tare: TARE_SAMPLES sampel berikutnya dirata-rata menjadi offset baru (~1 s pada 10 SPS),
// dikumpulkan oleh sensorPoll() lintas putaran loop(), jadi tidak memblokir.
void sensorStartTare();

// true bila tidak ada tare yang sedang berjalan (offset siap dipakai).
bool sensorTareDone();

// Nilai terakhir.
int32_t  sensorRaw();            // counts mentah (24 bit bertanda)
int32_t  sensorOffset();         // counts hasil tare terakhir
float    sensorKg();             // (raw - offset) / skala
uint32_t sensorLastSampleMs();   // millis() saat sampel terakhir diterima
uint32_t sensorReadUs();         // lama hxRead() terakhir (us), untuk uji waktu

// Buffer melingkar BUF_SIZE sampel kg terakhir.
bool  sensorPlateau();   // buffer penuh dan (maks - min) <= THRESHOLD_STABLE_KG
float sensorBufAvg();    // rata-rata isi buffer (0 bila kosong)
void  sensorBufReset();  // kosongkan buffer (mis. saat mulai mengukur)

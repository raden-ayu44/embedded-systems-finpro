// fsm.h - state machine utama: READY, GRIP, COMPUTE, REST, SUMMARY, FAULT, SAVE_FAILED.
// Alur dan alasan: alur-program-hgd.md Bagian 1.3, 6.1-6.5 dan 10.2-10.3.
// Bentuk: satu handler per state dipilih lewat switch di fsmRun(), satu fungsi enterX() per state
// untuk aksi masuk. Tabel transisi di Bagian 6.3 adalah spesifikasinya.
#pragma once
#include "types.h"

// Sesi 1, percobaan 1, masuk READY dengan kedip restart (nyala pertama = restart).
// Panggil sekali di setup(), setelah ledInit(), buttonInit(), displayInit() dan sensorInit().
void fsmInit();

// Panggil sekali per loop(), dengan hasil buttonUpdate() putaran ini. Urutan tetap:
// (1) reset total (tombol ditahan 15 s), (2) cek galat sensor bila state di dalam siklus
// percobaan (transisi pengecualian ke FAULT), (3) handler state aktif, (4) gambar layar.
void fsmRun(Press press);

// State aktif dan namanya (untuk pesan serial).
State fsmState();
const char* stateName(State s);

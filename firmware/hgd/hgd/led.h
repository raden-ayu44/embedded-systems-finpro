// led.h - LED status D1 (GPIO27 lewat 220 ohm).
// Aturan kapan menyala ada di alur-program-hgd.md Bagian 10.3 dan diputuskan oleh state machine
// (fsm.cpp) dan modul galat (fault.cpp). Modul ini hanya menyediakan nyala/mati, kedip restart,
// dan kedip alarm galat, semuanya non-blocking.
#pragma once
#include "types.h"

// Atur pin, LED mati. Panggil sekali di setup().
void ledInit();

// Nyalakan (true) atau matikan (false). Menghentikan kedip restart atau alarm bila sedang berjalan.
void ledSet(bool on);

// Mulai kedip restart: LED_BLINK_TOTAL_MS (5 s), ganti nyala/mati tiap LED_BLINK_MS (2 Hz).
// Berhenti sendiri (LED mati) setelah 5 s, atau lebih awal lewat ledSet().
void ledBlinkStart();

// Mulai kedip alarm galat (ERROR, GAGALSIMPAN): ganti nyala/mati tiap LED_ALARM_MS (5 Hz), lebih
// cepat dari kedip restart supaya keduanya bisa dibedakan. Tanpa batas waktu: berhenti hanya
// lewat ledSet() atau ledBlinkStart() (saat keluar dari state galat).
void ledAlarmStart();

// true selama kedip restart berjalan (bukan alarm).
bool ledBlinking();

// Panggil sekali per loop(). Menjalankan kedip dengan millis(), tanpa menunggu.
void ledUpdate();

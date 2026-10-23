// led.h - LED status D1 (GPIO27 lewat 220 ohm).
// Aturan kapan menyala ada di alur-program-hgd.md Bagian 10.3 dan diputuskan oleh state machine
// (fsm.cpp). Modul ini hanya menyediakan nyala/mati dan kedip restart yang non-blocking.
#pragma once
#include "types.h"

// Atur pin, LED mati. Panggil sekali di setup().
void ledInit();

// Nyalakan (true) atau matikan (false). Menghentikan kedip restart bila sedang berjalan.
void ledSet(bool on);

// Mulai kedip restart: LED_BLINK_TOTAL_MS (5 s), ganti nyala/mati tiap LED_BLINK_MS.
// Berhenti sendiri (LED mati) setelah 5 s, atau lebih awal lewat ledSet().
void ledBlinkStart();

// true selama kedip restart berjalan.
bool ledBlinking();

// Panggil sekali per loop(). Menjalankan kedip dengan millis(), tanpa menunggu.
void ledUpdate();

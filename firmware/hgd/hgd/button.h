// button.h - tombol tunggal (GPIO25): ISR + debounce, TAP vs TAHAN, waktu tahan reset total.
// Alur dan alasan: alur-program-hgd.md Bagian 4.1-4.5, 5.3-5.5, 6.2 (reset total).
#pragma once
#include "types.h"

// Pasang pin dan interrupt. Panggil sekali di setup().
void buttonInit();

// Panggil sekali per loop(). Mengembalikan PRESS_TAP saat tombol dilepas sebelum HOLD_MS,
// PRESS_HOLD sekali saat tekanan mencapai HOLD_MS (tanpa menunggu lepas), selain itu PRESS_NONE.
Press buttonUpdate();

// Lama tekanan fisik yang sedang berlangsung (ms); 0 bila tombol tidak ditekan.
// Dipakai reset total. Tidak terpengaruh buttonReset().
uint32_t buttonHeldMs();

// Panggil tiap masuk state yang menunggu tombol. Membuang flag ISR yang tersisa dan menandai
// tekanan yang sedang berlangsung sebagai "abaikan sampai dilepas" (tidak menghasilkan TAP/TAHAN).
void buttonReset();

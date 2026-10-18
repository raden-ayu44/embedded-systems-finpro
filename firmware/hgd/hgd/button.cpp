// button.cpp - lihat button.h.
// Pola dari sketsa uji_bench_hgd (U8), ditambah: debounce saat lepas, cek "pernah terbaca LOW",
// "abaikan sampai dilepas", dan waktu tahan untuk reset total (v5).
#include "button.h"

// Status tombol (dokumen: StatusTombol). Satu instance statis, tanpa malloc.
struct ButtonState {
  volatile bool     isrFlag;    // diset ISR saat tepi turun lolos debounce
  volatile uint32_t isrLastMs;  // waktu interrupt terakhir yang diterima; hanya dipakai di ISR
  bool     down;                // loop sedang mencatat satu tekanan
  bool     sawLow;              // pin pernah terbaca LOW selama tekanan ini
  bool     holdFired;           // TAHAN sudah dilaporkan untuk tekanan ini
  bool     ignore;              // tekanan ini diabaikan sampai dilepas (buttonReset)
  bool     highSeen;            // pin sedang terbaca HIGH sejak highStartMs (calon lepas)
  uint32_t highStartMs;         // millis() saat pin mulai terbaca HIGH
  uint32_t downMs;              // millis() saat tekanan mulai
  uint32_t releaseMs;           // millis() saat lepas terakhir dikonfirmasi
};

static ButtonState btn;

// ISR: dijalankan hardware saat GPIO25 turun (tombol ditekan).
// IRAM_ATTR = kode ditaruh di RAM supaya bisa jalan walau flash sedang diakses.
// Aturan ISR: singkat, hanya menulis flag; tidak memanggil Serial atau Wire.
static void IRAM_ATTR isrButton() {
  uint32_t now = millis();
  if (now - btn.isrLastMs < DEBOUNCE_MS) return;   // pantulan kontak: abaikan
  btn.isrLastMs = now;
  btn.isrFlag = true;
}

void buttonInit() {
  // Pull-up internal: pin tidak mengambang saat tombol dilepas (tanpa ini interrupt bisa acak).
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), isrButton, FALLING);
}

Press buttonUpdate() {
  uint32_t now = millis();
  bool pressed = (digitalRead(PIN_BUTTON) == LOW);   // LOW = sedang ditekan (pull-up)

  // 1) Tepi turun dari ISR: SELALU mulai satu tekanan, apa pun level pin saat ini.
  //    Putaran loop() pertama setelah ISR sering jatuh di tengah pantulan kontak, sehingga pin
  //    bisa terbaca HIGH walau tombol sedang ditekan. Karena itu level pin saat ini tidak dipakai
  //    untuk memutuskan; keputusan diambil saat lepas (langkah 3).
  if (btn.isrFlag) {
    btn.isrFlag = false;   // bool 1 byte: baca/tulis atomik, tidak perlu noInterrupts()
    if (!btn.down && (now - btn.releaseMs > DEBOUNCE_MS)) {
      btn.down = true;
      btn.downMs = now;
      btn.sawLow = pressed;
      btn.holdFired = false;
      btn.ignore = false;
      btn.highSeen = false;
    }
  }

  if (!btn.down) return PRESS_NONE;

  // 2) Pin LOW: tombol masih ditekan.
  if (pressed) {
    btn.sawLow = true;
    btn.highSeen = false;   // calon lepas sebelumnya hanya pantulan: batalkan
    if (!btn.holdFired && !btn.ignore && (now - btn.downMs >= HOLD_MS)) {
      btn.holdFired = true;
      return PRESS_HOLD;   // dilaporkan sekali, tanpa menunggu lepas
    }
    return PRESS_NONE;
  }

  // 3) Pin HIGH: debounce saat lepas (A12). Kontak memantul saat ditekan dan saat dilepas, jadi
  //    satu bacaan HIGH belum berarti lepas. Tombol dianggap lepas hanya bila pin terbaca HIGH
  //    terus-menerus selama DEBOUNCE_MS; bila sempat LOW lagi, langkah 2 membatalkan hitungannya.
  //    Tanpa ini, pantulan sesaat setelah ditekan terbaca sebagai lepas: TAP palsu keluar,
  //    ISR lalu menyaring tepi berikutnya, dan tekanan panjang tidak pernah menjadi TAHAN.
  if (!btn.highSeen) {
    btn.highSeen = true;
    btn.highStartMs = now;
  }
  if (now - btn.highStartMs < DEBOUNCE_MS) return PRESS_NONE;   // belum stabil HIGH

  // Lepas terkonfirmasi. TAP hanya bila:
  // - sawLow: pin pernah terbaca LOW oleh loop (tekanan nyata, bukan derau listrik sesaat
  //   yang memicu ISR tetapi tidak pernah terlihat sebagai LOW),
  // - belum TAHAN (tekanan panjang bukan TAP), dan
  // - tidak sedang diabaikan (buttonReset saat masuk state).
  btn.down = false;
  btn.releaseMs = now;
  if (btn.sawLow && !btn.holdFired && !btn.ignore) return PRESS_TAP;
  return PRESS_NONE;
}

uint32_t buttonHeldMs() {
  if (!btn.down) return 0;
  return millis() - btn.downMs;
}

void buttonReset() {
  btn.isrFlag = false;
  // Tekanan yang sedang berlangsung tidak dihapus (waktu tahan reset total tetap jalan),
  // hanya diabaikan supaya pelepasannya tidak terbaca sebagai TAP.
  if (btn.down) btn.ignore = true;
}

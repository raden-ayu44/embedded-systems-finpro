// led.cpp - lihat led.h.
#include "led.h"

// Dua jenis kedip, tidak pernah bersamaan:
// - kedip restart: 2 Hz, berhenti sendiri setelah 5 s;
// - kedip alarm galat: 5 Hz, terus sampai ledSet() dipanggil (keluar dari ERROR / GAGALSIMPAN).
static bool     blinking = false;     // kedip restart berjalan
static bool     alarmOn = false;      // kedip alarm galat berjalan
static uint32_t blinkStartMs = 0;     // millis() saat kedip (jenis apa pun) dimulai

void ledInit() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
}

void ledSet(bool on) {
  blinking = false;   // perintah langsung selalu mengalahkan kedip (restart maupun alarm)
  alarmOn = false;
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}

void ledBlinkStart() {
  blinking = true;
  alarmOn = false;
  blinkStartMs = millis();
  digitalWrite(PIN_LED, HIGH);   // kedip mulai dari nyala
}

void ledAlarmStart() {
  alarmOn = true;
  blinking = false;
  blinkStartMs = millis();
  digitalWrite(PIN_LED, HIGH);   // kedip mulai dari nyala
}

bool ledBlinking() {
  return blinking;
}

void ledUpdate() {
  uint32_t t = millis() - blinkStartMs;   // lama kedip sejauh ini
  if (alarmOn) {
    // Alarm galat: tanpa batas waktu. Selang genap = nyala, ganjil = mati (tiap LED_ALARM_MS).
    bool on = ((t / LED_ALARM_MS) % 2) == 0;
    digitalWrite(PIN_LED, on ? HIGH : LOW);
    return;
  }
  if (!blinking) return;
  if (t >= LED_BLINK_TOTAL_MS) {          // 5 s habis: berhenti dan mati
    blinking = false;
    digitalWrite(PIN_LED, LOW);
    return;
  }
  // Selang ke-0, 2, 4, ... = nyala; ke-1, 3, 5, ... = mati (tiap selang LED_BLINK_MS).
  bool on = ((t / LED_BLINK_MS) % 2) == 0;
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}

// led.cpp - lihat led.h.
#include "led.h"

static bool     blinking = false;
static uint32_t blinkStartMs = 0;

void ledInit() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
}

void ledSet(bool on) {
  blinking = false;   // perintah langsung selalu mengalahkan kedip
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}

void ledBlinkStart() {
  blinking = true;
  blinkStartMs = millis();
  digitalWrite(PIN_LED, HIGH);   // kedip mulai dari nyala
}

bool ledBlinking() {
  return blinking;
}

void ledUpdate() {
  if (!blinking) return;
  uint32_t t = millis() - blinkStartMs;   // lama kedip sejauh ini
  if (t >= LED_BLINK_TOTAL_MS) {          // 5 s habis: berhenti dan mati
    blinking = false;
    digitalWrite(PIN_LED, LOW);
    return;
  }
  // Selang ke-0, 2, 4, ... = nyala; ke-1, 3, 5, ... = mati (tiap selang LED_BLINK_MS).
  bool on = ((t / LED_BLINK_MS) % 2) == 0;
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}

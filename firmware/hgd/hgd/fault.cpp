// fault.cpp - lihat fault.h.
#include "fault.h"
#include "button.h"
#include "led.h"
#include "sensor.h"

static FaultType active = FAULT_NONE;   // galat yang sedang aktif

void faultEnter(FaultType type, uint16_t sessionNo, uint8_t trialNo, uint32_t sessionT0Ms) {
  active = type;
  buttonReset();       // tekanan yang sedang berlangsung tidak boleh langsung menjadi TAHAN keluar
  ledSet(false);       // keadaan aman (R7): LED dipaksa mati, tidak terus mengaktuasi saat galat
  // raw dan kg ikut dicetak supaya penyebabnya bisa diperiksa (mis. kg = 85 pada OUT_OF_RANGE).
  Serial.printf("[FAULT] %s raw=%ld kg=%.2f\n", faultName(type), (long)sensorRaw(), sensorKg());
  faultLogEvent((uint8_t)type, sessionNo, trialNo, sessionT0Ms);
}

FaultType faultActive() { return active; }

void faultClear() { active = FAULT_NONE; }

void faultDraw(char* l0, char* l1, size_t n) {
  switch (active) {
    case HX711_TIMEOUT:   snprintf(l0, n, "ERR:HX711 TMOUT");  break;
    case ADC_SATURATED:   snprintf(l0, n, "ERR:ADC SATURASI"); break;
    case OUT_OF_RANGE:    snprintf(l0, n, "ERR:DI LUAR RNTG"); break;
    case READING_INVALID: snprintf(l0, n, "ERR:BACA TDK VLD"); break;
    default:              snprintf(l0, n, "ERR");              break;
  }
  // raw live untuk pengawas: tetap diperbarui tiap LCD_REFRESH_MS selama FAULT, sehingga
  // terlihat kapan sambungan kembali normal (mis. kabel DT dipasang lagi).
  snprintf(l1, n, "RAW %ld", (long)sensorRaw());
}

void faultLogEvent(uint8_t code, uint16_t sessionNo, uint8_t trialNo, uint32_t sessionT0Ms) {
  uint32_t secs = (millis() - sessionT0Ms) / 1000;
  Serial.printf("[E] kode=%u sesi=%u percobaan=%u t=%lu s\n",
                code, sessionNo, trialNo, (unsigned long)secs);
}

// esp32_uji_awal.ino  -  uji awal ESP32 (DevKit V1) untuk proyek hand grip dynamometer
// Board: "DOIT ESP32 DEVKIT V1", Serial Monitor 115200 baud, line ending "Newline" atau "Both NL & CR".
// Tanpa multimeter. Kirim 'r' di Serial Monitor untuk mengulang semua uji.
//
// Kabel jumper untuk uji loopback (cabut semua modul lain dari pin ini lebih dulu):
//   GPIO18 <-> GPIO19
//   GPIO21 <-> GPIO22
//   GPIO25 <-> GPIO27
// JANGAN beri 5 V ke pin GPIO mana pun. Pakai satu sumber daya saja (USB).

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "esp_system.h"
#include "esp_chip_info.h"   // core 3.x memindahkan esp_chip_info_t ke header ini

const uint8_t PAIRS[][2] = {{18, 19}, {21, 22}, {25, 27}};
const uint8_t NPAIRS = sizeof(PAIRS) / sizeof(PAIRS[0]);
const uint8_t SCK_PIN = 19;     // pin SCK HX711 pada proyek
const uint8_t SDA_PIN = 21, SCL_PIN = 22;

const char* alasanReset(esp_reset_reason_t r) {
  switch (r) {
    case ESP_RST_POWERON:  return "POWERON (nyala normal)";
    case ESP_RST_EXT:      return "EXT (pin reset)";
    case ESP_RST_SW:       return "SW (esp_restart)";
    case ESP_RST_PANIC:    return "PANIC (crash program)";
    case ESP_RST_INT_WDT:  return "INT_WDT (watchdog interupsi)";
    case ESP_RST_TASK_WDT: return "TASK_WDT (watchdog task)";
    case ESP_RST_WDT:      return "WDT (watchdog lain)";
    case ESP_RST_DEEPSLEEP:return "DEEPSLEEP (bangun)";
    case ESP_RST_BROWNOUT: return "BROWNOUT (tegangan turun!)";
    case ESP_RST_SDIO:     return "SDIO";
    default:               return "UNKNOWN";
  }
}

void infoChip() {
  Serial.println(F("\n=== 1. IDENTITAS CHIP ==="));
  esp_chip_info_t ci;
  esp_chip_info(&ci);
  Serial.printf("Model        : %s\n", ESP.getChipModel());
  Serial.printf("Revisi       : %u  (core 2.x: 0..3, core 3.x: mis. 301 = v3.1)\n", ESP.getChipRevision());
  Serial.printf("Jumlah core  : %u\n", ESP.getChipCores());
  Serial.printf("Fitur        : %s%s%s%s\n",
                (ci.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi " : "",
                (ci.features & CHIP_FEATURE_BT) ? "BT " : "",
                (ci.features & CHIP_FEATURE_BLE) ? "BLE " : "",
                (ci.features & CHIP_FEATURE_EMB_FLASH) ? "flash-di-chip " : "flash-eksternal ");
  Serial.printf("Flash        : %u MB @ %u MHz\n", ESP.getFlashChipSize() / (1024 * 1024),
                ESP.getFlashChipSpeed() / 1000000);
  Serial.printf("PSRAM        : %u byte (0 = tidak ada, normal untuk WROOM-32D)\n", ESP.getPsramSize());
  Serial.printf("CPU          : %u MHz,  XTAL/APB: %u MHz\n", getCpuFrequencyMhz(), getApbFrequency() / 1000000);
  Serial.printf("SDK          : %s\n", ESP.getSdkVersion());
  Serial.printf("Alasan reset : %s\n", alasanReset(esp_reset_reason()));
  Serial.println(F("Catatan: D0WD / D0WD-V3 = cocok WROOM-32D. D0WDQ6 = chip WROOM-32 lama (cek ulang DS)."));
}

// a = pin pengemudi, b = pin pembaca. Dua kondisi dicek supaya pin mengambang tidak lolos.
bool arah(uint8_t a, uint8_t b, bool &lowOk, bool &highOk) {
  pinMode(b, INPUT_PULLUP);
  pinMode(a, OUTPUT);
  digitalWrite(a, LOW);
  delay(5);
  lowOk = (digitalRead(b) == LOW);      // jika tak ada jumper, pull-up membuat b = HIGH -> gagal
  pinMode(b, INPUT_PULLDOWN);
  digitalWrite(a, HIGH);
  delay(5);
  highOk = (digitalRead(b) == HIGH);    // jika tak ada jumper, pull-down membuat b = LOW -> gagal
  pinMode(a, INPUT);
  pinMode(b, INPUT);
  return lowOk && highOk;
}

void loopback() {
  Serial.println(F("\n=== 2. LOOPBACK GPIO (butuh jumper) ==="));
  for (uint8_t i = 0; i < NPAIRS; i++) {
    for (uint8_t d = 0; d < 2; d++) {
      uint8_t a = PAIRS[i][d], b = PAIRS[i][1 - d];
      bool lo, hi;
      bool ok = arah(a, b, lo, hi);
      Serial.printf("GPIO%-2u -> GPIO%-2u : %s  (LOW:%s HIGH:%s)\n", a, b, ok ? "LULUS" : "GAGAL",
                    lo ? "ok" : "x", hi ? "ok" : "x");
    }
  }
  Serial.println(F("GAGAL di kedua arah = jumper belum terpasang. GAGAL satu arah = pin itu bermasalah."));
}

void scanI2C() {
  Serial.println(F("\n=== 3. SCAN I2C (SDA=21, SCL=22) ==="));
  Wire.begin(SDA_PIN, SCL_PIN, 100000);
  uint8_t n = 0;
  for (uint8_t addr = 0x08; addr < 0x78; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("Ditemukan: 0x%02X\n", addr);
      n++;
    }
  }
  if (!n) Serial.println(F("Tidak ada perangkat (normal jika backpack belum dipasang)."));
  Wire.end();
  pinMode(SDA_PIN, INPUT);
  pinMode(SCL_PIN, INPUT);
}

// Mengukur lebar pulsa SCK tertinggi. HX711 masuk power-down jika SCK tinggi > 60 us.
void ujiSCK(const char* label) {
  const uint32_t f = getCpuFrequencyMhz();
  const int N = 50000;
  pinMode(18, INPUT);
  pinMode(SCK_PIN, OUTPUT);
  digitalWrite(SCK_PIN, LOW);

  uint32_t maxA = 0, sumA = 0;           // dengan interupsi aktif (kasus nyata terburuk)
  for (int i = 0; i < N; i++) {
    uint32_t t0 = ESP.getCycleCount();
    digitalWrite(SCK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(SCK_PIN, LOW);
    uint32_t d = ESP.getCycleCount() - t0;
    sumA += d / 16;                       // hindari overflow
    if (d > maxA) maxA = d;
    delayMicroseconds(4);
  }
  uint32_t maxB = 0;                      // dengan interupsi dimatikan sesaat
  for (int i = 0; i < N; i++) {
    noInterrupts();
    uint32_t t0 = ESP.getCycleCount();
    digitalWrite(SCK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(SCK_PIN, LOW);
    uint32_t d = ESP.getCycleCount() - t0;
    interrupts();
    if (d > maxB) maxB = d;
    delayMicroseconds(4);
  }
  float usA = (float)maxA / f, usB = (float)maxB / f;
  Serial.printf("%-22s CPU %3u MHz | SCK tinggi maks: interupsi aktif %7.2f us, dimatikan %6.2f us | rata2 %.2f us | %s\n",
                label, f, usA, usB, ((float)sumA * 16 / N) / f, usA < 50.0f ? "AMAN (<50 us)" : (usA < 60.0f ? "MEPET" : "MELEBIHI 60 us!"));
}

void sckSemua() {
  Serial.println(F("\n=== 4. WAKTU PULSA SCK (batas HX711: 60 us) ==="));
  Serial.println(F("(Pulsa nominal ~1 us. Yang dicari: lonjakan akibat interupsi/Wi-Fi.)"));
  const uint32_t freqs[] = {240, 160, 80};
  for (uint8_t i = 0; i < 3; i++) {
    setCpuFrequencyMhz(freqs[i]);
    ujiSCK("Wi-Fi mati");
  }
  setCpuFrequencyMhz(240);
  WiFi.mode(WIFI_STA);
  WiFi.begin("tidak-ada", "tidak-ada");   // radio dibuat sibuk (scan/coba konek)
  delay(500);
  ujiSCK("Wi-Fi aktif");
  setCpuFrequencyMhz(80);
  ujiSCK("Wi-Fi aktif");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  setCpuFrequencyMhz(240);
  Serial.println(F("Jika 'interupsi aktif' > 50 us saat Wi-Fi aktif: bungkus pembacaan 24 clock dengan noInterrupts()/interrupts()."));
}

void semua() {
  infoChip();
  loopback();
  scanI2C();
  sckSemua();
  Serial.println(F("\n=== SELESAI. Kirim 'r' untuk mengulang. ==="));
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  semua();
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'r' || c == 'R') semua();
  }
}

/*
  HGD bench test - Hand Grip Dynamometer, ESP32 DevKit V1
  --------------------------------------------------------
  Breadboard bring-up sketch. NOT the final firmware (no state machine, no
  logging). It checks each piece of hardware one at a time so you can find
  wiring problems early. Needs one library: "LiquidCrystal I2C" (header LiquidCrystal_I2C.h)
  (Library Manager).

  Pins (desain-pcb-hgd.md, section 2)
    Button   GPIO25  -> GND (internal pull-up, interrupt FALLING)
    LED      GPIO27  -> R1 220 ohm -> LED anode, cathode -> GND
    HX711 DT GPIO18, SCK GPIO19  (HX711 VCC on 3V3, not 5 V)
    LCD SDA  GPIO21, SCL GPIO22  (I2C address 0x27); backpack on VIN 5 V only
             through the BSS138 level shifter (SDA/SCL measured 4.93 V)

  What it tests
    U4  I2C scan at boot, finds the LCD at 0x27
    U7  HX711 response, live raw / kg, tare, noise, saturation, stuck value
    U8  button: debounce, TAP (toggle LED) vs HOLD 1.5 s (re-tare)
    E1  60 s noise test -> sigma and suggested THRESHOLD_ONSET_KG

  Serial Monitor: 115200 baud, line ending "No line ending" or "Newline".
  Commands:  t tare | k avg of 10 raw | p print state | e 60 s noise test
             n flip sign | l toggle LED | s I2C scan | h help
*/

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>   // "LiquidCrystal I2C" (Library Manager)

// ---------------- Pins ----------------
const uint8_t PIN_BTN    = 25;
const uint8_t PIN_LED    = 27;
const uint8_t PIN_HX_DT  = 18;
const uint8_t PIN_HX_SCK = 19;
const uint8_t PIN_SDA    = 21;
const uint8_t PIN_SCL    = 22;

// ---------------- Constants (alur-program-hgd.md, section 3) ----------------
const uint32_t BAUD                = 115200;
const uint32_t I2C_HZ              = 100000;
const uint32_t T_DEBOUNCE_MS       = 50;
const uint32_t T_TAHAN_MS          = 1500;   // HOLD threshold (E4 tunes this)
const uint32_t T_REFRESH_LCD_MS    = 250;    // E3 tunes this
const uint32_t T_TUNGGU_HX711_MS   = 1000;   // no sample for this long = timeout
const uint8_t  T_TARE_SAMPEL       = 10;
const uint8_t  UKURAN_BUF          = 5;
const float    THRESHOLD_STABIL_KG = 0.5f;
const float    SKALA_COUNTS_PER_KG = 53687.0f;  // initial estimate, replaced by calibration
const int32_t  ADC_MAX             = 8388607;
const float    SATURASI_FRAKSI     = 0.95f;
const int32_t  SAT_LIMIT           = (int32_t)(SATURASI_FRAKSI * (float)ADC_MAX);
const uint8_t  STUCK_N             = 10;     // identical raw values in a row = stuck
const uint32_t NOISE_MS            = 60000;  // E1 duration
const uint32_t MSG_MS              = 1500;   // how long button messages stay on the LCD

// ---------------- Types ----------------
enum Galat { GALAT_NONE = 0, HX711_TIMEOUT, ADC_SATURASI };
enum Job   { JOB_NONE = 0, JOB_TARE, JOB_AVG, JOB_NOISE };

// ---------------- LCD ----------------
// Same method as uji_lcd_i2c: scan until the backpack answers, THEN create and init the LCD.
// If the LCD is not ready at boot, loop() keeps retrying every 2 s.
LiquidCrystal_I2C *lcd = nullptr;
uint8_t lcdAddr = 0;
uint8_t i2cScan();   // defined below

bool lcdBegin() {
  uint8_t addr = i2cScan();
  if (addr == 0) return false;
  lcdAddr = addr;
  lcd = new LiquidCrystal_I2C(addr, 16, 2);
  lcd->init();
  lcd->backlight();
  lcd->clear();
  Serial.printf("LCD ready at 0x%02X\n", addr);
  return true;
}

// Re-syncs the LCD controller (HD44780 "initialise by instruction"). Works from any
// state, so a garbled display recovers by itself. Does not clear the screen.
void lcdResync() {
  const uint8_t BL = 0x08, EN = 0x04;                 // backpack: backlight, enable bits
  const uint8_t seq[4]  = {0x30, 0x30, 0x30, 0x20};   // 3,3,3 then 2 = 4-bit mode
  const uint8_t wait[4] = {5, 5, 1, 1};               // ms
  for (uint8_t i = 0; i < 4; i++) {
    Wire.beginTransmission(lcdAddr); Wire.write(seq[i] | BL | EN); Wire.endTransmission();
    delayMicroseconds(2);
    Wire.beginTransmission(lcdAddr); Wire.write(seq[i] | BL);      Wire.endTransmission();
    delay(wait[i]);
  }
  lcd->command(0x28);   // 4-bit, 2 lines
  lcd->command(0x0C);   // display on, cursor off
  lcd->command(0x06);   // entry mode
}

// Prints one line, padded to 16 chars so old text is overwritten.
void lcdLine(uint8_t row, const char* txt) {
  if (!lcd) return;
  char out[17];
  uint8_t i = 0;
  for (; i < 16 && txt[i]; i++) out[i] = txt[i];
  for (; i < 16; i++) out[i] = ' ';
  out[16] = 0;
  lcd->setCursor(0, row);
  lcd->print(out);
}

// ---------------- Button ----------------
volatile bool     btnFlag = false;
volatile uint32_t btnTsInt = 0;
uint32_t tapCount = 0, holdCount = 0;
bool     ledOn = false;
char     msgText[17] = "";
uint32_t msgUntil = 0;

void IRAM_ATTR isrButton() {
  uint32_t now = millis();
  if (now - btnTsInt < T_DEBOUNCE_MS) return;   // software debounce
  btnTsInt = now;
  btnFlag = true;
}

void showMsg(const char* fmt, unsigned long v) {
  snprintf(msgText, sizeof(msgText), fmt, v);
  msgUntil = millis() + MSG_MS;
}

void setLed(bool on) {
  ledOn = on;
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}

// ---------------- HX711 / sensor state ----------------
portMUX_TYPE hxMux = portMUX_INITIALIZER_UNLOCKED;
int32_t  rawNow = 0, rawPrev = 0, offsetCounts = 0;
float    signFactor = 1.0f;       // flip with 'n' if kg goes negative when you press
float    kgNow = 0;
uint32_t sampleCount = 0, lastSampleMs = 0;
uint8_t  sameCount = 0;
bool     saturated = false, stuck = false;
float    buf[UKURAN_BUF];
uint8_t  bufIdx = 0, bufN = 0;

float scaleNow() { return SKALA_COUNTS_PER_KG * signFactor; }

// Reads one conversion. Call only when DT is LOW (data ready).
// 24 clock pulses + 25th pulse = channel A, gain 128 for the next conversion.
int32_t hxRead() {
  uint32_t v = 0;
  portENTER_CRITICAL(&hxMux);
  for (uint8_t i = 0; i < 24; i++) {
    digitalWrite(PIN_HX_SCK, HIGH);
    delayMicroseconds(1);
    v = (v << 1) | (uint32_t)digitalRead(PIN_HX_DT);
    digitalWrite(PIN_HX_SCK, LOW);
    delayMicroseconds(1);
  }
  digitalWrite(PIN_HX_SCK, HIGH);
  delayMicroseconds(1);
  digitalWrite(PIN_HX_SCK, LOW);
  delayMicroseconds(1);
  portEXIT_CRITICAL(&hxMux);
  if (v & 0x800000UL) v |= 0xFF000000UL;   // sign extend 24 -> 32 bit
  return (int32_t)v;
}

void bufAdd(float kg) {
  buf[bufIdx] = kg;
  bufIdx = (bufIdx + 1) % UKURAN_BUF;
  if (bufN < UKURAN_BUF) bufN++;
}

void bufReset() { bufIdx = 0; bufN = 0; }

void windowStats(float &avg, float &spread, bool &full) {
  full = (bufN >= UKURAN_BUF);
  if (bufN == 0) { avg = 0; spread = 0; return; }
  float mn = buf[0], mx = buf[0], s = 0;
  for (uint8_t i = 0; i < bufN; i++) {
    if (buf[i] < mn) mn = buf[i];
    if (buf[i] > mx) mx = buf[i];
    s += buf[i];
  }
  avg = s / bufN;
  spread = mx - mn;
}

Galat currentGalat() {
  if (millis() - lastSampleMs > T_TUNGGU_HX711_MS) return HX711_TIMEOUT;
  if (saturated) return ADC_SATURASI;
  return GALAT_NONE;
}

// ---------------- Jobs: tare / avg of 10 / noise test ----------------
Job      job = JOB_NONE;
uint32_t jobStartMs = 0, jobN = 0;
double   jobSum = 0, jobMean = 0, jobM2 = 0;
int32_t  jobMin = 0, jobMax = 0;

void jobStart(Job j) {
  job = j;
  jobStartMs = millis();
  jobN = 0; jobSum = 0; jobMean = 0; jobM2 = 0;
  jobMin = INT32_MAX; jobMax = INT32_MIN;
}

void jobFeed(int32_t raw) {
  if (job == JOB_NONE) return;
  jobN++;
  jobSum += raw;
  double d = (double)raw - jobMean;
  jobMean += d / (double)jobN;
  jobM2 += d * ((double)raw - jobMean);
  if (raw < jobMin) jobMin = raw;
  if (raw > jobMax) jobMax = raw;

  if (job == JOB_TARE && jobN >= T_TARE_SAMPEL) {
    offsetCounts = (int32_t)lround(jobSum / (double)jobN);
    bufReset();
    Serial.printf("[TARE] offset=%ld counts (avg of %lu samples, spread %ld counts)\n",
                  (long)offsetCounts, (unsigned long)jobN, (long)(jobMax - jobMin));
    job = JOB_NONE;
  } else if (job == JOB_AVG && jobN >= 10) {
    Serial.printf("[AVG10] mean raw=%.1f  min=%ld  max=%ld  spread=%ld counts\n",
                  jobMean, (long)jobMin, (long)jobMax, (long)(jobMax - jobMin));
    job = JOB_NONE;
  } else if (job == JOB_NOISE && (millis() - jobStartMs) >= NOISE_MS) {
    double sd = (jobN > 1) ? sqrt(jobM2 / (double)(jobN - 1)) : 0.0;
    float sc = fabsf(scaleNow());
    float sdKg = (float)sd / sc;
    float ppKg = (float)(jobMax - jobMin) / sc;
    float thr = fmaxf(1.0f, 10.0f * sdKg);
    Serial.println("[E1] noise test done");
    Serial.printf("     samples=%lu  mean raw=%.1f\n", (unsigned long)jobN, jobMean);
    Serial.printf("     sigma=%.1f counts = %.4f kg   peak-to-peak=%ld counts = %.3f kg\n",
                  sd, sdKg, (long)(jobMax - jobMin), ppKg);
    Serial.printf("     suggested THRESHOLD_ONSET_KG = max(1.0, 10*sigma) = %.2f kg\n", thr);
    job = JOB_NONE;
  }
}

// ---------------- Sample handling ----------------
void onSample(int32_t raw) {
  lastSampleMs = millis();
  sampleCount++;
  if (sampleCount > 1 && raw == rawPrev) {
    if (sameCount < 255) sameCount++;
  } else {
    sameCount = 0;
  }
  rawPrev = raw;
  rawNow = raw;
  stuck = (sameCount >= STUCK_N);
  saturated = (abs(raw) >= SAT_LIMIT);
  kgNow = (float)(raw - offsetCounts) / scaleNow();
  bufAdd(kgNow);
  jobFeed(raw);
}

// ---------------- I2C scan ----------------
// Prints every device that answers. Returns 0x27 or 0x3F if found, else 0.
uint8_t i2cScan() {
  Serial.printf("I2C scan (SDA=%u, SCL=%u):\n", PIN_SDA, PIN_SCL);
  uint8_t found = 0, pick = 0;
  for (uint8_t a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  found 0x%02X\n", a);
      found++;
      if (a == 0x27 || (a == 0x3F && pick == 0)) pick = a;
    }
  }
  if (found == 0) Serial.println("  nothing found: check SDA/SCL wires, backpack VCC and GND");
  else if (pick == 0) Serial.println("  no LCD at 0x27 or 0x3F");
  return pick;
}

// ---------------- Button handling ----------------
void onTap() {
  tapCount++;
  setLed(!ledOn);
  Serial.printf("[BTN] TAP #%lu -> LED %s\n", (unsigned long)tapCount, ledOn ? "ON" : "OFF");
  showMsg("BTN TAP #%lu", tapCount);
}

void onHold() {
  holdCount++;
  Serial.printf("[BTN] HOLD #%lu (>= %lu ms) -> re-tare\n",
                (unsigned long)holdCount, (unsigned long)T_TAHAN_MS);
  showMsg("BTN HOLD #%lu", holdCount);
  jobStart(JOB_TARE);
}

// TAP / HOLD classification: sample the pin level each loop (alur-program 4.5).
void buttonUpdate() {
  static bool down = false, holdFired = false;
  static uint32_t tDown = 0, tRelease = 0;
  bool level = (digitalRead(PIN_BTN) == LOW);

  if (btnFlag) {
    btnFlag = false;
    if (!down && level && (millis() - tRelease > T_DEBOUNCE_MS)) {
      down = true;
      holdFired = false;
      tDown = millis();
    }
  }
  if (down) {
    if (level && !holdFired && (millis() - tDown >= T_TAHAN_MS)) {
      holdFired = true;
      onHold();                       // reported once, without waiting for release
    }
    if (!level) {
      if (!holdFired) onTap();
      down = false;
      tRelease = millis();
    }
  }
}

// ---------------- Serial commands ----------------
void printHelp() {
  Serial.println();
  Serial.println("Commands: t tare | k avg of 10 raw | p print state | e 60 s noise test (E1)");
  Serial.println("          n flip sign | l toggle LED | s I2C scan | h help");
  Serial.println("Button:   TAP toggles the LED, HOLD 1.5 s re-tares");
}

void printState() {
  float avg, spr; bool full;
  windowStats(avg, spr, full);
  Serial.printf("[STATE] raw=%ld offset=%ld kg=%+.3f window_avg=%+.3f spread=%.3f sign=%+d scale=%.0f counts/kg\n",
                (long)rawNow, (long)offsetCounts, kgNow, avg, spr,
                (int)signFactor, scaleNow());
}

void serialUpdate() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    switch (c) {
      case 't':
        Serial.println("[CMD] tare: keep the load cell unloaded...");
        jobStart(JOB_TARE);
        break;
      case 'k':
        Serial.println("[CMD] averaging 10 raw samples...");
        jobStart(JOB_AVG);
        break;
      case 'e':
        Serial.println("[E1] noise test for 60 s: load cell mounted, unloaded, untouched. Do not touch the table or cable.");
        jobStart(JOB_NOISE);
        break;
      case 'p':
        printState();
        break;
      case 'n':
        signFactor = -signFactor;
        bufReset();
        Serial.printf("[CMD] sign flipped, now %+d\n", (int)signFactor);
        break;
      case 'l':
        setLed(!ledOn);
        Serial.printf("[CMD] LED %s\n", ledOn ? "ON" : "OFF");
        break;
      case 's':
        i2cScan();
        break;
      case 'h':
      case '?':
        printHelp();
        break;
      default:
        break;   // ignore \r \n and anything else
    }
  }
}

// ---------------- Display ----------------
void refreshOutputs() {
  static uint32_t lastSync = 0;
  if (lcd && millis() - lastSync >= 2000) {   // self-heal a garbled LCD
    lastSync = millis();
    lcdResync();
  }
  float avg, spr; bool full;
  windowStats(avg, spr, full);
  bool stable = full && (spr <= THRESHOLD_STABIL_KG);
  Galat g = currentGalat();

  char l1[24], l2[24];
  if (g == HX711_TIMEOUT) {
    snprintf(l1, sizeof(l1), "ERR:HX711 TMOUT");
    snprintf(l2, sizeof(l2), "DT18 SCK19 3V3");
    Serial.println("ERR: HX711_TIMEOUT (no sample for 1 s). Check DT=GPIO18, SCK=GPIO19, VCC=3V3, GND.");
  } else if (g == ADC_SATURASI) {
    snprintf(l1, sizeof(l1), "ERR:ADC SATURASI");
    snprintf(l2, sizeof(l2), "RAW %ld", (long)rawNow);
    Serial.printf("ERR: ADC_SATURASI raw=%ld (check load cell wiring / floating inputs)\n", (long)rawNow);
  } else {
    snprintf(l1, sizeof(l1), "%+7.2f kg %-4s", avg, stable ? "STBL" : "");
    if (millis() < msgUntil) {
      snprintf(l2, sizeof(l2), "%s", msgText);
    } else if (job == JOB_NOISE) {
      snprintf(l2, sizeof(l2), "E1 noise %2lus", (unsigned long)((millis() - jobStartMs) / 1000));
    } else if (job == JOB_TARE) {
      snprintf(l2, sizeof(l2), "Taring...");
    } else if (stuck) {
      snprintf(l2, sizeof(l2), "WARN: STUCK");
    } else {
      snprintf(l2, sizeof(l2), "RAW %ld", (long)rawNow);
    }
    Serial.printf("raw=%ld  kg=%+.3f  win_avg=%+.3f  spread=%.3f%s%s\n",
                  (long)rawNow, kgNow, avg, spr,
                  stable ? "  STABLE" : "",
                  stuck ? "  WARN:STUCK(same raw value repeated)" : "");
  }
  lcdLine(0, l1);
  lcdLine(1, l2);
}

// ---------------- Arduino entry points ----------------
void setup() {
  Serial.begin(BAUD);
  delay(300);
  Serial.println();
  Serial.println("=== HGD bench test ===");

  pinMode(PIN_LED, OUTPUT);
  setLed(false);
  pinMode(PIN_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN), isrButton, FALLING);

  // HX711: DT with pull-up so a disconnected module reads "not ready"
  pinMode(PIN_HX_DT, INPUT_PULLUP);
  pinMode(PIN_HX_SCK, OUTPUT);
  digitalWrite(PIN_HX_SCK, LOW);

  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.setClock(I2C_HZ);
  if (lcdBegin()) {
    lcdLine(0, "HGD bench test");
    lcdLine(1, "LED blink x3");
  }

  for (uint8_t i = 0; i < 3; i++) {
    setLed(true);  delay(250);
    setLed(false); delay(250);
  }

  uint32_t t0 = millis();
  while (digitalRead(PIN_HX_DT) == HIGH && (millis() - t0) < T_TUNGGU_HX711_MS) delay(1);
  if (digitalRead(PIN_HX_DT) == HIGH) {
    Serial.println("HX711: NO RESPONSE (DT stays high). Check wiring and 3V3 supply.");
  } else {
    Serial.println("HX711: DT went low, module responds.");
  }
  Serial.println("Note: with the load cell not connected the HX711 inputs float and readings are meaningless.");

  lastSampleMs = millis();
  jobStart(JOB_TARE);   // auto-tare on boot (re-tare with 't' or by holding the button)
  printHelp();
}

void loop() {
  static uint32_t lastLcdTry = 0;
  if (!lcd && millis() - lastLcdTry >= 2000) {   // LCD not up yet: retry every 2 s
    lastLcdTry = millis();
    if (lcdBegin()) lcdLine(0, "HGD bench test");
  }

  buttonUpdate();
  serialUpdate();

  if (digitalRead(PIN_HX_DT) == LOW) {   // data ready, non-blocking
    onSample(hxRead());
  }

  static uint32_t lastRefresh = 0;
  uint32_t now = millis();
  if (now - lastRefresh >= T_REFRESH_LCD_MS) {
    lastRefresh = now;
    refreshOutputs();
  }
}

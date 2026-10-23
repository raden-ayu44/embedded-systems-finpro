// fsm.cpp - lihat fsm.h.
//
// Langkah 5 (kerangka): semua state, aksi masuk, layar (Bagian 10.2), aturan LED (Bagian 10.3),
// transisi pengecualian galat sensor, dan reset total. Transisi masih sederhana:
// - GRIP: onset -> fase 2; plateau pada t >= 3 s -> COMPUTE.
//   Langkah 6 menambah: dilepas sebelum 3 s (kembali ke fase 1) dan batas 5 s.
// - REST: setelah genggaman dilepas langsung menunggu TAP (NEXT).
//   Langkah 7 menambah hitung mundur istirahat REST_S.
// - Langkah 8 menambah cek rentang (OUT_OF_RANGE) dan modul galat; langkah 9 baris log + ACK.
#include "fsm.h"
#include "button.h"
#include "display.h"
#include "sensor.h"
#include "led.h"

// ---------------- Data state machine (semua statis, tanpa malloc) ----------------
static State     state = READY;
static Session   session;
static GripPhase gripPhase = WAIT_ONSET;
static RestPhase restPhase = RELEASE_REST;
static uint32_t  gripStartMs = 0;          // millis() saat onset (fase 2 mulai, timer 3-5 s)
static uint32_t  gripDurMs = 0;            // lama fase 2 saat keluar GRIP
static GripExit  gripExit = EXIT_NONE;     // jalur keluar GRIP terakhir
static bool      restartPending = false;   // true = masuk READY berikutnya adalah restart (kedip)
static FaultType activeFault = FAULT_NONE; // galat yang membuat masuk FAULT
static bool      countdownShown = false;   // layar hitung mundur reset total sedang tampil
static uint32_t  pageStartMs = 0;          // millis() saat masuk SAVE_FAILED (ganti halaman)

const char* stateName(State s) {
  switch (s) {
    case READY:       return "READY";
    case GRIP:        return "GRIP";
    case COMPUTE:     return "COMPUTE";
    case REST:        return "REST";
    case SUMMARY:     return "SUMMARY";
    case FAULT:       return "FAULT";
    case SAVE_FAILED: return "SAVE_FAILED";
    default:          return "?";
  }
}

State fsmState() { return state; }

// State di dalam superstate "siklus percobaan" (Bagian 1.3). Hanya di sini galat sensor
// memicu transisi pengecualian ke FAULT. SUMMARY dan SAVE_FAILED di luar siklus.
static bool inTrialCycle(State s) {
  return s == READY || s == GRIP || s == COMPUTE || s == REST;
}

// Sesi baru (Bagian 5.20): simpan nomor lama, kosongkan struct, pasang nomor lama + 1.
// Nomor sesi tidak pernah kembali ke 1 selama alat menyala.
static void fsmNewSession() {
  uint16_t nextNumber = session.number + 1;
  memset(&session, 0, sizeof(session));
  session.number = nextNumber;
  session.trialNo = 1;
  session.t0Ms = millis();
  Serial.printf("[FSM] sesi %u\n", session.number);
}

// ---------------- Aksi masuk tiap state ----------------
static void enterReady() {
  buttonReset();             // buang tekanan lama; tekanan yang sedang berlangsung diabaikan
  if (restartPending) {      // restart: power-up, SUMMARY->TAP, FAULT->HOLD, SAVE_FAILED->HOLD, reset total
    ledBlinkStart();
    restartPending = false;
  } else {                   // percobaan berikutnya: tanpa kedip
    ledSet(false);
  }
  sensorStartTare();         // auto-tare tiap masuk READY (10 sampel, tidak memblokir)
}

static void enterGrip() {
  buttonReset();             // tekanan selama GRIP tidak berarti apa-apa
  gripPhase = WAIT_ONSET;
  gripExit = EXIT_NONE;
  ledSet(false);             // fase 1: LED mati
}

static void enterCompute() {
  // One-shot, tidak ada aksi masuk; semua dikerjakan handleCompute().
}

static void enterRest() {
  buttonReset();
  restPhase = RELEASE_REST;
  // LED tidak diubah: bila GRIP keluar lewat plateau, LED tetap menyala sampai dilepas.
}

static void enterSummary() {
  buttonReset();
  ledSet(false);
  // Puncak dan rerata dari tiga ref (Bagian 5.14).
  float peak = session.trials[0].ref;
  float sum = 0.0f;
  for (uint8_t i = 0; i < TRIALS_PER_SESSION; i++) {
    if (session.trials[i].ref > peak) peak = session.trials[i].ref;
    sum += session.trials[i].ref;
  }
  session.peak = peak;
  session.avg = sum / TRIALS_PER_SESSION;
  Serial.printf("[SUMMARY] puncak=%.2f rerata=%.2f kg\n", session.peak, session.avg);
}

static void enterFault() {
  buttonReset();
  ledSet(false);             // keadaan aman: LED dipaksa mati, pengukuran berhenti
  Serial.printf("[FAULT] %s raw=%ld\n", faultName(activeFault), (long)sensorRaw());
}

static void enterSaveFailed() {
  buttonReset();
  ledSet(false);
  pageStartMs = millis();
}

// Pindah state: cetak transisi, jalankan aksi masuk, minta layar baru.
// Dipanggil juga untuk masuk ulang state yang sama (reset total saat sudah di READY).
static void changeState(State next) {
  Serial.printf("[FSM] %s -> %s\n", stateName(state), stateName(next));
  state = next;
  switch (state) {
    case READY:       enterReady();      break;
    case GRIP:        enterGrip();       break;
    case COMPUTE:     enterCompute();    break;
    case REST:        enterRest();       break;
    case SUMMARY:     enterSummary();    break;
    case FAULT:       enterFault();      break;
    case SAVE_FAILED: enterSaveFailed(); break;
  }
  displayRequest();
}

// ---------------- Handler tiap state: mengembalikan state berikut ----------------
static State handleReady(Press press) {
  if (press != PRESS_TAP) return READY;
  if (ledBlinking()) ledSet(false);         // TAP menghentikan kedip restart seketika
  if (sensorTareDone()) return GRIP;        // START hanya bila tare sudah selesai
  return READY;                             // TAP saat "Menyesuaikan nol" hanya menghentikan kedip
}

static State handleGrip() {
  float kg = sensorKg();

  if (gripPhase == WAIT_ONSET) {
    // Fase 1: timer belum jalan; tunggu gaya melewati onset.
    if (kg > THRESHOLD_ONSET_KG) {
      gripPhase = MEASURING;
      gripStartMs = millis();
      sensorBufReset();                     // buffer plateau mulai kosong di awal fase 2
      displayRequest();
      Serial.printf("[GRIP] onset kg=%.2f\n", kg);
    }
    return GRIP;
  }

  // Fase 2: LED = plateau (R4: LED dikendalikan kestabilan gaya yang terukur).
  bool plateau = sensorPlateau();
  ledSet(plateau);
  uint32_t t = millis() - gripStartMs;

  if (plateau && t >= GRIP_MIN_MS) {        // keluar lewat plateau: LED tetap menyala
    gripExit = EXIT_PLATEAU;
    gripDurMs = t;
    Serial.printf("[GRIP] exit=PLATEAU t=%lu ms\n", (unsigned long)t);
    return COMPUTE;
  }
  return GRIP;
}

static State handleCompute() {
  // ref = rata-rata buffer plateau, presisi penuh (pembulatan hanya di LCD).
  Trial& tr = session.trials[session.trialNo - 1];
  tr.ref = sensorBufAvg();
  tr.valid = true;                          // validasi rentang ditambahkan di langkah 8
  tr.durMs = gripDurMs;
  tr.exitBy = gripExit;
  Serial.printf("[COMPUTE] percobaan %u ref=%.2f kg\n", session.trialNo, tr.ref);
  return REST;
}

static State handleRest(Press press) {
  if (restPhase == RELEASE_REST) {
    // Sub-fase 1: tunggu genggaman dilepas. Saat itu LED mati.
    if (sensorKg() < THRESHOLD_ONSET_KG) {
      ledSet(false);
      restPhase = WAIT_NEXT;
      buttonReset();                        // sub-fase 2 menunggu tombol: mulai bersih
      displayRequest();
      Serial.println("[REST] dilepas");
    }
    return REST;
  }

  // Sub-fase 2: tunggu TAP (NEXT).
  if (press != PRESS_TAP) return REST;
  if (session.trialNo < TRIALS_PER_SESSION) {
    session.trialNo++;
    return READY;                           // percobaan berikutnya: tanpa kedip
  }
  return SUMMARY;
}

static State handleSummary(Press press) {
  if (press != PRESS_TAP) return SUMMARY;
  fsmNewSession();
  restartPending = true;                    // RESTART: READY dengan kedip
  return READY;
}

static State handleFault(Press press) {
  if (press != PRESS_HOLD) return FAULT;
  // TAHAN 1,5 s: kembali ke READY dengan kedip. Sesi dan nomor percobaan tetap,
  // percobaan yang gagal tidak dihitung (diulang).
  activeFault = FAULT_NONE;
  restartPending = true;
  return READY;
}

static State handleSaveFailed(Press press) {
  if (press != PRESS_HOLD) return SAVE_FAILED;
  fsmNewSession();
  restartPending = true;
  return READY;
}

// ---------------- Layar ----------------
// Teks per state dari Bagian 10.2. Dipanggil tiap putaran; menggambar hanya bila displayDue().
static void drawScreen() {
  if (!displayDue()) return;
  char l0[24], l1[24];   // lebih dari 16: lcdLine() memotong dan mengisi spasi

  if (countdownShown) {  // hitung mundur reset total menutupi layar state
    uint32_t leftS = (FORCE_RESET_MS - buttonHeldMs() + 999) / 1000;   // dibulatkan ke atas
    snprintf(l0, sizeof(l0), "Reset total %lu", (unsigned long)leftS);
    snprintf(l1, sizeof(l1), "Lepas = batal");
    lcdLine(0, l0);
    lcdLine(1, l1);
    return;
  }

  // Teks mengikuti Tabel 4 proposal (sama dengan firmware tim lainnya); semua <= 16 karakter.
  // Angka ditulis dengan lebar tetap (%5.1f dst.) supaya posisi tidak bergeser saat nilai berubah.
  const Trial& tr = session.trials[session.trialNo - 1];
  switch (state) {
    case READY:   // "SIAP - UKUR 1/3" / "Menyesuaikan nol" lalu "Tekan tmbl START"
      snprintf(l0, sizeof(l0), "SIAP - UKUR %u/%u", session.trialNo, TRIALS_PER_SESSION);
      snprintf(l1, sizeof(l1), "%s", sensorTareDone() ? "Tekan tmbl START" : "Menyesuaikan nol");
      break;
    case GRIP:
      if (gripPhase == WAIT_ONSET) {   // "Menunggu genggam" / " 0.4 kg" (kg live)
        snprintf(l0, sizeof(l0), "Menunggu genggam");
        snprintf(l1, sizeof(l1), "%5.1f kg", sensorKg());
      } else {   // "Tahan 3-5 detik" / " 25.3 kg  3.4s" (kg live + detik sejak onset)
        snprintf(l0, sizeof(l0), "Tahan 3-5 detik");
        snprintf(l1, sizeof(l1), "%5.1f kg  %.1fs", sensorKg(), (millis() - gripStartMs) / 1000.0f);
      }
      break;
    case COMPUTE:
      return;   // one-shot tanpa layar
    case REST:    // "Hasil 47.23 kg" / "Lepas>Rest 60s" lalu "NEXT > lanjut"
      snprintf(l0, sizeof(l0), "Hasil %5.2f kg", tr.ref);
      if (restPhase == RELEASE_REST) {
        // Langkah 7: setelah dilepas, hitung mundur detik bulat "Lepas>Rest %2lus".
        snprintf(l1, sizeof(l1), "Lepas>Rest %lus", (unsigned long)REST_S);
      } else {
        snprintf(l1, sizeof(l1), "%s",
                 session.trialNo < TRIALS_PER_SESSION ? "NEXT > lanjut" : "NEXT > ringkasan");
      }
      break;
    case SUMMARY: // "Mx47.2 Av44.8 kg" / "Tekan: RESTART"
      snprintf(l0, sizeof(l0), "Mx%4.1f Av%4.1f kg", session.peak, session.avg);
      snprintf(l1, sizeof(l1), "Tekan: RESTART");
      break;
    case FAULT:
      switch (activeFault) {
        case HX711_TIMEOUT:   snprintf(l0, sizeof(l0), "ERR:HX711 TMOUT");  break;
        case ADC_SATURATED:   snprintf(l0, sizeof(l0), "ERR:ADC SATURASI"); break;
        case OUT_OF_RANGE:    snprintf(l0, sizeof(l0), "ERR:DI LUAR RNTG"); break;
        case READING_INVALID: snprintf(l0, sizeof(l0), "ERR:BACA TDK VLD"); break;
        default:              snprintf(l0, sizeof(l0), "ERR");              break;
      }
      snprintf(l1, sizeof(l1), "RAW %ld", (long)sensorRaw());   // raw live untuk pengawas
      break;
    case SAVE_FAILED:
      // Dua halaman bergantian tiap SAVE_FAILED_PAGE_MS (3 s) supaya semua nilai bisa disalin.
      if (((millis() - pageStartMs) / SAVE_FAILED_PAGE_MS) % 2 == 0) {
        snprintf(l0, sizeof(l0), "P1 %.1f P2 %.1f", session.trials[0].ref, session.trials[1].ref);
        snprintf(l1, sizeof(l1), "P3 %.1f   GAGAL", session.trials[2].ref);
      } else {
        snprintf(l0, sizeof(l0), "Pk %.1f Av %.1f", session.peak, session.avg);
        snprintf(l1, sizeof(l1), "GAGAL SIMPAN");
      }
      break;
    default:
      return;
  }
  lcdLine(0, l0);
  lcdLine(1, l1);
}

// ---------------- Titik masuk ----------------
void fsmInit() {
  memset(&session, 0, sizeof(session));
  fsmNewSession();          // sesi 1
  restartPending = true;    // nyala pertama = restart: READY dengan kedip
  state = READY;
  Serial.println("[FSM] mulai -> READY");
  enterReady();
  displayRequest();
}

void fsmRun(Press press) {
  // (1) Reset total: tombol ditahan FORCE_RESET_MS di state apa pun (Bagian 6.2).
  //     Dicek paling awal supaya berlaku juga di FAULT dan SAVE_FAILED.
  static bool forceDone = false;            // sudah dijalankan untuk tekanan ini
  uint32_t held = buttonHeldMs();
  if (held == 0) {                          // tombol dilepas
    forceDone = false;
    if (countdownShown) {                   // batal: layar state kembali
      countdownShown = false;
      displayRequest();
    }
  } else if (!forceDone && held >= FORCE_RESET_MS) {
    forceDone = true;
    countdownShown = false;
    Serial.println("[FSM] RESET TOTAL");
    fsmNewSession();                        // sesi berjalan dibuang, nomor sesi naik
    restartPending = true;
    changeState(READY);                     // masuk ulang READY walau sudah di READY
    return;
  } else if (!forceDone && held >= FORCE_RESET_SHOW_MS && !countdownShown) {
    countdownShown = true;                  // mulai detik ke-3: tampilkan hitung mundur
    displayRequest();
  }

  // (2) Transisi pengecualian: galat sensor di dalam siklus percobaan -> FAULT.
  if (inTrialCycle(state)) {
    FaultType f = sensorCheck();
    if (f != FAULT_NONE) {
      activeFault = f;
      changeState(FAULT);
      drawScreen();
      return;
    }
  }

  // (3) Handler state aktif; bila state berikut berbeda, jalankan aksi masuknya.
  State next = state;
  switch (state) {
    case READY:       next = handleReady(press);      break;
    case GRIP:        next = handleGrip();            break;
    case COMPUTE:     next = handleCompute();         break;
    case REST:        next = handleRest(press);       break;
    case SUMMARY:     next = handleSummary(press);    break;
    case FAULT:       next = handleFault(press);      break;
    case SAVE_FAILED: next = handleSaveFailed(press); break;
  }
  if (next != state) changeState(next);

  // (4) Layar.
  drawScreen();
}

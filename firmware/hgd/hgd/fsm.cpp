// fsm.cpp - lihat fsm.h.
//
// Langkah 5 (kerangka): semua state, aksi masuk, layar (Bagian 10.2), aturan LED (Bagian 10.3),
// transisi pengecualian galat sensor, dan reset total. Transisi masih sederhana:
// - GRIP (langkah 6): onset -> fase 2; dilepas sebelum 3 s -> fase 1; plateau genggaman pada
//   t >= 3 s -> COMPUTE; batas 5 s -> COMPUTE dengan jendela buffer terkuat.
// - REST (langkah 7): tunggu dilepas -> hitung mundur REST_S (tombol diabaikan, digenggam lagi = jeda)
//   -> tunggu TAP (NEXT).
// - Langkah 5c: penjaga nol di READY (kg > +1 = START diabaikan, kg < -1 = tare ulang), di GRIP
//   fase 1 (kg < -1 = kembali ke READY), dan TAHAN di READY = tare ulang manual.
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
static bool      restCounting = false;     // sub-fase 1: genggaman sudah dilepas sekali, istirahat dimulai
static bool      restPaused = false;       // hitung mundur dijeda karena digenggam lagi (kg > onset)
static uint32_t  restStartMs = 0;          // millis() awal potongan istirahat yang sedang berjalan
static uint32_t  restDoneMs = 0;           // lama istirahat dari potongan sebelumnya (sebelum jeda)

// Lama istirahat yang sudah dijalani: potongan lama + potongan yang sedang berjalan (bila tidak dijeda).
static uint32_t restElapsedMs() {
  if (!restCounting) return 0;
  if (restPaused) return restDoneMs;
  return restDoneMs + (millis() - restStartMs);
}
static uint32_t  gripStartMs = 0;          // millis() saat onset (fase 2 mulai, timer 3-5 s)
static uint32_t  gripDurMs = 0;            // lama fase 2 saat keluar GRIP
static GripExit  gripExit = EXIT_NONE;     // jalur keluar GRIP terakhir
static uint32_t  gripLastSampleMs = 0;     // waktu sampel terakhir yang sudah dinilai di fase 2
static float     bestAvg = 0.0f;           // rata-rata tertinggi jendela 5 sampel di atas onset (batas 5 s)
static bool      bestFound = false;        // sudah ada jendela genggaman yang dinilai
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
  buttonReset();             // buang tekanan lama; di GRIP hanya TAHAN di fase 1 yang berarti (tare ulang)
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
  restCounting = false;      // hitung mundur baru mulai saat genggaman dilepas
  restPaused = false;
  restDoneMs = 0;
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
// Tare ulang dari READY: layar kembali "Menyesuaikan nol" sampai 10 sampel baru terkumpul.
static void readyRetare(const char* why) {
  Serial.printf("[READY] tare ulang: %s\n", why);
  sensorStartTare();
  displayRequest();
}

static State handleReady(Press press) {
  if (press == PRESS_TAP && ledBlinking()) ledSet(false);   // TAP menghentikan kedip restart seketika

  // TAHAN 1,5 s = tare ulang manual (reset total 15 s tetap berlaku di atasnya, lihat fsmRun).
  if (press == PRESS_HOLD) {
    readyRetare("manual (TAHAN)");
    return READY;
  }
  if (!sensorTareDone()) return READY;      // TAP saat "Menyesuaikan nol" hanya menghentikan kedip

  // Penjaga nol (langkah 5c). Tanpa beban kg seharusnya ~0 (terukur: alat diangkat/diputar
  // menggeser < 0,01 kg). Jauh di bawah nol = tare diambil saat alat sedang digenggam:
  // genggaman itu sudah dilepas, jadi nol salah dan semua hasil akan terlalu kecil -> tare ulang.
  float kg = sensorKg();
  if (kg < -THRESHOLD_ONSET_KG) {
    readyRetare("kg di bawah -1, nol diambil saat dibebani");
    return READY;
  }
  // Masih digenggam (kg > +1): START diabaikan, layar "Lepas genggaman".
  if (press == PRESS_TAP && kg <= THRESHOLD_ONSET_KG) return GRIP;
  return READY;
}

static State handleGrip(Press press) {
  float kg = sensorKg();

  if (gripPhase == WAIT_ONSET) {
    // Fase 1: timer belum jalan; tunggu gaya melewati onset.
    // TAHAN 1,5 s (langkah 6c): tare ulang DI TEMPAT, tetap di fase 1. Baris 2 LCD menampilkan
    // "Menyesuaikan nol" sampai tare selesai (~1 s), lalu kg live lagi.
    // Hanya di fase 1; di fase 2 tombol tidak berarti apa-apa (pengukuran sedang berjalan).
    if (press == PRESS_HOLD) {
      Serial.println("[GRIP] TAHAN di fase 1: tare ulang");
      sensorStartTare();
      displayRequest();
      return GRIP;
    }
    // Selama tare kg masih memakai offset lama: onset dan penjaga nol belum boleh dinilai.
    if (!sensorTareDone()) return GRIP;
    // Penjaga nol (langkah 5c): kg < -1 = tare diambil saat digenggam dan genggaman itu baru
    // dilepas sekarang (orang menggenggam terus sampai START). Kembali ke READY = tare ulang.
    if (kg < -THRESHOLD_ONSET_KG) {
      Serial.printf("[GRIP] kg=%.2f di bawah -1, nol salah: kembali ke READY\n", kg);
      return READY;
    }
    if (kg > THRESHOLD_ONSET_KG) {
      gripPhase = MEASURING;
      gripStartMs = millis();
      sensorBufReset();                     // buffer plateau mulai kosong di awal fase 2
      bestFound = false;
      gripLastSampleMs = sensorLastSampleMs();
      displayRequest();
      Serial.printf("[GRIP] onset kg=%.2f\n", kg);
    }
    return GRIP;
  }

  // Fase 2. Waktu sejak onset.
  uint32_t t = millis() - gripStartMs;

  // Jalur 1: dilepas sebelum 3 s (kg turun di bawah onset) -> kembali ke fase 1, percobaan
  // tidak dihitung. Setelah 3 s ambang ini tidak dipakai lagi: akhir ditentukan plateau atau 5 s.
  if (kg < THRESHOLD_ONSET_KG && t < GRIP_MIN_MS) {
    gripPhase = WAIT_ONSET;
    sensorBufReset();
    ledSet(false);
    displayRequest();
    Serial.printf("[GRIP] exit=EARLY_RELEASE t=%lu ms, kembali menunggu\n", (unsigned long)t);
    return GRIP;
  }

  // Plateau GENGGAMAN: buffer stabil DAN semua isinya di atas onset. Tanpa syarat kedua, tangan
  // yang sudah lepas (kg ~0, sangat stabil) terbaca sebagai plateau dan ref menjadi ~0 kg.
  bool gripWindow = sensorBufFull() && sensorBufMin() > THRESHOLD_ONSET_KG;
  bool plateau = gripWindow && sensorPlateau();

  // Catat jendela TERKUAT (rata-rata tertinggi), sekali per sampel baru (bukan per putaran loop).
  // Kekuatan genggaman adalah nilai maksimum, jadi bila tidak ada plateau dipakai jendela terkuat.
  if (sensorLastSampleMs() != gripLastSampleMs) {
    gripLastSampleMs = sensorLastSampleMs();
    if (gripWindow) {
      float avg = sensorBufAvg();
      if (!bestFound || avg > bestAvg) {
        bestFound = true;
        bestAvg = avg;
      }
    }
  }

  // LED = plateau (R4: LED dikendalikan kestabilan gaya yang terukur).
  ledSet(plateau);

  // Jalur 2: plateau pada t >= 3 s. LED tetap menyala sampai genggaman dilepas di REST.
  if (plateau && t >= GRIP_MIN_MS) {
    gripExit = EXIT_PLATEAU;
    gripDurMs = t;
    Serial.printf("[GRIP] exit=PLATEAU t=%lu ms sebaran=%.2f kg\n",
                  (unsigned long)t, sensorBufSpread());
    return COMPUTE;
  }

  // Jalur 3: batas 5 s tanpa plateau (force-stop, bukan pembatalan). LED mati.
  if (t >= GRIP_MAX_MS) {
    gripExit = EXIT_TIMEOUT;
    gripDurMs = t;
    ledSet(false);
    Serial.printf("[GRIP] exit=TIMEOUT t=%lu ms jendela terkuat rata=%.2f kg\n",
                  (unsigned long)t, bestAvg);
    return COMPUTE;
  }
  return GRIP;
}

static State handleCompute() {
  // ref = rata-rata buffer plateau, presisi penuh (pembulatan hanya di LCD).
  Trial& tr = session.trials[session.trialNo - 1];
  // PLATEAU: rata-rata buffer sekarang (plateau yang baru terjadi).
  // TIMEOUT: rata-rata jendela terkuat selama fase 2 ("nilai buffer terbaik", Bagian 5.11).
  //          Selalu ada setelah >= 3 s di atas onset; bila tidak ada, pakai buffer sekarang.
  if (gripExit == EXIT_TIMEOUT && bestFound) tr.ref = bestAvg;
  else                                        tr.ref = sensorBufAvg();
  tr.valid = true;                          // validasi rentang ditambahkan di langkah 8
  tr.durMs = gripDurMs;
  tr.exitBy = gripExit;
  Serial.printf("[COMPUTE] percobaan %u ref=%.2f kg (%s)\n", session.trialNo, tr.ref,
                gripExit == EXIT_TIMEOUT ? "TIMEOUT" : "PLATEAU");
  return REST;
}

static State handleRest(Press press) {
  if (restPhase == RELEASE_REST) {
    // Sub-fase 1a: tunggu genggaman dilepas. Saat itu LED mati dan hitung mundur mulai.
    if (!restCounting) {
      if (sensorKg() < THRESHOLD_ONSET_KG) {
        ledSet(false);
        restCounting = true;
        restStartMs = millis();
        displayRequest();
        Serial.printf("[REST] dilepas, istirahat %lu s\n", (unsigned long)REST_S);
      }
      return REST;
    }
    // Sub-fase 1b: hitung mundur REST_S dari millis() (tanpa delay). Tombol diabaikan.
    // Digenggam lagi (kg > onset) = jeda: angka berhenti; dilepas lagi = lanjut dari angka yang sama.
    // Alasan: istirahat untuk pemulihan otot; waktu menggenggam tidak dihitung sebagai istirahat.
    float kg = sensorKg();
    if (!restPaused && kg > THRESHOLD_ONSET_KG) {
      restDoneMs += millis() - restStartMs;   // simpan potongan yang sudah dijalani
      restPaused = true;
      Serial.printf("[REST] digenggam lagi, jeda (sisa %lu ms)\n",
                    (unsigned long)(REST_S * 1000UL - restDoneMs));
    } else if (restPaused && kg < THRESHOLD_ONSET_KG) {
      restStartMs = millis();                 // potongan baru mulai sekarang
      restPaused = false;
      Serial.println("[REST] dilepas lagi, lanjut");
    }
    if (restElapsedMs() >= REST_S * 1000UL) {
      restPhase = WAIT_NEXT;
      buttonReset();                        // sub-fase 2 menunggu tombol: tekanan lama dibuang
      displayRequest();
      Serial.println("[REST] istirahat selesai");
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
    case READY:   // "SIAP - UKUR 1/3" / "Menyesuaikan nol", "Lepas genggaman" atau "Tekan tmbl START"
      snprintf(l0, sizeof(l0), "SIAP - UKUR %u/%u", session.trialNo, TRIALS_PER_SESSION);
      if (!sensorTareDone())                     snprintf(l1, sizeof(l1), "Menyesuaikan nol");
      else if (sensorKg() > THRESHOLD_ONSET_KG)  snprintf(l1, sizeof(l1), "Lepas genggaman");
      else                                       snprintf(l1, sizeof(l1), "Tekan tmbl START");
      break;
    case GRIP:
      if (gripPhase == WAIT_ONSET) {   // "Menunggu genggam" / " 0.42 kg  >1.0" (kg live + target onset)
        // Dua desimal dan target supaya penggenggam melihat tekanannya bergerak dan seberapa jauh
        // lagi sampai onset (dengan satu desimal tekanan kecil tetap tampil "0.0").
        snprintf(l0, sizeof(l0), "Menunggu genggam");
        if (!sensorTareDone()) snprintf(l1, sizeof(l1), "Menyesuaikan nol");   // tare ulang (TAHAN)
        else snprintf(l1, sizeof(l1), "%5.2f kg  >%.1f", sensorKg(), THRESHOLD_ONSET_KG);
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
        // Sebelum dilepas: angka tetap REST_S. Setelah dilepas (langkah 7): sisa detik, dibulatkan
        // ke atas (59,2 s tampil "60s", 0,4 s tampil " 1s"), digambar ulang tiap LCD_REFRESH_MS.
        // Saat dijeda angkanya berhenti (restElapsedMs() tidak bertambah).
        uint32_t elapsed = restElapsedMs();
        uint32_t total = REST_S * 1000UL;
        uint32_t leftS = (elapsed >= total) ? 0 : (total - elapsed + 999) / 1000;
        snprintf(l1, sizeof(l1), "Lepas>Rest %2lus", (unsigned long)leftS);
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
    case GRIP:        next = handleGrip(press);       break;
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

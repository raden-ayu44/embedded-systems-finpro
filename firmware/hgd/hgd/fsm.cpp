// fsm.cpp - lihat fsm.h.
//
// Langkah 5 (kerangka): semua state, aksi masuk, layar (Bagian 10.2), aturan LED (Bagian 10.3),
// transisi pengecualian galat sensor, dan reset total. Transisi masih sederhana:
// - GRIP (langkah 6, 6d): onset -> fase 2; dilepas sebelum 3 s -> fase 1; dilepas antara 3 dan 5 s
//   -> COMPUTE (pengguna yang memutuskan); masih digenggam pada 5 s -> COMPUTE. ref = jendela stabil
//   terkuat, atau jendela terkuat bila tidak pernah stabil.
// - REST (langkah 7): tunggu dilepas -> hitung mundur REST_S (tombol diabaikan, digenggam lagi = jeda)
//   -> tunggu TAP (NEXT).
// - Langkah 5c: penjaga nol di READY (kg > +1 = START diabaikan, kg < -1 = tare ulang), di GRIP
//   fase 1 (kg < -1 = kembali ke READY), dan TAHAN di READY = tare ulang manual.
// - Langkah 8: cek rentang OUT_OF_RANGE live di GRIP dan pada ref di COMPUTE; keadaan aman, layar
//   galat dan baris E di modul fault.cpp.
// - Langkah 9: SUMMARY mengirim 3 baris T + 1 baris S (serial_log), menunggu ACK; gagal -> SAVE_FAILED.
#include "fsm.h"
#include "button.h"
#include "display.h"
#include "sensor.h"
#include "led.h"
#include "fault.h"
#include "serial_log.h"

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
static float     bestAvg = 0.0f;           // rata-rata tertinggi jendela 5 sampel di atas onset
static bool      bestFound = false;        // sudah ada jendela genggaman yang dinilai
static float     bestSteadyAvg = 0.0f;     // rata-rata tertinggi jendela STABIL (plateau), langkah 6d
static bool      steadyFound = false;      // sudah ada plateau selama fase 2
static bool      gripSteadyNow = false;    // plateau pada putaran ini (layar "Lepas ok", langkah 6d)
static bool      summaryChecked = false;   // hasil kiriman ringkasan sudah dinilai (langkah 9)
static bool      restartPending = false;   // true = masuk READY berikutnya adalah restart (kedip)
static FaultType pendingFault = FAULT_NONE; // galat yang dicatat saat masuk FAULT (diisi sebelum transisi)
static bool      countdownShown = false;   // layar hitung mundur reset total sedang tampil
static uint32_t  pageStartMs = 0;          // millis() awal layar berganti halaman (SAVE_FAILED, SUMMARY tersimpan)

// ---------------- Rekaman upaya genggam (langkah 10c, Bagian 9.2) ----------------
// Satu UPAYA = dari masuk GRIP (atau sesudah dilepas terlalu awal) sampai GRIP berakhir dengan cara
// apa pun. Satu percobaan bisa punya beberapa upaya (tiap dilepas sebelum 3 s = upaya baru).
// Selama upaya terbuka, tiap sampel baru dikirim sebagai baris G; saat upaya berakhir dikirim
// SATU penanda M (gripAttemptEnd). Semua waktu dalam ms sejak upaya mulai; -1 = tidak ada.
static uint16_t  attemptNo = 0;            // nomor upaya sejak boot (1, 2, 3, ...)
static bool      attemptOpen = false;      // ada upaya yang belum dikirim penandanya
static uint32_t  attemptStartMs = 0;       // millis() saat upaya mulai
static uint32_t  attemptLoggedMs = 0;      // waktu sampel terakhir yang sudah dikirim sebagai baris G
static uint16_t  attemptSamples = 0;       // jumlah baris G upaya ini (kolom n penanda M)
static bool      attemptDemo = false;      // ada sampel upaya ini dengan skala demo x
static int32_t   onsetMs = -1;             // sampel onset (waktu nol grafik)
static float     peakKg = -1.0f;           // sampel tertinggi di fase 2
// Waktu 5 sampel terakhir di fase 2, sejajar dengan buffer plateau di sensor.cpp, supaya awal dan
// akhir jendela yang menjadi ref bisa dicatat. Setelah diisi, winTimes[winNext] = sampel tertua.
// Saat onset buffer plateau di-reset (sensorBufReset) dan winNext = 0, jadi sampel onset tidak ada
// di keduanya dan keduanya mulai dari sampel berikutnya.
static int32_t   winTimes[BUF_SIZE];
static uint8_t   winNext = 0;
static int32_t   bestWinStartMs = -1, bestWinEndMs = -1;       // jendela terkuat
static int32_t   steadyWinStartMs = -1, steadyWinEndMs = -1;   // jendela stabil terkuat

// Waktu (millis) diubah menjadi ms sejak upaya mulai.
static int32_t attemptMs(uint32_t t) { return (int32_t)(t - attemptStartMs); }

// Upaya baru: nomor naik, semua catatan upaya dikosongkan. Sampel yang sudah ada sebelum upaya
// mulai tidak dikirim (attemptLoggedMs = sampel terakhir sekarang).
static void gripAttemptStart() {
  attemptNo++;
  attemptOpen = true;
  attemptStartMs = millis();
  attemptLoggedMs = sensorLastSampleMs();
  attemptSamples = 0;
  attemptDemo = false;
  onsetMs = -1;
  peakKg = -1.0f;
  bestWinStartMs = bestWinEndMs = -1;
  steadyWinStartMs = steadyWinEndMs = -1;
}

// Kirim sampel baru (bila ada) sebagai baris G, dan catat puncak di fase 2. Dipanggil di fsmRun()
// SEBELUM cek galat, supaya sampel yang memicu galat ikut terekam: kg > 80 (OUT_OF_RANGE) dan
// sampel saturasi (ADC_SATURATED). HX711_TIMEOUT tidak punya sampel (justru tidak ada sampel
// baru), jadi pada galat itu tidak ada baris G tambahan. Hanya pencatatan yang dipindah: galat
// tetap terpicu pada putaran yang sama seperti sebelumnya.
// Puncak hanya untuk rekaman (penanda M). Puncak upaya yang diputus galat TIDAK pernah menjadi
// hasil percobaan dan tidak ikut Mx/Av: hasil hanya dari handleCompute() (ref), dan upaya galat
// tidak melewati HITUNG (penanda: ref -1, kolom galat terisi).
// Fase yang tercatat = fase sebelum sampel ini dinilai handleGrip() (sampel onset = fase 1).
static void gripLogNewSample() {
  if (sensorLastSampleMs() == attemptLoggedMs) return;   // belum ada sampel baru
  attemptLoggedMs = sensorLastSampleMs();
  uint8_t phase = !sensorTareDone() ? 0 : (gripPhase == WAIT_ONSET ? 1 : 2);
  if (phase == 2 && sensorKg() > peakKg) peakKg = sensorKg();
  if (sensorDemoActive()) attemptDemo = true;
  logGripSample(attemptNo, attemptMs(attemptLoggedMs), sensorKg(), phase, sensorDemoActive());
  if (LOG_RAW) attemptSamples++;            // LOG_RAW = 0: tidak ada baris G, n tetap 0
}

// Upaya berakhir: kirim satu penanda M. hasRef = upaya menghasilkan ref (hanya dari HITUNG).
// Jendela yang dicatat = jendela yang dipakai handleCompute() untuk ref (stabil, atau terkuat).
// Aman dipanggil dua kali: tanpa upaya terbuka tidak terjadi apa-apa.
static void gripAttemptEnd(GripExit exitBy, FaultType fault, bool hasRef, float ref) {
  if (!attemptOpen) return;
  attemptOpen = false;
  GripMarker m;
  m.session = session.number;
  m.trial = session.trialNo;
  m.attempt = attemptNo;
  m.exitBy = (uint8_t)exitBy;
  m.onsetMs = onsetMs;
  m.endMs = attemptMs(millis());
  m.winStartMs = m.winEndMs = -1;
  if (hasRef && steadyFound) {
    m.winStartMs = steadyWinStartMs;
    m.winEndMs = steadyWinEndMs;
  } else if (hasRef && bestFound) {
    m.winStartMs = bestWinStartMs;
    m.winEndMs = bestWinEndMs;
  }
  m.ref = hasRef ? ref : -1.0f;
  m.peak = peakKg;
  m.demo = attemptDemo;
  m.fault = (uint8_t)fault;
  m.samples = attemptSamples;
  logGripMarker(m);
  Serial.printf("[GRIP] upaya %u selesai: keluar=%u galat=%u, %u baris G\n",
                attemptNo, m.exitBy, m.fault, attemptSamples);
}

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
  // Skala demo (x) berakhir di sesi baru (SUMMARY->TAP, SAVE_FAILED->TAHAN, reset total).
  if (sensorDemoActive()) {
    sensorDemoSet(false);
    Serial.println("[FSM] skala demo x dimatikan (sesi baru)");
  }
}

// ---------------- Uji waktu LED (langkah 11, spesifikasi S5) ----------------
// LED "genggaman stabil" dinyalakan/dimatikan lewat fungsi ini, yang juga mencetak latensi
// FIRMWARE: dari dimulainya baca sampel pemicu (sampel yang membuat genggaman stabil, atau
// sampel lepas kg < 1) sampai pin LED ditulis. Rantai lengkap untuk brief dihitung terpisah:
// konversi HX711 (~86 ms) + jendela 5 sampel (~340-430 ms, disengaja) + latensi ini.
// Hanya perubahan yang dicetak; ledSteadyOn dinolkan setiap LED dipaksa mati di luar fungsi ini
// (masuk GRIP, galat), supaya tidak ada "LED mati" palsu.
static bool ledSteadyOn = false;
static void ledSteady(bool on) {
  ledSet(on);
  if (on == ledSteadyOn) return;
  ledSteadyOn = on;
  uint32_t us = micros() - sensorLastSampleUs();   // dihitung sebelum mencetak
  if (on) {
    // winTimes[winNext] = sampel tertua jendela stabil yang baru saja terpenuhi.
    Serial.printf("[TIMING] LED nyala %lu us setelah sampel pemicu (jendela 5 sampel %ld ms)\n",
                  (unsigned long)us, (long)(attemptMs(sensorLastSampleMs()) - winTimes[winNext]));
  } else {
    Serial.printf("[TIMING] LED mati %lu us setelah sampel pemicu (kg=%.2f)\n",
                  (unsigned long)us, sensorKg());
  }
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
  ledSteadyOn = false;
  gripAttemptStart();        // langkah 10c: upaya pertama percobaan ini
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
  bool anyDemo = false;
  for (uint8_t i = 0; i < TRIALS_PER_SESSION; i++) anyDemo = anyDemo || session.trials[i].demo;
  Serial.printf("[SUMMARY] puncak=%.2f rerata=%.2f kg%s\n", session.peak, session.avg,
                anyDemo ? " (berisi percobaan DEMO x40)" : "");

  // Langkah 9 (Bagian 9.5): 3 baris T lalu 1 baris S, masing-masing menunggu ACK sendiri.
  // Percobaan dengan skala demo (x) ditandai: T nilai2 = 1, S percobaan = jumlah percobaan demo.
  uint8_t demoCount = 0;
  logBatchStart();
  for (uint8_t i = 0; i < TRIALS_PER_SESSION; i++) {
    const Trial& t = session.trials[i];
    if (t.demo) demoCount++;
    logTrial(session.number, i + 1, t.ref, t.demo);
  }
  logSummary(session.number, demoCount, session.peak, session.avg);
  summaryChecked = false;
}

static void enterFault() {
  // Keadaan aman (LED mati, pesan, baris E) dikerjakan modul galat (fault.cpp).
  faultEnter(pendingFault, session.number, session.trialNo, session.t0Ms);
}

static void enterSaveFailed() {
  buttonReset();
  ledSet(false);
  pageStartMs = millis();
  // Nilai juga dicetak sebagai teks biasa supaya bisa disalin dari konsol logger.py (Bagian 4.8).
  Serial.printf("[GAGAL SIMPAN] sesi %u: P1=%.2f P2=%.2f P3=%.2f puncak=%.2f rerata=%.2f kg\n",
                session.number, session.trials[0].ref, session.trials[1].ref, session.trials[2].ref,
                session.peak, session.avg);
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
  // Memakai kg dengan skala NORMAL (sensorKgReal): setelah perintah x kg terskala 40x lebih besar,
  // sehingga penjaga ini akan terpicu oleh goyangan kecil dan tare ulangnya membatalkan x.
  float kgReal = sensorKgReal();
  if (kgReal < -THRESHOLD_ONSET_KG) {
    readyRetare("kg di bawah -1, nol diambil saat dibebani");
    return READY;
  }
  // Masih digenggam (kg > +1): START diabaikan, layar "Lepas genggaman".
  if (press == PRESS_TAP && kgReal <= THRESHOLD_ONSET_KG) return GRIP;
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
    // Skala normal (sensorKgReal), sama alasannya dengan penjaga di READY. Onset tetap memakai kg.
    if (sensorKgReal() < -THRESHOLD_ONSET_KG) {
      Serial.printf("[GRIP] kg=%.2f di bawah -1, nol salah: kembali ke READY\n", sensorKgReal());
      gripAttemptEnd(EXIT_ZERO_GUARD, FAULT_NONE, false, 0.0f);
      return READY;
    }
    if (kg > THRESHOLD_ONSET_KG) {
      gripPhase = MEASURING;
      gripStartMs = millis();
      sensorBufReset();                     // buffer plateau mulai kosong di awal fase 2
      bestFound = false;
      steadyFound = false;
      gripSteadyNow = false;
      gripLastSampleMs = sensorLastSampleMs();
      onsetMs = attemptMs(sensorLastSampleMs());   // langkah 10c: waktu nol grafik
      winNext = 0;                          // waktu jendela mulai kosong, sejajar buffer plateau
      displayRequest();
      Serial.printf("[GRIP] onset kg=%.2f\n", kg);
    }
    return GRIP;
  }

  // Fase 2. Waktu sejak onset.
  uint32_t t = millis() - gripStartMs;

  // Langkah 6d: pengguna yang memutuskan kapan lepas. Plateau tidak lagi mengakhiri percobaan;
  // LED (plateau) memberi tahu bahwa genggaman sudah stabil dan boleh dilepas kapan saja setelah 3 s.

  // Jalur 1: dilepas sebelum 3 s (kg turun di bawah onset) -> kembali ke fase 1, percobaan
  // tidak dihitung.
  if (kg < THRESHOLD_ONSET_KG && t < GRIP_MIN_MS) {
    gripPhase = WAIT_ONSET;
    sensorBufReset();
    ledSteady(false);
    displayRequest();
    Serial.printf("[GRIP] exit=EARLY_RELEASE t=%lu ms, kembali menunggu\n", (unsigned long)t);
    gripAttemptEnd(EXIT_EARLY_RELEASE, FAULT_NONE, false, 0.0f);   // langkah 10c: upaya ini
    gripAttemptStart();                                            // selesai, upaya baru mulai
    return GRIP;
  }

  // Plateau GENGGAMAN: buffer stabil DAN semua isinya di atas onset. Tanpa syarat kedua, tangan
  // yang sudah lepas (kg ~0, sangat stabil) terbaca sebagai plateau dan ref menjadi ~0 kg.
  bool gripWindow = sensorBufFull() && sensorBufMin() > THRESHOLD_ONSET_KG;
  bool plateau = gripWindow && sensorPlateau();

  // Catat, sekali per sampel baru (bukan per putaran loop), dua kandidat ref:
  // - jendela STABIL terkuat (plateau dengan rata-rata tertinggi) -> ref utama;
  // - jendela terkuat apa pun (di atas onset) -> ref bila genggaman tidak pernah stabil.
  // Kekuatan genggaman adalah nilai maksimum, jadi dipilih yang tertinggi. Jendela saat tangan
  // melepas tidak stabil dan lebih rendah, jadi tidak terpilih.
  if (sensorLastSampleMs() != gripLastSampleMs) {
    gripLastSampleMs = sensorLastSampleMs();
    // Langkah 10c: waktu sampel ini masuk cincin waktu jendela (sejajar buffer plateau).
    // Puncak upaya dicatat di gripLogNewSample().
    int32_t sampleMs = attemptMs(gripLastSampleMs);
    winTimes[winNext] = sampleMs;
    winNext = (winNext + 1) % BUF_SIZE;     // sekarang menunjuk sampel tertua jendela
    if (gripWindow) {
      float avg = sensorBufAvg();
      if (!bestFound || avg > bestAvg) {
        bestFound = true;
        bestAvg = avg;
        bestWinStartMs = winTimes[winNext];
        bestWinEndMs = sampleMs;
      }
      if (plateau && (!steadyFound || avg > bestSteadyAvg)) {
        steadyFound = true;
        bestSteadyAvg = avg;
        steadyWinStartMs = winTimes[winNext];
        steadyWinEndMs = sampleMs;
      }
    }
  }

  // LED = plateau (R4: LED dikendalikan kestabilan gaya yang terukur).
  ledSteady(plateau);
  gripSteadyNow = plateau;                  // dipakai drawScreen() untuk petunjuk "Lepas ok"

  // Jalur 2: dilepas antara 3 dan 5 s -> percobaan selesai (pilihan pengguna).
  if (kg < THRESHOLD_ONSET_KG) {
    gripExit = EXIT_RELEASE;
    gripDurMs = t;
    ledSteady(false);
    Serial.printf("[GRIP] exit=RELEASE t=%lu ms%s\n", (unsigned long)t,
                  steadyFound ? "" : " (tidak pernah stabil)");
    return COMPUTE;
  }

  // Jalur 3: masih digenggam pada 5 s -> batas paksa. Bila genggaman sedang stabil, LED tetap
  // menyala sampai dilepas di REST (Bagian 10.3); bila tidak, LED mati.
  if (t >= GRIP_MAX_MS) {
    gripExit = EXIT_TIMEOUT;
    gripDurMs = t;
    if (!plateau) ledSet(false);
    Serial.printf("[GRIP] exit=TIMEOUT t=%lu ms%s\n", (unsigned long)t,
                  steadyFound ? "" : " (tidak pernah stabil)");
    return COMPUTE;
  }
  return GRIP;
}

static State handleCompute() {
  // ref (langkah 6d, Bagian 5.11), presisi penuh (pembulatan hanya di LCD):
  // 1) jendela stabil terkuat bila ada plateau selama fase 2;
  // 2) jendela terkuat (di atas onset) bila tidak pernah stabil;
  // 3) cadangan: rata-rata buffer sekarang (praktis tidak terjadi setelah >= 3 s di atas onset).
  Trial& tr = session.trials[session.trialNo - 1];
  tr.steady = steadyFound;
  if (steadyFound)    tr.ref = bestSteadyAvg;
  else if (bestFound) tr.ref = bestAvg;
  else                tr.ref = sensorBufAvg();
  tr.durMs = gripDurMs;
  tr.exitBy = gripExit;
  tr.demo = sensorDemoActive();             // percobaan dengan skala demo bukan data nyata
  Serial.printf("[COMPUTE] percobaan %u ref=%.2f kg (%s, %s)%s\n", session.trialNo, tr.ref,
                gripExit == EXIT_TIMEOUT ? "5 s" : "dilepas",
                tr.steady ? "stabil terkuat" : "terkuat", tr.demo ? " DEMO x40" : "");

  // Validasi rentang (langkah 8, cek kedua; cek pertama berjalan live di GRIP):
  // ref harus > MIN_FORCE_KG (0) dan <= MAX_FORCE_KG (80, kapasitas load cell).
  tr.valid = (tr.ref > MIN_FORCE_KG && tr.ref <= MAX_FORCE_KG);
  // Langkah 10c: penanda M upaya ini. ref di luar rentang -> galat 3 (OUT_OF_RANGE) di penanda,
  // supaya grafik tidak memperlakukannya sebagai hasil yang dihitung.
  gripAttemptEnd(gripExit, tr.valid ? FAULT_NONE : OUT_OF_RANGE, true, tr.ref);
  if (!tr.valid) {
    pendingFault = OUT_OF_RANGE;            // percobaan tidak dihitung; TAHAN mengulangnya
    return FAULT;
  }
  return REST;
}

static State handleRest(Press press) {
  if (restPhase == RELEASE_REST) {
    // Sub-fase 1a: tunggu genggaman dilepas. Saat itu LED mati dan hitung mundur mulai.
    if (!restCounting) {
      if (sensorKg() < THRESHOLD_ONSET_KG && session.trialNo >= TRIALS_PER_SESSION) {
        // Setelah percobaan 3 tidak ada istirahat: istirahat ada untuk pemulihan otot SEBELUM
        // genggaman berikutnya, dan sesudah percobaan 3 tidak ada genggaman lagi. Dilepas ->
        // langsung "NEXT > ringkasan"; hasil percobaan 3 tetap tampil di baris 1.
        ledSteady(false);
        restPhase = WAIT_NEXT;
        buttonReset();                      // sub-fase 2 menunggu tombol: tekanan lama dibuang
        displayRequest();
        Serial.println("[REST] percobaan terakhir dilepas: tanpa istirahat, tunggu NEXT");
        return REST;
      }
      if (sensorKg() < THRESHOLD_ONSET_KG) {
        ledSteady(false);
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
  // Langkah 9: tunggu semua baris ringkasan selesai (ACK, atau gagal setelah kirim ulang).
  // Selama itu TAP diabaikan; layar "Menyimpan...".
  if (logBusy()) return SUMMARY;
  if (!summaryChecked) {
    summaryChecked = true;
    if (logBatchFailed()) return SAVE_FAILED;   // ada baris yang tidak ber-ACK: data belum aman
    Serial.println("[SUMMARY] semua baris tersimpan (ACK)");
    pageStartMs = millis();                 // layar mulai dari "Data tersimpan"
    displayRequest();
  }
  if (press != PRESS_TAP) return SUMMARY;
  fsmNewSession();
  restartPending = true;                    // RESTART: READY dengan kedip
  return READY;
}

static State handleFault(Press press) {
  if (press != PRESS_HOLD) return FAULT;
  // TAHAN 1,5 s: kembali ke READY dengan kedip. Sesi dan nomor percobaan tetap,
  // percobaan yang gagal tidak dihitung (diulang).
  faultClear();
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
// Nilai kg untuk LCD (langkah 11). Slot %5.2f hanya muat -9.99 s.d. 99.99 (5 karakter):
// - di luar rentang itu angkanya dibatasi, supaya teks di kanannya tidak tergeser atau terpotong
//   (mis. satu gambar 112 kg dengan skala demo x sebelum OUT_OF_RANGE, atau 156 kg saat saturasi);
// - |kg| < 0,005 ditulis 0, karena printf menulis -0.004 sebagai "-0.00" dan angka di layar
//   berkedip antara " 0.00" dan "-0.00" saat alat diam.
// Hanya untuk tampilan: nilai yang dihitung, dicatat, dan dikirim tetap nilai aslinya.
static float lcdKg(float kg) {
  if (kg > 99.99f) return 99.99f;
  if (kg < -9.99f) return -9.99f;
  if (kg > -0.005f && kg < 0.005f) return 0.0f;
  return kg;
}

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

  // Teks mengikuti Tabel 4 proposal (sama dengan firmware tim lainnya) dengan perubahan langkah 10:
  // semua kg dua desimal, RINGKASAN tanpa "kg" (supaya muat), GAGALSIMPAN tiga halaman, dan
  // "Data tersimpan" setelah semua ACK. Daftar lengkap: alur-program-hgd.md Bagian 10.2 dan
  // teks-lcd-hgd.md. Semua baris <= 16 karakter.
  // Angka ditulis dengan lebar tetap (%5.2f = 5 karakter untuk -9.99 s.d. 99.99) supaya posisi
  // tidak bergeser saat nilai berubah; semua kg lewat lcdKg() supaya tetap di rentang itu.
  const Trial& tr = session.trials[session.trialNo - 1];
  switch (state) {
    case READY:   // "SIAP - UKUR 1/3" / "Menyesuaikan nol", "Lepas genggaman" atau "Tekan tmbl START"
      // Skala demo aktif: "SIAP x40 1/3", supaya tidak ada yang mengukur sungguhan tanpa sadar.
      if (sensorDemoActive())
        snprintf(l0, sizeof(l0), "SIAP x%.0f %u/%u", DEMO_SCALE_DIV, session.trialNo, TRIALS_PER_SESSION);
      else
        snprintf(l0, sizeof(l0), "SIAP - UKUR %u/%u", session.trialNo, TRIALS_PER_SESSION);
      // "Nol ulang: lepas" = tare diulang karena offset di luar jendela (8d, teks langkah 10);
      // "Lepas genggaman" di bawah = alat masih ditekan setelah tare (5c).
      if (sensorTareWindowRetry())               snprintf(l1, sizeof(l1), "Nol ulang: lepas");
      else if (!sensorTareDone())                snprintf(l1, sizeof(l1), "Menyesuaikan nol");
      else if (sensorKgReal() > THRESHOLD_ONSET_KG) snprintf(l1, sizeof(l1), "Lepas genggaman");
      else                                       snprintf(l1, sizeof(l1), "Tekan tmbl START");
      break;
    case GRIP:
      if (gripPhase == WAIT_ONSET) {   // "Menunggu genggam" / " 0.42kg >1.00kg" (kg live + target onset)
        // Dua desimal dan target supaya penggenggam melihat tekanannya bergerak dan seberapa jauh
        // lagi sampai onset (dengan satu desimal tekanan kecil tetap tampil "0.0").
        snprintf(l0, sizeof(l0), "Menunggu genggam");
        if (sensorTareWindowRetry()) snprintf(l1, sizeof(l1), "Nol ulang: lepas");     // 8d
        else if (!sensorTareDone()) snprintf(l1, sizeof(l1), "Menyesuaikan nol");   // tare ulang (TAHAN)
        // Target juga dua desimal dengan satuan, gaya sama dengan "25.34kg Lepas ok" (15 karakter).
        else snprintf(l1, sizeof(l1), "%5.2fkg >%.2fkg", lcdKg(sensorKg()), THRESHOLD_ONSET_KG);
      } else {   // "Tahan 3-5 detik" / "25.34 kg  3.4s" (kg live + detik sejak onset)
        snprintf(l0, sizeof(l0), "Tahan 3-5 detik");
        uint32_t t = millis() - gripStartMs;
        if (t >= GRIP_MIN_MS && gripSteadyNow)
          // Langkah 6d: sudah >= 3 s dan genggaman stabil (LED menyala) -> boleh dilepas kapan saja.
          // "25.34kg Lepas ok" = 16 karakter (tanpa spasi sebelum "kg" supaya muat).
          snprintf(l1, sizeof(l1), "%5.2fkg Lepas ok", lcdKg(sensorKg()));
        else
          snprintf(l1, sizeof(l1), "%5.2f kg  %.1fs", lcdKg(sensorKg()), t / 1000.0f);
      }
      break;
    case COMPUTE:
      return;   // one-shot tanpa layar
    case REST:    // "Hasil 47.23 kg" / "Lepas>Rest 60s" lalu "NEXT > lanjut"
      snprintf(l0, sizeof(l0), "Hasil %5.2f kg", lcdKg(tr.ref));
      if (restPhase == RELEASE_REST && session.trialNo >= TRIALS_PER_SESSION) {
        // Percobaan 3: tanpa istirahat, jadi tanpa hitung mundur. Gaya sama dengan "Lepas>Rest";
        // "Lepas genggaman" tidak dipakai karena di SIAP artinya lain (alat masih ditekan).
        snprintf(l1, sizeof(l1), "Lepas>ringkasan");
      } else if (restPhase == RELEASE_REST) {
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
    case SUMMARY: // "Mx47.23 Av44.81" / "Menyimpan...", lalu "Data tersimpan" <-> "Tekan: RESTART"
      // Dua desimal: "kg" tidak muat lagi (18 karakter), jadi dibuang; Mx = puncak, Av = rerata.
      snprintf(l0, sizeof(l0), "Mx%5.2f Av%5.2f", lcdKg(session.peak), lcdKg(session.avg));
      if (logBusy() || !summaryChecked) {
        snprintf(l1, sizeof(l1), "Menyimpan...");
      } else {
        // "Menyimpan..." dengan logger.py hanya tampil < 0,1 s (tidak sempat terbaca), jadi bukti
        // data sudah aman adalah "Data tersimpan". Baris 2 bergantian tiap SUMMARY_PAGE_MS (2 s)
        // dengan petunjuk "Tekan: RESTART", supaya tetap terlihat walau LCD baru dilihat kemudian.
        // Tidak ada penundaan buatan: TAP tetap langsung memulai sesi baru.
        bool savedPage = ((millis() - pageStartMs) / SUMMARY_PAGE_MS) % 2 == 0;
        snprintf(l1, sizeof(l1), "%s", savedPage ? "Data tersimpan" : "Tekan: RESTART");
      }
      break;
    case FAULT:   // "ERR:..." / "RAW <raw live>" (fault.cpp)
      faultDraw(l0, l1, sizeof(l0));
      break;
    case SAVE_FAILED:
      // Tiga halaman bergantian tiap SAVE_FAILED_PAGE_MS (3 s) supaya semua nilai bisa disalin.
      // Dua desimal tidak muat bila P1 dan P2 satu baris (17 karakter), jadi satu nilai per baris.
      // Halaman 3 memakai label "Mx" yang sama dengan layar SUMMARY (puncak = satu nama).
      switch (((millis() - pageStartMs) / SAVE_FAILED_PAGE_MS) % 3) {
        case 0:
          snprintf(l0, sizeof(l0), "P1 %5.2f kg", lcdKg(session.trials[0].ref));
          snprintf(l1, sizeof(l1), "P2 %5.2f kg", lcdKg(session.trials[1].ref));
          break;
        case 1:
          snprintf(l0, sizeof(l0), "P3 %5.2f kg", lcdKg(session.trials[2].ref));
          snprintf(l1, sizeof(l1), "GAGAL SIMPAN");
          break;
        default:
          snprintf(l0, sizeof(l0), "Mx%5.2f Av%5.2f", lcdKg(session.peak), lcdKg(session.avg));
          snprintf(l1, sizeof(l1), "GAGAL SIMPAN");
          break;
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
    // Kiriman yang masih menunggu ACK dibatalkan lebih dulu (Bagian 9.5 butir 5). Baris E
    // FORCE_RESET memakai sesi dan percobaan SEBELUM reset (Bagian 9.2); bila ACK-nya gagal,
    // reset tetap berjalan.
    logCancel();
    // Langkah 10c: upaya genggam yang sedang berjalan ditutup dengan penanda (setelah logCancel(),
    // supaya penanda ini tidak ikut dibatalkan).
    gripAttemptEnd(EXIT_FORCE_RESET, FAULT_NONE, false, 0.0f);
    faultLogEvent(EVENT_FORCE_RESET, session.number, session.trialNo, session.t0Ms);
    faultClear();                           // reset dari FAULT juga menghapus galat aktif
    fsmNewSession();                        // sesi berjalan dibuang, nomor sesi naik
    restartPending = true;
    changeState(READY);                     // masuk ulang READY walau sudah di READY
    return;
  } else if (!forceDone && held >= FORCE_RESET_SHOW_MS && !countdownShown) {
    countdownShown = true;                  // mulai detik ke-3: tampilkan hitung mundur
    displayRequest();
  }

  // (2) Transisi pengecualian: galat sensor di dalam siklus percobaan -> FAULT.
  //     Langkah 8: di GRIP juga cek rentang LIVE, tanpa menunggu percobaan selesai
  //     (kg > 80 kg pada sampel mana pun = melewati kapasitas load cell).
  if (inTrialCycle(state)) {
    // Langkah 10c: sampel baru di GRIP dicatat (baris G, puncak) sebelum dinilai di sini, supaya
    // sampel yang memicu galat juga ada di rekaman. Tidak mengubah kapan galat terpicu.
    if (state == GRIP) gripLogNewSample();
    FaultType f = sensorCheck();
    if (f == FAULT_NONE && state == GRIP && sensorKg() > MAX_FORCE_KG) f = OUT_OF_RANGE;
    if (f != FAULT_NONE) {
      pendingFault = f;
      // Langkah 10c: galat saat GRIP (atau HITUNG) memutus upaya: penanda dengan keluar = 4 dan
      // kode galat, supaya grafik tidak mencampurnya dengan data nyata.
      gripAttemptEnd(EXIT_FAULT, f, false, 0.0f);
      ledSteadyOn = false;                  // LED dipaksa mati di faultEnter()
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

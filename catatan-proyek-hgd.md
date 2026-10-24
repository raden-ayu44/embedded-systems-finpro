---
title: Catatan Proyek - Hand Grip Dynamometer

---

# Catatan Proyek - Hand Grip Dynamometer (v52)
**Proyek akhir - embedded system**

> Catatan kerja proyek akhir mata kuliah embedded system, sekaligus **dokumen induk**: ringkasan, kebutuhan, rumusan masalah, spesifikasi, BOM, risiko, jadwal, anggaran, keselamatan, riwayat keputusan, dan log audit ada di sini; bahan teknis dipecah ke dokumen di peta di bawah. Ditulis supaya bisa dibaca sendiri (offline). Dipakai untuk G2-G4, makalah akhir, dan demo.

## Peta dokumen

| Dokumen | Isi | Status |
|---|---|---|
| [firmware/alur-program-hgd.md](firmware/alur-program-hgd.md) | Seluruh firmware dan perangkat lunak laptop: struktur berkas, struktur data, konstanta, fungsi, state machine, HX711 dan sinyal, kalibrasi (pengolahan data), logging, antarmuka, uji modul, build, constraint, hal terbuka (Bagian 18) | v5; **firmware utama langkah 0-10c sudah dikode dan diuji di papan** (sampai 24 Okt. 2026, `firmware/hgd/`), termasuk `laptop/logger.py` (tiga CSV harian: hasil, galat, rekaman genggaman); tanpa istirahat setelah percobaan 3 (24 Okt. 2026); berikutnya langkah 11; sketsa uji U4, U7, U8 dan sketsa bench `uji_bench_hgd` ada di `firmware/sketches/`; hasil uji bench 15 Okt. 2026 tercatat |
| [firmware/teks-lcd-hgd.md](firmware/teks-lcd-hgd.md) | Semua layar LCD persis seperti dicetak firmware (baris 1/2, jumlah karakter, format di kode), untuk ditinjau | v1 (24 Okt. 2026); rujukan utama tetap Bagian 10.2 di alur program |
| [pcb/desain-pcb-hgd.md](pcb/desain-pcb-hgd.md) | Skema dan PCB: komponen, alokasi pin, rantai sinyal, load cell, catu, keputusan catu LCD, wiring, netlist KiCad `hgd-v1`, usulan penempatan, pemeriksaan komponen, sisa pekerjaan; gambar di `pcb/gambar/` | v4: **skema dan PCB v1 selesai, belum disetujui** (ERC dan DRC 0 error, Gerber dicek di viewer JLCPCB); berkas KiCad dan STEP di `pcb/kicad/proyek/`; log kerja di `pcb/log-kerja-kicad-hgd.md` |
| [pcb/pcb_netlist.xlsx](pcb/pcb_netlist.xlsx) | Netlist; komponen dengan nama umum, nama part resmi, simbol dan footprint KiCad; modul di luar PCB; asumsi dan sisa | Sesuai skema `hgd-v1` (15 Okt. 2026) |
| [pcb/kicad/README.md](pcb/kicad/README.md) | Hub berkas KiCad: simbol dan footprint (DevKit dari SnapEDA), struktur folder untuk proyek, pustaka, PDF, dan Gerber | v5; skema, papan, salinan 3D verbatim, dan STEP ada di `proyek/`; Gerber v1 di luar repo sampai disetujui |
| [housing-load-cell/desain-housing-load-cell.md](housing-load-cell/desain-housing-load-cell.md) | Housing grip load cell: layout dua batang, kekakuan batang, kalibrasi, pengadaan pelat, rujukan | Ada (v10) |
| [housing-load-cell/asumsi.md](housing-load-cell/asumsi.md) | Asumsi dan keputusan di balik gambar housing; gambar SVG di `gambar/`, parameter Fusion di `fusion/` | Gabungan |
| [housing-elektronik/desain-housing-elektronik.md](housing-elektronik/desain-housing-elektronik.md) | Housing PCB, ESP32, LCD, HX711, level shifter, tombol, LED; ukuran LCD, parameter Fusion dan STEP di `fusion/`, kompensasi susut, aturan cetak 3D | v5: **desain v1 selesai, belum disetujui**; belum dicetak (menunggu PCB) |
| [firmware/protokol-uji-bench-hgd.md](firmware/protokol-uji-bench-hgd.md) | Langkah uji bench dengan sketsa `uji_bench_hgd` (U4, U7, U8, pemanasan, E1, arah tekan, tare ulang), cara membaca log, dan tabel hasil | v2 (wiring level shifter); E1 resmi 16 Okt. 2026 |
| [kalibrasi/protokol-kalibrasi-hgd.md](kalibrasi/protokol-kalibrasi-hgd.md) | Protokol kalibrasi dumbbell (tahap 1 sampai 20 kg, tahap 2 sampai 42 kg) | Belum diuji |
| [kalibrasi/hgd_perhitungan.xlsx](kalibrasi/hgd_perhitungan.xlsx) | Perhitungan dengan rumus: tab Mekanik (kekakuan, tegangan, celah, spacer, baut, massa) dan tab Elektrik (sinyal, ADC, noise, galat, regresi kalibrasi) | Berjalan |
| [kalibrasi/dimensi_dumbbell.xlsx](kalibrasi/dimensi_dumbbell.xlsx) | Dimensi dumbbell heksagonal untuk rig kalibrasi | Berjalan |
| [pengukuran/tabel-pengukuran-hgd.xlsx](pengukuran/tabel-pengukuran-hgd.xlsx) | Tabel pengukuran jangka sorong, satu tab per komponen, dengan angka resmi bila ada | Sebagian terisi; LCD dan HX711 per 15 Okt. 2026 |
| [pengukuran/e1-noise-hgd.xlsx](pengukuran/e1-noise-hgd.xlsx) | Semua baris uji noise E1 (15 dan 16 Okt. 2026), pemanasan, uji tekan, log mentah; ringkasan dengan rumus dan grafik; lembar kosong untuk E1 rakitan akhir | Berjalan |
| [datasheet/datasheet-library.md](datasheet/datasheet-library.md) | Catatan datasheet per komponen (DS-LC1, DS-HX1, DS-ESP1, DS-ESP2, DS-LCD1, DS-I2C1, DS-LLS1, DS-INS1, dan lainnya) beserta daftar cek kedatangan; PDF di `datasheet/pdf/` | Berjalan; memuat hasil ukur 10 dan 15 Okt. 2026 |
| [datasheet/daftar-isian-datasheet.md](datasheet/daftar-isian-datasheet.md) | Nilai datasheet yang belum terisi | Berjalan |
| [sitasi/sitasi-hgd.md](sitasi/sitasi-hgd.md) | Teori buku, temuan paper, kode sitasi lokal, daftar pustaka gaya IEEE; PDF di `sitasi/pdf/` | Berjalan (v1) |

Kode sitasi lokal (M1, W1, V1, dst.) dijelaskan di [sitasi-hgd.md](sitasi/sitasi-hgd.md).

## Cara membaca

| Tanda | Arti |
|-------|------|
| ✔ | **Terverifikasi**: sudah diukur atau diuji sendiri (tanggal dicantumkan) |
| ▣ | **Rencana**: keputusan sudah diambil dan akan dikerjakan |
| ◐ | **Sedang dikerjakan** (WIP) |
| `[sitasi]` | Klaim yang bersandar pada literatur; sumbernya disusun di Bagian 2 di [sitasi-hgd.md](sitasi/sitasi-hgd.md) |
| *nilai awal* | Angka rencana yang dipakai sebagai titik mulai dan **disetel lewat eksperimen** yang dinamai di sebelahnya (E1, E2, ...). Hasil eksperimen menggantikannya |

---

## 1. RINGKASAN PROYEK

| Item | Detail |
|------|--------|
| Mata kuliah | Embedded system, proyek akhir berkelompok (3 orang) |
| Posisi saat ini (17 Okt. 2026) | Semua modul sudah tiba, termasuk level shifter; semua ukuran untuk PCB terukur dengan jangka sorong. Uji bench 15 Okt. 2026 di breadboard: LCD (0x27), HX711 dengan load cell, tombol dan LED hidup; noise E1 σ ≈ 0,7 g. Catu LCD diputuskan: 5 V + level shifter. PCB `hgd-v1` v1 berjalur dengan LED daya D2, pour GND di kedua lapis dan 57 via jahit, ERC dan DRC 0 error (belum disetujui). Level shifter disolder dan dicek (LV 3,26 V, HV 4,98 V), LCD bekerja lewat shifter di breadboard; pull-up I2C dihitung dari ukuran, PCB tanpa pull-up tambahan. **Skema, PCB, dan housing elektronik versi 1 selesai (17 Okt. 2026), belum disetujui**: Gerber dicek di viewer JLCPCB; housing (lid, body, bibir, 4 snap-fit) di Fusion, STEP di repo. Berikutnya: persetujuan PCB lalu unggah Gerber (batas 27 Okt. 2026); PDF skema dan BOM; cetak uji satu lidah snap |
| Pembagian kerja | Penulis catatan ini memegang peran sensing, kalibrasi, power budget, skema dan layout PCB. Dua anggota lain: firmware dan antarmuka pengguna (state machine, LCD, LED), serta jalur logging, enclosure, BOM, dan dokumentasi |
| Platform wajib | ESP32 (Arduino IDE) |
| Anggaran komponen | Maksimum Rp300.000 per tim, dengan bukti pembelian |
| Filosofi cakupan | Menguji keterampilan teknis mahasiswa - BUKAN prototipe siap produksi massal |
| Perangkat | Hand grip dynamometer |

---

## 2. TUJUAN DAN SPESIFIKASI TARGET

### 2.1 Apa yang dibangun
Hand grip dynamometer digital: dua pelat aluminium dikepit tangan, sebuah load cell di antara pelat membaca gaya, modul HX711 mendigitalkannya, dan ESP32 menampilkan hasil di LCD 16x2, menyalakan LED selama genggaman, dan mengirim hasil tiap sesi ke laptop untuk disimpan sebagai berkas CSV secara otomatis.

### 2.2 Kenapa
Kekuatan genggam (HGS) makin dipakai sebagai indikator kesehatan, dan nilai klinisnya terutama datang dari pengukuran berulang dari waktu ke waktu, bukan satu kali baca `[sitasi]`. Dynamometer klasik umumnya menampilkan angka tanpa pencatatan otomatis sehingga nilainya disalin manual, rawan salah dan hilang `[sitasi]`. Kontribusi proyek ini: **pengukuran sesuai protokol standar + pencatatan otomatis**.

### 2.3 Standar keberhasilan (dua saja)
| # | Standar | Bukti |
|---|---------|-------|
| 1 | **Measurement**: pengukuran mengikuti protokol standar (duduk, siku 90°, tahan 3-5 detik, 3 percobaan, istirahat ≈ 1 menit) `[sitasi]` | RMSE/MAE terhadap beban acuan (Bagian 8 di [alur-program-hgd.md](firmware/alur-program-hgd.md)) |
| 2 | **Logging**: tiap sesi tersimpan otomatis ke CSV di laptop, tanpa salin manual | Uji S4 (Bagian 9) |

### 2.4 Spesifikasi target
| ID | Parameter | Target |
|----|-----------|--------|
| S1 | Rentang gaya genggam | 20-70 kgf |
| S2 | Update hitung mundur istirahat | tiap 100 ms, variasi ±10-20 ms |
| S3 | Daya | rata-rata ≤ 150 mA, puncak ≤ 600 mA |
| S4 | Pencatatan | 3 percobaan per sesi tersimpan otomatis ke CSV |
| S5 | LED | menyala sepanjang genggaman (state `GENGGAM`) |

> Batas bawah ketidakpastian load cell: error gabungan ±0,03 %RO ≈ **±24 g** (≈ ±34 g bila digabung suhu dan drift HX711), dihitung dari datasheet. Akurasi sistem penuh ditentukan lewat RMSE empiris, bukan angka ini.

### 2.5 Yang tidak dilakukan alat
- Tidak diuji pada orang di luar anggota tim. Seluruh validasi memakai anggota tim, sesuai batas pengukuran non-invasif bertegangan rendah.
- Bukan pengganti dynamometer klinis bersertifikasi; tidak dipakai untuk diagnosis.
- Tidak memvalidasi teknik genggam (posisi jari dan ibu jari); kepatuhan pada protokol mengandalkan instruksi verbal.
- Tidak mengukur dua tangan bersamaan; satu tangan per percobaan.
- Gaya di luar 20-70 kgf tidak dijamin akurat. Nilai di luar rentang fisik (≤ 0 atau > 100 kg) ditolak sebagai kegagalan sensor.
- Bukan data logger mandiri: pencatatan butuh laptop host tersambung USB selama sesi.

### 2.6 Di luar cakupan (sengaja)
Game biofeedback, sensor jari (FSR), IMU tremor, relative HGS di firmware (dihitung di analisis pasca-pengukuran), buzzer, Raspberry Pi, dan Edge AI. Cloud logging dan dashboard tren adalah stretch goal, tidak dijanjikan.

---

## 3. SUMBER RUJUKAN

*(Tautan folder berbagi dihapus supaya catatan ini anonim; isi folder: PDF paper dan buku rujukan di bawah.)*

| Sumber | Fokus utama | Kontribusi ke proyek ini |
|--------|-------------|---------------------------|
| Ramadhani et al. 2019 (IJEEEMI) | Alat ukur genggam pasien pasca-stroke: Arduino Uno + HX711 + load cell batang + LCD 16x2 + indikator 3-tingkat | Arsitektur paling sederhana dan paling dekat dengan skala proyek kelas - jadi referensi utama arsitektur hardware |
| Gotthelf et al. 2021 (J NeuroEng Rehab) | Sistem grip berbasis game (Rocket Launch), tracking MVC + fatigue selama sesi, divalidasi terhadap data normatif literatur | Sumber ide "pengukuran serial dari waktu ke waktu", TIDAK diadopsi fitur game-nya (di luar skop kelas) |
| Becerra et al. 2021 (Applied Sciences, UIB) | Perangkat wireless multi-sensor: gaya genggam + tekanan jari (FSR) + orientasi/tremor (IMU) + Bluetooth | Referensi untuk memahami instrumentation amplifier (AD627) sebagai alternatif HX711 - TIDAK diadopsi multi-sensornya (di luar skop kelas) |
| Chang & Chen 2015 (Bio-Med Mat & Eng, Taiwan) | Dynamometer + NI DAQ + LabView, data normatif Taiwan berdasarkan usia/tinggi/berat/panjang telapak | Referensi metodologi korelasi (tinggi, berat, panjang tangan vs kekuatan genggam) - platform (NI DAQ) TIDAK relevan untuk ESP32 |
| Vaishya et al. 2024 (J Health Pop & Nutrition) | Narrative review: HGS sebagai vital sign baru, cutoff per populasi, asosiasi dengan T2D/CVD/mortalitas/sarcopenia, protokol pengukuran (Box 1) | Sumber argumen medis utama + protokol pengukuran standar (duduk, siku 90 derajat, tahan 3-5 detik, 3 kali percobaan, istirahat 1 menit antar percobaan) + konsep relative HGS (HGS/BMI) |
| Marwedel, *Embedded System Design* (ed. 4) | Buku teks embedded system - teori state machine, evaluasi, dependability | §2.4 StateCharts (formalisasi hierarki 2 lapisan, Bagian 15 di [alur-program-hgd.md](firmware/alur-program-hgd.md)), §5.3 Quality Metrics (RMSE/MAE, Bagian 7), §5.6.5 FMEA/FTA (daftar risiko, Bagian 7), Bab 1 Tabel 1.2 (justifikasi ESP32, Bagian 17 di [alur-program-hgd.md](firmware/alur-program-hgd.md)) |
| White, *Making Embedded Systems* (ed. 1, 2011) - dirujuk panduan sebagai [3] | Buku teks embedded system - praktik implementasi C untuk mikrokontroler | Bab 4 (debounce tombol, PWM), Bab 5 (table-driven state machine, watchdog), Bab 6 (circular buffer, event vs data-driven), Bab 3 (pola "Error Handling Library"), Bab 9 (taking an average) - lihat §6.4 untuk rincian per keputusan |
| Russell, *Introduction to Embedded Systems Using ANSI C and the Arduino Development Environment* (2010) - dirujuk panduan sebagai [1] | Buku teks embedded system - dasar C, arsitektur ATmega328P, GPIO/timer/interrupt/ADC | §9.1.2 ISR and Main Task Communication (kebenaran teknis komunikasi ISR-loop utama, Bagian 15.2 di [alur-program-hgd.md](firmware/alur-program-hgd.md) SIAP), §6.2.2 Internal Pull-up Resistor (justifikasi hindari floating pin, Bagian 15.2 di [alur-program-hgd.md](firmware/alur-program-hgd.md) SIAP) |
| Dokumentasi alat komersial: Kinvent K-Grip, GripAble, Vernier HD-BTA, Biopac SS25LA (dokumen pabrikan dan penjual, bukan peer-review) | Bentuk, rentang, dan ergonomi hand dynamometer komersial | Pembanding desain housing grip ([HL2](housing-load-cell/desain-housing-load-cell.md#hl2-pembanding-alat-komersial-dokumen-pabrikan-bukan-peer-review)). Bukan bukti ilmiah; jangan disitasi sebagai sumber akademik. Rincian dan URL di [HL18 dokumen housing](housing-load-cell/desain-housing-load-cell.md#hl18-sumber-rujukan-housing-bukan-peer-review) |
| Diskusi komunitas: Arduino Forum, "calibrating a straight-bar load cell" (bukan sumber akademik) | Kapasitas load cell untuk grip, rig kalibrasi, efek kekakuan rangka | Kalibrasi setelah rakit ([HL5](housing-load-cell/desain-housing-load-cell.md#hl5-dampak-housing-terhadap-pembacaan-gaya)). Rincian di [HL18 dokumen housing](housing-load-cell/desain-housing-load-cell.md#hl18-sumber-rujukan-housing-bukan-peer-review) |

---

## 4. KEBUTUHAN DARI PANDUAN MATA KULIAH

### 4.1 Gerbang proyek dan tenggat waktu

| Minggu | Tonggak |
|--------|---------|
| 5-6 | Design review: revisi sesuai catatan TA (Minggu 6); skema rangkaian sendiri (modul praktikum) |
| 7 | Layout PCB dan gerber (modul praktikum) |
| Rabu 14 Okt. 2026 | **Draf PCB v1** dibawa ke kelas. Sifatnya take-home dan non-formal: belum ada pengumpulan atau penilaian. Isi sekarang di Bagian 8 di [desain-pcb-hgd.md](pcb/desain-pcb-hgd.md) (PCB `hgd-v1`) |
| 7 ATAU 8 | Tenggat berkas gerber: **konflik antar dokumen resmi**, lihat catatan di bawah |
| 9 | **Tahap prototipe**: satu jalur penginderaan dan satu jalur aktuasi hidup di breadboard; PCB dipesan kolektif |
| 10-11 | Modul Raspberry Pi dan Edge AI; sertifikasi cetak 3D wajib selesai paling lambat Minggu 11 |
| 11-12 | PCB diterima (sekitar dua minggu setelah pesanan) |
| 12 | **Tahap akhir**: PCB dirakit dan lolos uji dasar |
| 14 | **G4**: purwarupa terintegrasi lolos uji mandiri |
| 15 | Evaluasi sejawat rahasia |
| 16 | UAS: demo expo, makalah akhir, viva individu |

> Konflik yang belum terkonfirmasi: panduan mata kuliah menyebut tenggat gerber Minggu ke-7 di dua tempat (modul praktikum, dan Bagian D.5: berkas gerber yang melewati tenggat Minggu ke-7 tidak ikut batch fabrikasi). Panduan proposal menyebut Minggu 8 ("Gerber files due - no extension", pada baris UTS, terpisah dari baris layout PCB di Minggu 7). Sampai TA mengonfirmasi, jangan menulis salah satu minggu sebagai final di dokumen resmi berikutnya.

### 4.2 Checklist internal (bukan rubrik skor resmi)

> Rubrik skor proposal yang sebenarnya hanya punya **4 kriteria** (perumusan masalah medis; spesifikasi terukur; kelayakan teknis + arsitektur + BOM digabung; jadwal + risiko digabung), masing-masing 0-100, skor = rata-rata keempatnya. Tabel di bawah adalah checklist internal yang lebih granular (6 baris); jangan disamakan dengan struktur rubrik resmi saat presentasi design review.

| Bagian wajib (checklist internal) | Status |
|---|---|
| Perumusan masalah medis | Draf kerja versi v5 ada di Bagian 5 catatan ini |
| Spesifikasi terukur (range, akurasi, waktu respons, daya) | Belum dikunci di dokumen formal. Akurasi sistem penuh baru diverifikasi lewat RMSE di G4 |
| Arsitektur sistem + kelayakan teknis | Lihat Bagian 15 di [alur-program-hgd.md](firmware/alur-program-hgd.md) dan Bagian 8 di [desain-pcb-hgd.md](pcb/desain-pcb-hgd.md) (netlist PCB `hgd-v1`) |
| BOM dalam anggaran Rp300.000 | BOM aktual di Bagian 7.1 (total pembelian sekitar Rp253.875 sebelum ongkir) |
| Jadwal selaras dengan gerbang proyek | Lihat Bagian 4.1; terkait konflik tenggat gerber |
| Daftar risiko dan mitigasi | Belum ada dokumen risiko tersendiri; kandidat risiko di Bagian 7 |

### 4.3 Persyaratan format resmi (dari Panduan dan template)

| Persyaratan | Sumber | Status |
|---|---|---|
| Body maksimal 8 halaman (Bagian 1-6; cover, referensi, lampiran tidak dihitung) | Template | **Perlu dicek**: pada PDF yang dikumpulkan, Bagian 1-6 menempati halaman 2 sampai 16 (sekitar 15 halaman) |
| Pakai Template resmi apa adanya, urutan bagian tidak boleh diubah | Panduan | Dipenuhi (urutan bagian 1-7 sesuai template) |
| Tabel alokasi pin: tidak ada pin dobel, pin input-only dan strapping ESP32 tidak disalahgunakan | Template §3.4 | Ada, tetapi **SDA tertulis GPIO 22 (sama dengan SCL)**; yang benar GPIO 21. Perbaiki di revisi |
| Power budget ditutup 1 baris: catu daya, arus rated, margin di atas peak total | Template §3.3 | Dipenuhi (satu baris di bawah tabel 3.3) |
| Rail aktuator terpisah dari rail logika, satu ground bersama | Panduan | Tidak ada motor atau aktuator berarus besar. Rail 3,3 V (ESP32, HX711, LED ≈ 6 mA) dan rail 5 V dari VIN (LCD), satu ground bersama. |
| Design review: anggota manapun bisa ditanya bagian manapun | Panduan | Semua anggota perlu paham seluruh desain, bukan hanya subsistem sendiri |

---

## 5. EVOLUSI PERUMUSAN MASALAH

Keputusan populasi target berubah beberapa kali selama perumusan awal - dicatat di sini supaya alasan penolakan versi-versi sebelumnya tidak hilang.

| Versi | Populasi target | Alasan ditolak/direvisi |
|-------|------------------|--------------------------|
| **v1 (motivasi medis dasar - lihat catatan di bawah)** | Pasien pasca-stroke | Terlalu besar skopnya untuk proyek satu semester; butuh proses rekrutmen populasi rentan yang lebih rumit dari yang bisa ditangani jadwal kelas. **TIDAK dibuang** - dipakai kembali di v5 sebagai rujukan kebutuhan klinis, bukan sebagai populasi yang diuji |
| v2 | Mahasiswa teknik, argumen berbasis tugas okupasional (mengetik/coding, menyolder, menggambar manual) | Lebih kuat, tapi populasinya (lintas program studi) masih sulit direkrut secara realistis |
| v3 (superseded oleh v5) | Mahasiswa dari tiga program studi serumpun | Sempat dipilih karena: (1) tidak mudah ditebak arah hasilnya; (2) populasi realistis direkrut karena serumpun; (3) penulis sendiri bagian dari populasi tersebut. **Dibatalkan**: tetap merupakan pengujian pada orang di luar anggota tim (lintas jurusan), yang menurut panduan tugas ("Testing on patients or on anyone outside your team") perlu persetujuan tertulis dosen sebelum dokumen formal diajukan - risiko approval tidak turun tepat waktu sebelum tenggat Minggu 5 dianggap terlalu tinggi |
| v4 (superseded oleh v5) | Mahasiswa satu program studi saja | Sempat dicatat sebagai fallback kalau rekrutmen 3 jurusan (v3) tidak realistis. **Dibatalkan untuk alasan yang sama seperti v3**: tetap pengujian di luar anggota tim, tetap butuh persetujuan yang sama, cuma skalanya lebih kecil - tidak menyelesaikan masalah kepatuhan, cuma mengecilkannya |
| **v5 (final)** | **Hanya anggota tim sendiri** - tidak ada pengujian ke pasien maupun ke mahasiswa/pihak lain di luar tim | Dipilih karena sesuai persis dengan batas yang diizinkan panduan tugas: *"Non-invasive, low-voltage measurements on team members are fine"* - dynamometry genggam tangan non-invasif dan tegangan rendah, jadi seluruh pengukuran validasi (kalibrasi, MVC, protokol V1) bisa dijalankan tanpa perlu persetujuan tertulis dosen sama sekali. Pertanyaan "apakah jurusan berkorelasi dengan kekuatan genggam" (inti v3) **tidak lagi jadi tujuan proyek** - dicatat sebagai future work/di luar cakupan mata kuliah, bukan sesuatu yang diklaim sudah dijawab. Motivasi klinis proyek tetap dari v1 (lihat catatan di bawah), dan celah yang diisi proyek berubah dari "pertanyaan lintas-jurusan" menjadi "logging otomatis yang tidak dimiliki alat komersial" (lihat catatan di bawah) |

*v5 adalah rencana final. v1-v4 tetap didokumentasikan di sini supaya jejak alasan penolakan tiap versi tidak hilang - terutama v1, yang isinya (motivasi medis) tetap dipakai lagi di v5 walau populasi ujinya tidak.*

> **v1 sebagai motivasi medis dasar (bukan populasi uji).** Kebutuhan klinis yang memotivasi proyek ini tetap argumen v1: pasien pasca-stroke dan populasi dengan penurunan kekuatan genggam adalah alasan HGS penting diukur dan dipantau secara klinis (R1 - Ramadhani et al. memakai populasi ini langsung; V1/V2/V4 - Vaishya et al. mendokumentasikan HGS sebagai vital sign dan ambang klinisnya). Catatan ini memakai v1 sebagai *rujukan kebutuhan klinis*, bukan sebagai populasi yang direkrut - device dirancang mengikuti protokol dan rentang yang relevan untuk populasi tersebut, tapi pengujian aktual di mata kuliah ini (Minggu 5-14) hanya dilakukan pada anggota tim, sesuai v5.

> **Celah yang diisi proyek (pengganti pertanyaan lintas-jurusan v3): logging.** Kebanyakan hand grip dynamometer komersial (termasuk model analog klasik seperti JAMAR) tidak punya pencatatan data otomatis - pencatat harus menyalin nilai secara manual satu per satu, yang rawan human error dan kehilangan data (C2 - Chang & Chen 2015 secara eksplisit menyebut ini sebagai kelemahan dynamometer tradisional yang coba diatasi versi digital-terintegrasi). Argumen v2 dari Vaishya (nilai klinis HGS datang dari pengukuran serial dari waktu ke waktu, bukan sekali baca) berarti logging bukan fitur tambahan - itu bagian dari kenapa alat ini punya nilai dibanding alat manual. Kontribusi proyek: menambahkan logging otomatis yang tidak dimiliki alat pembanding.

> **Standar keberhasilan proyek disederhanakan jadi dua hal saja (menggantikan daftar 5-6 fitur "wajib" di Bagian 6 sebagai kriteria sukses utama):**
> 1. **Measurement** - pengukuran sesuai protokol standar V1 (duduk, siku ditekuk 90°, tahan 3-5 detik, 3 kali percobaan, istirahat ~1 menit antar percobaan), diverifikasi dengan RMSE/MAE terhadap beban referensi (M5).
> 2. **Logging** - data setiap sesi tersimpan otomatis (tidak hilang, tidak perlu disalin manual) ke berkas CSV di laptop host lewat USB-Serial. ESP32 mengirim baris CSV dengan `Serial.print()`, dan skrip Python `logger.py` berbasis pyserial di laptop yang menulis berkas dan melakukan flush tiap baris (pola modul praktikum: *"Logging in Guided Example 4 now goes to a CSV file on your own laptop over the USB serial link"*). Cloud logging bukan requirement: panduan tugas mengelompokkannya sejajar Raspberry Pi sebagai tambahan opsional (*"may be added where the problem needs them; say why in the proposal"*), jadi masuk stretch goals. SD card tidak dipakai karena lab hanya punya satu modul SD sebagai stasiun opsional (risiko rebutan). Trade-off: logging membutuhkan laptop host tersambung selama sesi.

> **Rumusan masalah (draf kerja, versi v5):**
> Kekuatan genggam (HGS) makin diakui sebagai indikator kesehatan, dan nilai klinisnya terutama datang dari pengukuran berulang dari waktu ke waktu (V2). Dynamometer komersial umumnya menampilkan angka tanpa pencatatan otomatis sehingga nilai disalin manual (C2). Proyek ini membuat dynamometer genggam digital berbasis ESP32 dan load cell yang mengikuti protokol pengukuran standar (V1: tiga percobaan, tahan 3-5 detik, istirahat sekitar satu menit) dan mencatat tiap sesi secara otomatis ke CSV. Alat hanya divalidasi pada anggota tim sendiri (v5); pertanyaan perbandingan antar-jurusan dari v3 tidak dikerjakan (future work). Catatan: teks ini draf kerja dari catatan ini.

---

## 6. FUNGSI PERANGKAT (Keputusan Skop)

| # | Fungsi | Topik kuliah terkait | Sumber ide | Status |
|---|--------|-------------------|------------|--------|
| 1 | Pembacaan gaya real-time (kg/N) | ADC | Semua paper referensi | Wajib |
| 2 | Penangkapan gaya puncak per percobaan (MVC) | topik kuliah | Ramadhani (R1), Gotthelf (G1), protokol Vaishya (V1) | Wajib |
| 3 | Tampilan hasil (LCD/serial) | topik kuliah | Semua paper referensi | Wajib |
| 4 | Umpan balik aktuator (LED, menyala selama proses genggam) | aktuator | Tidak ada di paper - ditambahkan agar topik kuliah punya tempat wajar di perangkat ini | Wajib |
| 5 | Pencatatan data lintas-sesi (CSV lokal - inti; cloud - stretch goal) | IoT, dipelajari generik di modul praktikum | Argumen utama Vaishya: nilai klinis HGS datang dari **pengukuran serial**, bukan sekali baca (V2). Status cloud direvisi setelah dicek ke panduan tugas: *"cloud logging... may be added where the problem needs them"* - dikelompokkan eksplisit sejajar dengan Raspberry Pi sebagai opsional, bukan requirement inti | Wajib (CSV) / Opsional (cloud) |
| 6 | Rata-rata 3 percobaan + timer istirahat | - | Protokol standar Box 1, Vaishya (V1): 3 percobaan per tangan, istirahat ~1 menit | Opsional (murah, hanya logika software) |

**Sengaja TIDAK dimasukkan** (di luar skop "uji keterampilan teknis"):
- Relative HGS (V3, butuh input tinggi/berat + logika tambahan) - **dipindah ke tahap analisis data pasca-pengukuran**, bukan fitur firmware
- Game biofeedback penuh ala Gotthelf - itu proyek software/UX, bukan embedded
- Sensor jari (FSR) + IMU tremor ala Becerra - di luar cakupan penilaian mata kuliah

**Variabel yang tetap dicatat manual (bukan fitur firmware), untuk analisis data di makalah akhir:**
tinggi badan, berat badan, tangan dominan, dan frekuensi olahraga. Opsional, hanya untuk analisis pasca-pengukuran pada anggota tim (v5), misalnya menghitung relative HGS (V3). Perbandingan antar-jurusan bukan tujuan proyek (future work).

---

## 7. STATUS ITEM TERBUKA DAN BOM

| Area | Status | Catatan |
|---|---|---|
| Spesifikasi terukur | Terbuka | Akurasi sistem penuh diverifikasi lewat RMSE di G4 (M5). Rentang 20-70 kg masih perlu justifikasi dari kekuatan genggam anggota tim |
| Diagram blok, pin, power budget | Dikerjakan di PCB `hgd-v1` | Lihat Bagian 2, 5, dan 8 di [desain-pcb-hgd.md](pcb/desain-pcb-hgd.md) (alokasi pin, rail, netlist) |
| Media logging | Selesai | CSV lewat USB-Serial dengan `logger.py` (inti); cloud = stretch goal; SD card tidak dipakai (Bagian 5 dan Bagian 15.1 di [alur-program-hgd.md](firmware/alur-program-hgd.md)) |
| Deteksi "data tidak tersimpan" (GAGALSIMPAN) | Selesai | Protokol ACK (Bagian 4.8 dan 9 di [alur-program-hgd.md](firmware/alur-program-hgd.md)): `logger.py` menulis CSV dulu lalu ACK, NAK bila gagal tulis; timeout ACK 500 ms dari uji E5; diuji U9, U10, X4, X14 (23-24 Okt. 2026) |
| Sumber riwayat sesi sebelumnya | Terbuka | Layar RINGKASAN menyebut perbandingan dengan sesi sebelumnya, tetapi CSV berada di laptop, bukan di ESP32. Pilihan: (a) ESP32 menyimpan ringkasan sesi terakhir di flash (NVS), (b) `logger.py` mengirim riwayat kembali, (c) perbandingan dilakukan di analisis pasca-pengukuran dan layar hanya menampilkan sesi berjalan. Perlu diputuskan, dan ditulis di Bagian 15 di [alur-program-hgd.md](firmware/alur-program-hgd.md) |
| Angka yang menunggu uji fisik | Sebagian | ✔ `thresholdOnset` = 1,0 kg (E1, 15 Okt. 2026: σ 38,2 counts = 0,0007 kg). Masih terbuka: faktor kalibrasi (counts per kg; perkiraan awal sekitar 53.700, rasiometrik), `thresholdStabilitas`, refresh rate LCD (kisaran 50-500 ms), durasi tare (sekitar 1 s), timeout HX711, ambang saturasi ADC, durasi tekan-tahan (1-2 s), timeout ACK |
| Desain housing grip | Terbuka | Layout dipilih ([HL4](housing-load-cell/desain-housing-load-cell.md#hl4-tata-letak-terpilih-dua-batang-dengan-load-cell-di-antaranya)). Pelat PLA 7 mm terbukti terlalu lentur; pelat aluminium polos 10 mm diputuskan, celah 2,5 mm, spacer PLA terpisah 28 x 28 x 2,5 mm, baut M6 x 25, untuk load cell CZL 601 80 kg ([HL9](housing-load-cell/desain-housing-load-cell.md#hl9-temuan-kekakuan-batang-dan-usulan-bahan)). **Load cell sudah diukur (10 Okt. 2026): 130 x 28 x 21,5 mm.** Lebar 28 mm terkonfirmasi (menyelesaikan beda 28 vs 30 mm), tinggi 21,5 mm, bukan 22 mm, sehingga tebal total berkurang sekitar 0,5 mm (hitungan sederhana: 10 + 2,5 + 21,5 + 2,5 + 10 + ridge 5 = 51,5 mm); dokumen housing sudah dihitung ulang (v9, 11 Okt. 2026; kabel keluar dari muka samping, lubang Ø5 mm, klip kabel menyatu dengan Spacer A, panah beban terukur): aluminium 6061-T6, lubang Ø6,6 pelat dan Ø7,0 spacer, ulir M6 tembus dari kedua sisi (terukur), kabel Ø4 mm sekitar 28 cm, baut ISO 4017 M6 x 25 torsi 3 N·m, alas karet 6 mm. Gambar SVG v5 belum digambar ulang. Masih menunggu pengukuran: jarak lubang dan jarak dari ujung badan, tinggi penutup strain gauge, dan prototipe kardus |
| Catu LCD (3,3 V atau 5 V) | **Selesai (15 Okt. 2026)** | Diukur: dengan backpack di VIN, SDA/SCL diam di 4,93 V (melebihi batas GPIO ESP32 ≈ 3,6 V); di 3V3 kontras LCD gagal walau trimpot diputar ±135°. Keputusan: LCD dan backpack dari VIN (5 V), SDA/SCL lewat **modul level shifter dua arah 4 kanal BSS138** (sudah tiba). Netlist `hgd-v1` sudah memuat rail +5V dan J3/J4 untuk shifter (Bagian 6 di [desain-pcb-hgd.md](pcb/desain-pcb-hgd.md)) |
| Modul HX711, LCD, level shifter | Sebagian | HX711 dan LCD ada dan lolos uji bench (15 Okt. 2026); ukuran HX711 24 x 15,6 mm dan LCD diukur. Daftar cek kedatangan DS-HX1, DS-LCD1, DS-I2C1 sebagian terisi. Belum: E+/E− HX711 (U6), skema papan HX711 yang dibeli. Level shifter sudah tiba; jarak baris header 10,16 mm (DS-LLS1) |
| Uji load cell | Sebagian | Resistansi lulus dua kali; uji bertegangan lewat HX711 berjalan (15 Okt. 2026); E1 resmi setelah pemanasan (16 Okt. 2026): σ 35,0 counts (0,0007 kg), peak-to-peak 0,004 kg; arah tekan positif. Percobaan E1 pertama tidak valid (jumper DT/SCK longgar). Langkah di [protokol-uji-bench-hgd.md](firmware/protokol-uji-bench-hgd.md). Belum: kalibrasi, linearitas, E1 ulang setelah housing dirakit |
| PCB `hgd-v1` | Berjalan | Skema dan netlist benar (15 Okt. 2026); R1 220 ohm; DevKit dan semua soket di sisi bawah; PCB v1 selesai digambar 16 Okt. 2026: 80 x 55 mm, lubang H1-H4 Ø3,0 (pola 75 x 50, segaris PCB LCD, baut M2,5), jalur dua lapis, silkscreen nama pin J1-J4, lolos DRC KiCad (Bagian 8 di [desain-pcb-hgd.md](pcb/desain-pcb-hgd.md)). Sisa: persetujuan, gerber, persyaratan tugas PCB dari kelas |
| Jadwal | Lihat Bagian 4.1 | Terkait konflik tenggat gerber (Bagian 4.1) |
| Risiko | Belum didokumentasikan secara formal | Kandidat dari desain housing: batang melentur menyentuh load cell atau patah ([HL9](housing-load-cell/desain-housing-load-cell.md#hl9-temuan-kekakuan-batang-dan-usulan-bahan)); lubang baut mengurangi penampang pelat di pangkal; kabel load cell tertarik terbaca sebagai gaya; dua bagian bersentuhan (jalur gaya paralel); arah beban kalibrasi, kalibrasi hanya sampai 42 kg dengan ekstrapolasi ke 70 kgf, dan A yang bertumpu pada kepala baut saat kalibrasi ([HL10](housing-load-cell/desain-housing-load-cell.md#hl10-kalibrasi-dan-arah-beban)); kabel load cell pendek (sekitar 28-42 cm) dan diameternya belum diukur ([HL4](housing-load-cell/desain-housing-load-cell.md#hl4-tata-letak-terpilih-dua-batang-dengan-load-cell-di-antaranya)); pelat logam butuh akses bengkel dan anggaran ([HL11](housing-load-cell/desain-housing-load-cell.md#hl11-pengadaan-pelat-logam)); ketersediaan beban acuan (dumbbell berpasangan sampai 42 kg milik anggota) |
| Consent partisipan | Tidak relevan | v5 hanya menguji anggota tim sendiri (Bagian 5) |
| Link supplier | Terbuka | URL listing Shopee belum dicatat di sini |

### 7.1 BOM

| No | Komponen | Part number | Qty | Sumber | Harga satuan | Subtotal | Catatan |
|---|---|---|---|---|---|---|---|
| 1 | ESP32 DevKit | ESP32-WROOM-32 | 1 | Lab | - | - | Disediakan lab (tim sudah memiliki papan DevKit V1; dimensi diukur 10 Okt. 2026: 51,5 x 28,2 mm, 15 pin per baris, jarak baris 25,3 mm; rinci di tab ESP32 pada [tabel pengukuran](pengukuran/tabel-pengukuran-hgd.xlsx) dan DS-ESP2). Cek varian fisik (WROOM atau WROVER) sebelum pin dikunci: GPIO 16/17 dipakai PSRAM di WROVER, sedangkan GPIO 18/19 aman di keduanya. Uji 8 Okt. 2026 (DS-ESP1 di `datasheets/datasheet-library.md`): chip ESP32-D0WD-V3 rev v3.1, flash 4 MB, tanpa PSRAM; pin 18, 19, 21, 22, 25, 27 lolos loopback; akhiran modul (WROOM-32D atau -32E) belum dipastikan |
| 2 | Load cell straight-bar 180 kg (4 kabel, kabel 110 cm, shield) | Generik ("Nankai/YZC-133", tanpa P/N pabrikan) | 1 | Beli (Shopee) | Rp193.375 | Rp193.375 | Rated output 2,0±0,2 mV/V, non-linearity 0,02% FS, kelas C3, creep 0,0016% FS (30 menit), overload aman 150% FS, destruktif 200% FS, eksitasi 4-12 VDC (maks 15 V), IP67, aluminium, 147 x 30 x 22 mm. Harga naik dari estimasi awal Rp115.000 |
| 3 | Modul HX711 | HX711 (papan XFW-HX711) | 1 | Beli | Rp8.500 | Rp8.500 | Channel A saja; daya 2,7-5 V dari ESP32. Header jantan belum disolder; modul belum ada di tangan pada 10 Okt. 2026. Catu 3,3 V mengikuti DS-HX1 Peringatan 2 (level DT), dengan eksitasi load cell yang lebih rendah dari pada 5 V |
| 4 | LCD 16x2 + backpack I2C | LCD1602 (HD44780) + PCF8574T | 1 | Beli | Rp39.500 | Rp39.500 | Catu 5 V dari VIN, SDA/SCL lewat level shifter (diputuskan 15 Okt. 2026). Datang dengan backpack I2C (pin jantan). Alamat I2C 0x27 (✔) |
| 5 | LED 5 mm diffused (paket 30 pcs) | Generik | 1 paket | Beli | Rp10.000 | Rp10.000 | Tidak bisa dibeli eceran |
| 6 | Tactile push button | 6x5x5 mm, 2-pin (BOM) | 5 | Beli | Rp500 | Rp2.500 | Hanya butuh 1 (satu tombol untuk START/NEXT/RESTART); MOQ 5. Catatan lama menulis 6x6x5 mm: ukur untuk memastikan (dokumen [tabel-pengukuran-hgd.xlsx](pengukuran/tabel-pengukuran-hgd.xlsx)) |
| 7 | Modul level shifter dua arah 4 kanal (BSS138) | Generik | 1 | Beli | belum dicatat | belum dicatat | Ditambahkan 15 Okt. 2026 untuk SDA/SCL LCD 5 V. Harga dari bukti beli; total di bawah belum memuatnya |
| | Total pembelian | | | | | **Rp253.875** + ongkir | Batas Rp300.000. Belum memuat level shifter (baris 7) dan harga load cell CZL 601 (Bagian 9.1.1) |

Resistor pembatas LED (220 ohm) termasuk komponen pasif lab (panduan D.5) dan tidak dibeli. Breadboard dan kabel jumper juga disediakan lab.

#### 9.1.1 Perubahan BOM (v37)

Perubahan terhadap tabel di atas:
- **Load cell diganti.** Yang dibeli adalah CZL 601 80 kg (listing Automa-88, Shopee; DS-LC1 di `datasheets/datasheet-library.md`), bukan load cell generik 180 kg pada baris 2. Spesifikasi di baris 2 (2,0 mV/V, overload 150%, kabel 110 cm, 147 x 30 x 22 mm) tidak berlaku lagi. Nilai baru dari DS-LC1: 130 x 28 x 22 mm (gambar penjual; GJ Impex menulis lebar 30 mm); **diukur sendiri pada barang yang tiba (10 Okt. 2026): 130 x 28 x 21,5 mm**, sensitivitas 2,0±0,2 mV/V (vendor), error gabungan ±0,03 %RO (sekitar ±24 g, sekitar ±34 g bila digabung dengan suhu dan drift HX711), overload yang dipakai desain 120% / 150% (96 / 120 kgf), kabel sekitar 28 cm (listing) atau 0,42 m (GJ Impex). Harga pembelian tidak tercatat di catatan ini: isi dari bukti beli, lalu hitung ulang total dan sisa anggaran (Rp253.875 di atas masih memuat Rp193.375 untuk load cell lama).
- **Komponen housing grip yang belum ada di BOM** ([HL11](housing-load-cell/desain-housing-load-cell.md#hl11-pengadaan-pelat-logam)): pelat aluminium 170 x 30 x 10 mm (2 pcs + 1 cadangan; grade dicantumkan) dan jasa potong atau bor, baut M6 x 25 dan washer logam (4 buah, lebih satu set cadangan), filamen PLA untuk spacer 28 x 28 x 2,5 mm dan penjepit kabel (printer lab), serta kabel sambungan load cell bila housing elektronik tidak berada dalam jangkauan kabel (≈28 cm; sambungan di luar lingkup dokumen housing). Harga belum diverifikasi.
- Anggota tim sudah memiliki ESP32 DevKit V1, LED 5 mm, tombol, resistor, breadboard, dan kabel jumper; LCD (dengan backpack I2C), modul HX711, dan load cell sudah dibeli. Load cell sudah diterima dan diperiksa (Bagian 9.1 di [desain-pcb-hgd.md](pcb/desain-pcb-hgd.md)); status fisik modul HX711 dikonfirmasi (Bagian 7). Daftar komponen yang diminta dari lab disusun terpisah (tab terpisah).
- **Tambahan untuk PCB `hgd-v1`** (Bagian 8 di [desain-pcb-hgd.md](pcb/desain-pcb-hgd.md); menggantikan daftar PCB v1): soket betina 1x15 (2 pcs), soket betina 1x4 (2 pcs) dan 1x6 (2 pcs), kabel jumper betina-jantan (F-M) untuk LCD, kapasitor 100 nF keramik (2) dan 10 uF elektrolit (2) dari komponen lab, resistor 220 ohm. Harga belum dicatat.

---

## 8. RINGKASAN DESAIN HOUSING

Desain housing dipisah ke dokumen sendiri supaya catatan ini tidak melebar:

| Dokumen | Isi | Status |
|---|---|---|
| [desain-housing-load-cell.md](housing-load-cell/desain-housing-load-cell.md) | Housing grip: layout dua batang dengan load cell, kekakuan batang, kalibrasi (protokol dumbbell), pengadaan pelat, rujukan, pointer ke `asumsi.md` di `drawings/` | Ada (v10) |
| [desain-housing-elektronik.md](housing-elektronik/desain-housing-elektronik.md) | Housing PCB, ESP32, LCD, HX711, level shifter, tombol, LED; parameter Fusion dan STEP di `fusion/` | v5: desain v1 selesai, belum disetujui |

Ringkasan status (rinci di dokumen housing grip):
- Dua housing terpisah: grip hanya berisi load cell, elektronik di housing sendiri ([HL1](housing-load-cell/desain-housing-load-cell.md#hl1-keputusan-dua-housing-terpisah)).
- Layout grip: batang A (telapak, tetap) dan batang B (jari, bergerak) dengan load cell di antaranya ([HL4](housing-load-cell/desain-housing-load-cell.md#hl4-tata-letak-terpilih-dua-batang-dengan-load-cell-di-antaranya)).
- Pelat PLA 7 mm terbukti terlalu lentur. Diputuskan: pelat polos aluminium 10 mm (tebal total 51,5 mm tanpa kepala baut, dengan tinggi load cell terukur 21,5 mm) dengan celah 2,5 mm dan spacer PLA terpisah 28 x 28 x 2,5 mm untuk load cell CZL 601; lenturan ujung jauh 0,24 mm pada 70 kgf, sisa celah ≈0,46 mm setelah anggaran celah (lenturan load cell 0,5, penutup 1,0, margin 0,3 mm; asumsi desain) ([HL9](housing-load-cell/desain-housing-load-cell.md#hl9-temuan-kekakuan-batang-dan-usulan-bahan)). Diputuskan 11 Okt. 2026: 6061-T6, lebar load cell 28 mm, lubang Ø6,6 mm, ulir M6 tembus, kabel Ø4 mm.
- Load cell sudah diukur sebagian (130 x 28 x 21,5 mm; dokumen [tabel-pengukuran-hgd.xlsx](pengukuran/tabel-pengukuran-hgd.xlsx) dan Bagian 9.1 di [desain-pcb-hgd.md](pcb/desain-pcb-hgd.md)). Menunggu: sisa pengukuran load cell (dokumen [tabel-pengukuran-hgd.xlsx](pengukuran/tabel-pengukuran-hgd.xlsx)), jawaban dosen atau TA dan konsultasi teknik mesin ([HL20](housing-load-cell/desain-housing-load-cell.md#hl20-pertanyaan-terbuka-untuk-dosen-atau-ta-dan-konsultasi-teknik-mesin)), serta prototipe kardus ([HL14](housing-load-cell/desain-housing-load-cell.md#hl14-urutan-prototipe-kardus-sampai-cetak)).
- Kalibrasi memakai protokol dumbbell sampai 42 kg ([protokol-kalibrasi-hgd.md](kalibrasi/protokol-kalibrasi-hgd.md), [HL10](housing-load-cell/desain-housing-load-cell.md#hl10-kalibrasi-dan-arah-beban)); 42 sampai 70 kgf adalah ekstrapolasi, dan protokol belum diuji.

Tabel pengukuran jangka sorong (dokumen [tabel-pengukuran-hgd.xlsx](pengukuran/tabel-pengukuran-hgd.xlsx)) ada di dokumen sendiri karena dipakai bersama oleh kedua housing dan layout PCB.

---

## 9. PENGUJIAN DAN VERIFIKASI

### 9.1 Verifikasi spesifikasi (gerbang G4)
| Spesifikasi | Cara verifikasi |
|-------------|-----------------|
| S1 Rentang | Kalibrasi (Bagian 8 di [alur-program-hgd.md](firmware/alur-program-hgd.md)); RMSE/MAE terhadap beban acuan; verifikasi dengan massa yang tidak dipakai fitting; 42-70 kgf dinyatakan ekstrapolasi |
| S2 Update hitung mundur | Ukur interval aktual antar-update selama 60 detik (600 update); rata-rata ≈ 100 ms dan tidak ada yang terlewat |
| S3 Daya | Multimeter atau lab supply, 30-60 menit operasi kontinu; tidak ada brownout; bandingkan dengan estimasi |
| S4 Pencatatan | 3 percobaan berurutan; cek isi CSV; uji tambahan blokir penulisan → `GAGALSIMPAN` dan nilai tetap di Serial |
| S5 LED | Ukur waktu nyala terhadap transisi masuk/keluar `GENGGAM`, 5 percobaan |

### 9.2 Gerbang proyek
| Gerbang | Isi | Bukti |
|---------|-----|-------|
| G1 | Gerbang awal kelas | Lihat Bagian 4.1 |
| G2 | Satu jalur sensing dan satu jalur aktuasi hidup di breadboard | U7 + LED/LCD berjalan |
| G3 | PCB dirakit dan uji dasar 3,3 V | U2 pada PCB, loopback pin pada PCB |
| G4 | Purwarupa terintegrasi lolos uji mandiri | S1-S5 + X1-X10 |

---

## 10. RISIKO DAN MITIGASI

Tiap risiko: tanda peringatan dini, mitigasi, dan fallback.

| # | Risiko | Tanda dini | Mitigasi | Fallback |
|---|--------|------------|----------|----------|
| R1 | Load cell tidak sesuai spesifikasi yang diasumsikan | Resistansi/dimensi/keluaran menyimpang | Pemeriksaan kedatangan (✔ resistansi dan dimensi lolos); uji linearitas sendiri | Kalibrasi ulang; ganti unit bila di luar toleransi |
| R2 | Rentang 20-70 kg hanya ≈ 25-88% F.S.; noise dan drift terasa pada ujung bawah | Noise besar pada E1 | Rata-rata buffer; tare tiap percobaan; dicatat σ | Naikkan jumlah sampel rata-rata |
| R3 | Beban acuan 70 kg tidak aman/tersedia | Tumpukan goyah | Kalibrasi sampai 42 kg dengan pemandu; 42-70 ekstrapolasi dinyatakan | Tambah titik dengan beban lain bila tersedia dan aman |
| R4 | PCB pertama bermasalah (lead time ≈ 2 minggu, tanpa iterasi kedua sebelum G4) | DRC gagal; loopback gagal di breadboard | Gerber dikumpulkan lebih awal; draf v1 take-home diperiksa lebih dulu; soket (bukan solder langsung) untuk ESP32 | Prototipe di breadboard/perfboard dengan kabel jumper |
| R5 | Timing firmware meleset (tare dan baca blocking; S2 menuntut 100 ms) | Hitung mundur tersendat | Pewaktuan `millis()`; blocking dibatasi ke dua titik | Turunkan beban LCD; refresh dipisah |
| R6 | Jalur logging bergantung laptop | Baris tidak tertulis | Protokol ACK + checksum; Serial Monitor sebagai cadangan | Salin manual dari Serial Monitor |
| R7 | Housing melentur/patah atau dua bagian bersentuhan | Pembacaan tidak linier; bunyi gesek | Aluminium 10 mm; celah 2,5 mm; uji beban bertahap | Pelat lebih tebal atau bracket logam |
| R8 | Lubang baut melemahkan penampang pelat di pangkal | Retak halus di sekitar lubang | σ_net dihitung (≈ 91 MPa untuk Ø6,6); counterbore tidak dipakai | Pelat lebih tebal |
| R9 | Kabel load cell tertarik terbaca sebagai gaya | Pembacaan bergeser saat kabel disentuh | Penjepit kabel di A; uji tarik kabel (E6) | Rute kabel diubah; tambah penahan |
| R10 | Pelat logam butuh akses bengkel dan anggaran | Tidak ada jasa potong | Pesan potong dan bor sekaligus; cadangan 1 pelat | Gunakan bengkel kampus atau jasa luar |
| R11 | Tegangan 5 V dari backpack LCD masuk ke pin ESP32 | SDA/SCL terukur 4,93 V (✔ 15 Okt. 2026) | Level shifter BSS138; GPIO 21/22 hanya ke sisi LV; uji VIN langsung di bench dibatasi singkat | Lepas pull-up backpack |
| R12 | Modul terpasang terbalik di soket tak berkunci (shifter terbalik 180° memberi 5 V ke sisi LV dan GPIO) | Modul panas/tidak menyala; LV terukur 5 V | Pin 1 dan sisi HV/LV ditandai di B.Silkscreen; cek tegangan LV sebelum ESP32 dicolok | Ganti modul; periksa GPIO 21/22 |
| R15 | Logam dan jalur dekat antena DevKit melemahkan radio | RSSI rendah bila WiFi/Bluetooth dipakai | Zona 15 mm di samping dan depan antena bebas tembaga, komponen, via; level shifter dipindah ke sisi pin 30 | Logging inti lewat USB, tidak bergantung radio |
| R13 | Kondensasi dan kelembapan merusak load cell | Nol bergeser, resistansi isolasi turun | Penyimpanan kedap udara dengan silika gel; suhu ruang sebelum buka | Ganti silika gel; keringkan perlahan |
| R14 | Beban lebih tak sengaja | Pembacaan > 70 kg | Validasi rentang; batas aman 96 kgf | Stop overload mekanis ditambahkan |

---

## 11. JADWAL DAN PEMBAGIAN KERJA

### 11.1 Pembagian peran
| Peran | Tanggung jawab |
|-------|----------------|
| Sensing dan PCB | Spesifikasi dan desain sistem, sensing, kalibrasi, power budget, skema dan layout PCB |
| Firmware dan antarmuka | State machine, LCD, LED, tombol |
| Logging dan enclosure | Jalur logging (`logger.py`), enclosure, BOM, dokumentasi |

> Design review: anggota mana pun dapat ditanya tentang bagian mana pun, jadi semua anggota memahami seluruh alur ini.

### 11.2 Jadwal
| Minggu | Tonggak |
|--------|---------|
| 5-6 | Proposal + design review; skema |
| **Rabu 14 Okt. 2026** | **Draf PCB v1 dibawa ke kelas** (take-home, non-formal) |
| 7 | Layout PCB dan gerber |
| 7 atau 8 | Tenggat berkas gerber (dua dokumen resmi berbeda; menunggu konfirmasi TA; direncanakan dikumpulkan lebih awal) |
| 9 | **G2**: satu jalur sensing dan satu jalur aktuasi hidup di breadboard; PCB dipesan kolektif |
| 10-11 | CAD casing; sertifikasi cetak 3D selesai paling lambat Minggu 11; `logger.py` |
| 11-12 | PCB diterima (≈ 2 minggu setelah pesanan) |
| 12 | **G3**: PCB dirakit, uji dasar 3,3 V |
| 14 | **G4**: purwarupa terintegrasi lolos uji mandiri; dry run S1-S5 |
| 15 | Menutup celah, draf makalah, latihan demo dan viva; evaluasi sejawat |
| 16 | Demo expo, makalah akhir, viva individu |

### 11.3 Urutan kerja (alur ketergantungan)
```
Pengadaan ──► Pemeriksaan kedatangan ──► Uji modul (U1-U10)
                                              │
Skema ──► PCB v1 ──► Layout/DRC ──► Gerber ──► Pesan ──► Rakit ──► G3
                                              │
Firmware: HX711 → tare → buffer → state machine → LCD/LED → ACK ──► G2/G4
                                              │
Housing: ukur LC → prototipe kardus → pesan pelat → cetak → rakit ──► Kalibrasi ──► G4
```

---

## 12. ANGGARAN DAN PENGADAAN

| No | Komponen | Qty | Harga | Status |
|----|----------|-----|-------|--------|
| 1 | ESP32 DevKit V1 | 1 | - (disediakan lab/tim) | ✔ ada |
| 2 | Load cell CZL 601 80 kg | 1 | Dicatat dari bukti beli | ✔ diterima |
| 3 | Modul HX711 | 1 | Rp8.500 | ✔ ada; lolos uji bench |
| 4 | LCD 16x2 + backpack I2C | 1 | Rp39.500 | ✔ ada |
| 5 | LED 5 mm (paket 30) | 1 paket | Rp10.000 | Komponen tim |
| 6 | Tactile push button (MOQ 5) | 5 | Rp2.500 | Komponen tim |
| 7 | Resistor, breadboard, kabel jumper | - | Disediakan lab | - |
| 8 | Soket betina 1x15, 1x4, 1x6; kabel jumper F-M | - | Dari lab/tim | Tambahan PCB `hgd-v1` |
| 8a | Modul level shifter 4 kanal BSS138 | 1 | Dicatat dari bukti beli | Sudah tiba |
| 9 | Kapasitor 100 nF dan 10 µF | 2 + 2 | Komponen lab | Decoupling 3V3 dan 5V |
| 10 | Pelat aluminium 170 x 30 x 10 mm (+ jasa potong/bor) | 2 + 1 | Belum dicatat | Belum dibeli |
| 11 | Baut M6 x 25 + washer | 4 + cadangan | Belum dicatat | Belum dibeli |
| 12 | Filamen PLA (spacer, penjepit kabel) | - | Printer lab | - |
| 13 | Kabel sambungan load cell (shielded 4 inti) | bila perlu | Belum dicatat | - |

- **Batas:** Rp300.000 per tim, dengan bukti pembelian.
- Pembelian yang tercatat (baris 3-6) Rp60.500. Sisa anggaran untuk load cell, pelat, baut, kabel, dan ongkir ≈ Rp239.500 (harga load cell dan total aktual dihitung ulang dari bukti beli).
- Tiap komponen yang dibeli punya tautan supplier riil.

---

## 13. KESELAMATAN

### 13.1 Keselamatan listrik
Sistem 3,3 V/5 V berdaya USB, tegangan rendah, tanpa baterai. Tidak ada tegangan jala. Backpack LCD dicatu 5 V hanya bila SDA/SCL ESP32 lewat level shifter (SDA/SCL backpack terukur 4,93 V).

### 13.2 Keselamatan mekanik
- Batas aman load cell 96 kgf; tidak ada penahan beban lebih (batasan).
- Kalibrasi: tumpukan beban (Bagian 8.8 di [alur-program-hgd.md](firmware/alur-program-hgd.md)), pemandu, sepatu tertutup.
- Baut M6 dikencangkan merata; periksa kekencangan sebelum tiap sesi.
- Pengguna: hanya anggota tim; berhenti bila tangan sakit.

### 13.3 Penyimpanan load cell
Lihat Bagian 4 di [desain-pcb-hgd.md](pcb/desain-pcb-hgd.md). Jangan dibawa lewat kabel, jangan ditumpuk beban, jangan dibiarkan mengalami benturan, dan jaga kering.

---

## 14. RIWAYAT KEPUTUSAN

| Topik | Sebelumnya | Sekarang | Alasan |
|-------|-----------|----------|--------|
| Populasi uji | Pasien pasca-stroke → mahasiswa lintas program → satu program | **Anggota tim sendiri** | Pengujian di luar tim butuh persetujuan tertulis; anggota tim sesuai batas non-invasif bertegangan rendah. Motivasi klinis tetap dari kebutuhan HGS |
| Tombol | Dua tombol | **Satu tombol** (tap vs tahan) | Yang menggenggam hanya anggota tim; pembedaan peran tidak perlu |
| Buzzer | Direncanakan | **Dihapus** | LED cukup; alat kecil yang diawasi tidak butuh sinyal audio |
| Pencatatan galat | Logbook manual | **CSV otomatis** | Logbook manual di tengah sesi sering terlewat |
| Deteksi data tidak tersimpan | Hanya deteksi USB putus | **ACK per baris + checksum** | USB putus tidak mendeteksi skrip mati atau baris terpotong |
| Media logging | Cloud / SD card | **CSV via USB-Serial** | SD hanya satu modul di lab; cloud opsional menurut panduan |
| Istirahat | State terpisah | **Sub-fase dalam `RESPON_ISTIRAHAT`** | Menyederhanakan hierarki |
| Threshold | Dua threshold | **Satu** | Cukup satu eksperimen |
| Load cell | Generik 180 kg | **CZL 601 80 kg** | Dibeli; 70 kgf = 87,5% kapasitas, jadi tidak ada "headroom besar" lagi |
| Bahan pelat | PLA 7 mm | **Aluminium polos 10 mm** | PLA terlalu lentur |
| Counterbore / tonjolan | Direncanakan | **Dihapus** (pelat polos + spacer PLA) | Tanpa frais |
| ESP32 di PCB | Disolder langsung | **Soket betina** | Papan bisa dicabut; sesuai persetujuan lab |
| LCD dan HX711 | Header langsung | **Kabel jumper F-M ke PCB** | Bagian yang aus adalah yang murah |
| Catu LCD | 3,3 V → diputuskan lewat pengukuran | **5 V dari VIN + level shifter BSS138 4 kanal** (15 Okt. 2026) | SDA/SCL backpack 4,93 V; di 3,3 V kontras gagal |
| Baterai | Dibahas | **Tidak dipakai; USB saja** | Logging butuh laptop tersambung; catu USB cukup |
| LCD dan HX711 (revisi) | Kabel jumper F-M ke header PCB | **Soket betina di PCB; HX711 dicolok langsung, LCD lewat kabel** | Jumper DT/SCK longgar merusak E1 pertama; LCD di tutup tetap perlu kabel |
| Sisi DevKit | Sisi atas | **Sisi bawah PCB** (semua soket dan kapasitor juga di bawah) | Di atas, tombol tidak cukup menonjol keluar tutup |
| R1 | 100 ohm | **220 ohm** | Nilai yang dipakai tim; LED ≈ 6 mA |
| Perangkat lunak PCB | EasyEDA (arahan modul) | **KiCad** (diizinkan tim pengajar, 17 Okt. 2026) | Skema dan PCB sudah dikerjakan di KiCad |
| LED daya | Tidak ada (hanya D1 di GPIO27) | **D2 LED merah 3 mm + R2 220 ohm dari +3V3** (17 Okt. 2026) | D1 hanya menyala bila firmware jalan; indikator daya menunjukkan papan berdaya walau firmware mati |

---

## 15. STATUS WIP

### 15.1 Kemajuan per bagian (17 Okt. 2026)
| Bagian | Status | Keterangan |
|--------|--------|------------|
| Pengadaan | ◐ | Semua modul elektronik ada (load cell, LCD + backpack, HX711, level shifter); pelat dan baut belum dibeli |
| Pemeriksaan load cell | ◐ | Dimensi dan resistansi ✔; uji bertegangan dan E1 ✔ (15 Okt. 2026); kalibrasi belum |
| PCB `hgd-v1` | ✔ | **v1 selesai (17 Okt. 2026), belum disetujui**: DRC 0 error; pour GND dua lapis + 57 via; LED daya D2; silkscreen tim; Gerber diekspor dan dicek di viewer JLCPCB (2 lapis, 80 x 55 mm); belum diunggah |
| Skema KiCad | ✔ | **v1 selesai (17 Okt. 2026), belum disetujui**: ERC 0 error; D2/R2; pull-up I2C dihitung dari ukuran (tanpa pull-up tambahan); breadboard sama dengan skema. PDF dan BOM belum |
| Firmware | ◐ | Langkah 0-10c dikode dan diuji di papan (24 Okt. 2026): state machine, tare, onset/plateau, istirahat, galat dan reset total, baris log + ACK dengan nomor boot, rekaman tiap sampel genggaman (`LOG_RAW`) dan penanda per upaya, layar LCD dua desimal. Tanpa istirahat setelah percobaan 3 (24 Okt. 2026). Belum: langkah 11 (watchdog, pengukuran waktu, build akhir Arduino IDE, lima skenario galat) |
| `logger.py` dan `kalibrasi.py` | ◐ | `logger.py` dikode dan diuji (U9, U10) 23 Okt. 2026, tulis-sebelum-ACK dan uji sesi penuh, X4, X14 24 Okt. 2026; langkah 10c: berkas galat dan rekaman genggaman, duplikat dengan nomor boot, penulisan terlambat bila berkas terkunci, uji U11-U14, X15, X16, E7 24 Okt. 2026 ([laptop/logger.py](laptop/logger.py)); `kalibrasi.py` belum dikode |
| Housing grip | ◐ | Desain dan hitungan ada; menunggu sisa pengukuran load cell dan prototipe kardus |
| Housing elektronik | ✔ | **Desain v1 selesai (17 Okt. 2026), belum disetujui**: lid, body, bibir, 4 snap-fit, slice uji; STEP di repo. Belum: lubang tombol/LED (termasuk D2), tiang PCB, cetak uji lidah; cetak menunggu PCB |
| Kalibrasi | ▣ | Protokol ada; belum dijalankan |
| Pengujian | ◐ | U1-U5, U7, E1 ✔; level shifter di breadboard ✔ (17 Okt. 2026); U8 sebagian; U9, U10 ✔ (23 Okt. 2026); X4, X14 ✔ (24 Okt. 2026); U11-U14, X15, X16, E7 ✔ (24 Okt. 2026, langkah 10c); U6 belum |
| Dokumen | ◐ | Catatan proyek v50, alur program v5, teks LCD v1, protokol uji bench v3, desain PCB v4, log kerja KiCad v1, README KiCad v5, housing elektronik v5, tabel pengukuran, sitasi v1, datasheet library |

### 15.2 Yang akan dikerjakan berikutnya
1. Persetujuan PCB v1, lalu unggah ZIP Gerber (batas 27 Okt. 2026, 23:59).
2. PDF skema dan BOM (kolom sesuai modul).
3. Bahan laporan PCB (tangkapan DRC dan 3D, lebar jalur 5V, data papan) dan review papan tim lain.
4. Housing elektronik: lubang tombol dan LED (SW1, D1, D2) dan tiang PCB di lid; cetak uji satu lidah dan slot; izin cetak; cetak setelah PCB tiba.
5. Jalankan U6 (E+/E− HX711) dan U8 penuh (10 tap, 10 tahan).
6. Ukur sisa load cell (dokumen [tabel-pengukuran-hgd.xlsx](pengukuran/tabel-pengukuran-hgd.xlsx)); housing sudah dihitung ulang dengan tinggi 21,5 mm (v8).
7. Kode firmware bertahap: sensor → tombol → mesin state → LCD → catat/ACK.
8. Kode `logger.py` dan jalankan U9, U10.
9. Prototipe kardus housing, lalu pesan pelat.
10. Susun daftar sitasi (Bagian 2 di [sitasi-hgd.md](sitasi/sitasi-hgd.md)).

### 15.3 Perubahan
| Versi | Tanggal | Perubahan |
|-------|---------|-----------|
| v1 | 13 Okt. 2026 | Alur proyek dipecah: firmware ke `alur-program-hgd.md`, perangkat keras ke `desain-pcb-hgd.md`, sisanya ke Bagian ini |
| WIP-0.1 | 10 Okt. 2026 | Dokumen alur proyek pertama: struktur data, konstanta, perangkat keras, fungsi, state machine, sinyal, kalibrasi, logging, antarmuka, housing, pengujian, risiko, jadwal, edge case, riwayat keputusan |

---

## 16. LOG CROSS-CHECK DAN AUDIT

*Jejak pemeriksaan catatan terhadap dokumen resmi (Rubrik, Panduan, Template).*

- Rubrik skor resmi: 4 kriteria (bukan 6), masing-masing 0-100, skor = rata-rata. Sudah dikoreksi di Bagian 4.2.
- Tenggat gerber: konflik panduan (Minggu 7) dan panduan tugas (Minggu 8) belum terkonfirmasi (Bagian 4.1).
- Media logging: SD card hanya fasilitas opsional lab (satu unit); cloud opsional menurut Panduan; keputusan: CSV lewat USB-Serial sebagai inti (Bagian 5 dan Bagian 15.1 di [alur-program-hgd.md](firmware/alur-program-hgd.md)).
- Persyaratan format resmi: status per item di Bagian 4.3.
- v52 (24 Okt. 2026): Tanpa istirahat setelah percobaan 3 (keputusan tim; menyimpang dari proposal, dicatat di daftar perubahan alur program butir 11): `Lepas>ringkasan` sampai dilepas, lalu langsung `NEXT > ringkasan`. Alur program Bagian 6.1-6.3, 9.5, 10.2, 16; teks LCD.
- v51 (24 Okt. 2026): Langkah 10c selesai dan diuji: nomor boot di semua baris log, berkas `hgd_galat_…` dan `hgd_genggam_…` (rekaman tiap sampel genggaman + penanda per upaya), penulisan terlambat bila berkas genggaman terkunci. Status firmware, `logger.py`, dan pengujian di Bagian 15.1 diperbarui; alur program Bagian 2, 3, 6.2, 9, 11, 12.2, 13.
- v50 (24 Okt. 2026): Status firmware diperbarui: langkah 0-10 dikode dan diuji di papan, termasuk `logger.py` (peta dokumen, Bagian 7 GAGALSIMPAN selesai, Bagian 15.1 firmware, logger, pengujian, dokumen). Peta dokumen: `firmware/teks-lcd-hgd.md` (v1).
- v49 (17 Okt. 2026): **Skema, PCB, dan housing elektronik versi 1 selesai, belum disetujui.** Status WIP (Bagian 15.1, 15.2), peta dokumen, dan posisi saat ini diperbarui; gambar `pcb/gambar/penempatan-pcb-hgd.svg` diperbarui dari papan final; desain PCB v4 (status v1 dan bukti di Bagian 8), README KiCad v5, housing elektronik v5 (status v1, sisa pekerjaan disusun ulang).
- v48 (17 Okt. 2026): Desain PCB v4: pour GND di F.Cu dan B.Cu (thermal relief, area antena kosong) dijahit 57 via GND; satu error spoke thermal pad 2 U2 dikecualikan dengan alasan; LED daya D2 (3 mm) + R2 220 ohm dari +3V3 ditambahkan ke skema dan PCB; ERC 0 error (no-connect pada pin tak terpakai, `PWR_FLAG` di GND); DRC 0 error. Hitungan pull-up I2C dari ukuran multimeter: shifter 9,86 kΩ (LV) dan 9,90 kΩ (HV), backpack 4,67 kΩ; gabungan LV 9,86 kΩ, HV 3,17 kΩ, keduanya dalam batas 1-11,8 kΩ, jadi PCB tanpa pull-up tambahan (datasheet library DS-LLS1, DS-I2C1). Header shifter disolder; jembatan LV1-LV2 dan HV1-HV2 ditemukan dengan uji kontinuitas dan diperbaiki; tegangan LV1/LV2 3,26 V, HV1/HV2 4,98 V. Uji breadboard dengan wiring sesuai skema (tanpa perbedaan): LCD bekerja lewat level shifter dan menampilkan data HX711; LED daya D2 menyala dengan R2 220 ohm. Protokol uji bench v3. Berkas KiCad di repo (skema, papan, salinan 3D verbatim, STEP rakitan PCB) diperbarui; STEP housing elektronik terbaru disimpan di `housing-elektronik/fusion/`; log langkah KiCad `pcb/log-kerja-kicad-hgd.md` dimulai. Gambar penempatan dan Gerber belum memuat D2/R2.
- v47 (16-17 Okt. 2026): Housing elektronik v4: usulan snap-fit lid ke body (4 lidah horizontal di bibir, Covestro desain 2, ε 0,44 % vs batas 0,57 % untuk PLA Basic) di Bagian 1.3, dengan celah bawah lidah 2,0, dua tiang korban (jembatan ≤ 10), dan slot berkompensasi susut; parameter snap ditambahkan ke `fusion_parameters_housing_elektronik.csv`; instruksi kerja Fusion `housing-elektronik/fusion/work_instructions_fusion_housing_elektronik.md`; gambar `housing-elektronik-hgd.svg` v2 bergaya gambar housing load cell. Bibir dan 4 snap-fit dimodelkan di Fusion (17 Okt. 2026), dicek dari STEP per fase; belum disetujui. Level shifter sudah tiba (status diperbarui di desain PCB, datasheet library, tabel pengadaan); protokol uji bench v2 dengan wiring shifter. Gambar `pcb/gambar/penempatan-pcb-hgd.svg` diperbarui: teks dari silkscreen berkas papan; Gerber `hgd-v1` diekspor dan dicek 17 Okt. 2026, disimpan di luar repo sampai disetujui.
- v46 (16 Okt. 2026): PCB `hgd-v1` v1 berjalur dan lolos DRC; gambar `pcb/gambar/penempatan-pcb-hgd.svg` diganti dengan papan final; skema `pcb/kicad/proyek/hgd-v1.kicad_sch` ditambahkan; desain PCB v3; belum disetujui. Housing elektronik v3: gambar model Fusion `housing-elektronik/gambar/housing-elektronik-hgd.svg` (lid berdinding, body baki, jendela USB dan kabel) dengan usulan bibir (belum diputuskan).
- v45 (16 Okt. 2026): data E1 lengkap di `pengukuran/e1-noise-hgd.xlsx`; protokol uji bench `firmware/protokol-uji-bench-hgd.md` (v1) ditambahkan; E1 resmi setelah pemanasan 11 menit (σ 35,0 counts; ambang onset tetap 1,0 kg); arah tekan positif; U8 kumulatif 5 TAP dan 3 TAHAN; alur program v4.
- v44 (16 Okt. 2026): hasil uji bench 15 Okt. 2026 dicatat (U4 0x27, U5 SDA/SCL 4,93 V, U7, U8 sebagian, E1 σ 38,2 counts); catu LCD diputuskan (5 V + level shifter BSS138 4 kanal, BOM baris 7); tanpa baterai; R1 220 ohm; PCB `hgd-v1`: DevKit dan soket di sisi bawah, HX711 dicolok ke J1, netlist KiCad, usulan penempatan v2 dengan zona antena; desain PCB v2, alur program v3, housing elektronik v2 dengan parameter Fusion dan kompensasi susut; tabel pengukuran LCD dan HX711; DS-LLS1 ditambahkan; risiko R11, R12 diperbarui dan R15 ditambahkan; sketsa `uji_bench_hgd` ditambahkan.
- v43 (15 Okt. 2026): alur program menjadi v2 (sitasi dan rujukan silang dikoreksi terhadap modul praktikum dan buku, istilah "interrupt transition" menjadi "transisi pengecualian", hal terbuka A5-A13 di Bagian 18); sketsa uji `uji_lcd_i2c`, `uji_hx711`, `uji_tombol_led` ditambahkan.
- v42 (13 Okt. 2026): catatan dipecah menjadi dokumen induk (ini) dan dokumen terpisah: `alur-program-hgd.md` (semua firmware, dari `alur-proyek-hgd.md` dan Bagian 6-8 lama), `desain-pcb-hgd.md` (Bagian 16 lama dan perangkat keras di alur), `desain-housing-elektronik.md`, `tabel-pengukuran-hgd.xlsx` (Bagian 15 lama), dan `sitasi-hgd.md` (Bagian 11 lama dan sitasi alur). `alur-proyek-hgd.md` dihapus; isi housing grip di alur sudah tercakup `desain-housing-load-cell.md`. Penomoran bagian berubah; nomor bagian di entri log v41 ke bawah memakai penomoran sebelum pemecahan.
- v41 (13 Okt. 2026): isi proposal dihapus dari catatan ini (Bagian 13 dikosongkan, audit proposal v26 dihapus, rujukan "sudah di proposal" diganti status dari catatan sendiri); catatan belum merujuk dokumen formal.
- v40 (13 Okt. 2026): ditambahkan Bagian 11.10 (daftar pustaka gaya IEEE untuk buku dan paper pada Bagian 11, dengan pemetaan kode lokal ke entri).
- v39 (12 Okt. 2026): identitas dihapus dari seluruh berkas (hanya "embedded system"); `catatan-riset-hgd.md` dan `catatan-riset-hgd-anon.md` (v30, pendahulu catatan ini) dihapus; `ds-library.md` (duplikat `datasheets/datasheet-library.md`) dihapus; salinan lama `koreksi*.md` diganti `koreksi.md` dan `../drawings/asumsi.md` versi gabungan; referensi versi dokumen housing menjadi v10; keterangan gambar v5 dan arah panah stiker diperbarui.
- v38 (10 Okt. 2026): pemeriksaan ulang seluruh catatan. Perubahan: identitas dan nama mata kuliah dihapus (hanya "embedded system"), tautan folder berbagi dihapus; status komponen diperbarui (load cell diterima dan diukur, LCD dan backpack, HX711 belum ada di tangan); hasil resistansi dan dimensi load cell dicatat; tabel jangka sorong (Bagian 15) diisi untuk ESP32 DevKit V1 dan load cell; tenggat dan isi draf PCB v1 ditambahkan (Bagian 3.1, 9, 16); **pertentangan catu LCD 3,3 V vs 5 V** dicatat; penamaan `SiklusPengukuran` di diagram Mermaid disamakan menjadi `SiklusPercobaan`.
- v37: dokumen housing menjadi v7: disesuaikan dengan load cell CZL 601 80 kg (130 x 28 x 22 mm), gambar v5 (spacer 28 mm, a = 37 mm, pelat aluminium 10 mm diputuskan, baut M6 x 25, kabel Ø5 mm asumsi), dan metode kalibrasi dumbbell (HL10); Bagian 9, 9.1.1, 14, 15 diperbarui.
- v36: dokumen housing menjadi v6 (parameter posisi spacer).
- v35: dokumen housing menjadi v5 (spacer PLA terpisah 30 x 30 mm; hitungan kekakuan diperbarui).
- v34: dokumen housing menjadi v4 (tautan ke folder `fusion/`).
- v33: dokumen housing menjadi v3 (gambar Cowork v4 dipasang di HL4 dan HL10).
- v32: diagram HL4 di dokumen housing diganti SVG mandiri; dokumen housing menjadi v2.
- v31: Bagian 14 (desain housing grip), sumber housing (11.9, 11.10), dan audit housing (v27, v28) dipisah ke dokumen housing; tabel pengukuran jangka sorong (Bagian 15) ada di dokumen sendiri karena dipakai bersama oleh kedua housing dan layout PCB.

### Audit v26: perbaikan catatan

Temuan pada catatan ini yang sudah diperbaiki di v26: harga load cell (Rp115.000 menjadi Rp193.375) dan total BOM; rumusan masalah yang masih berisi hipotesis lintas-jurusan (v3) diganti versi v5; media logging di Bagian 5 dan Bagian 7 yang masih menyebut cloud sebagai jalur utama; GagalSimpan yang masih bergantung pada "CSV gagal ditulis"; narasi UX yang menyebut dashboard dan "bar terisi" (tidak dirancang); label dan frasa yang merujuk ke sumber di luar catatan diganti penjelasan langsung.

### Audit desain housing

Audit v27 dan v28 (kekakuan batang, arah beban kalibrasi, hasil gambar v4) dipindah ke [desain-housing-load-cell.md, HL19](housing-load-cell/desain-housing-load-cell.md#hl19-riwayat-koreksi-dan-audit).

### Temuan audit v38

1. **Catu LCD:** catatan (Bagian 7.1) dan PCB v1 memakai 3,3 V, sedangkan DS-LCD1 menyebut 5,0 V. Selesai di v44: 5 V + level shifter (Bagian 7).
2. **Beda ukuran load cell:** gambar v5 dan dokumen housing memakai tinggi 22 mm; hasil ukur 21,5 mm. Sudah dihitung ulang di dokumen housing v8 (tebal total 51,5 mm); gambar sudah digambar ulang (12 Okt. 2026): potongan v11, exploded v12, setup kalibrasi v11, dan dua tampak klem v1, semuanya di folder `housing-grip/` pada Project.
3. **Warna kabel load cell:** datasheet memberi merah/hitam sebagai eksitasi dan hijau/putih sebagai sinyal; pada barang ini pengukuran menunjukkan eksitasi = hitam dan putih, sinyal = merah dan hijau (Bagian 9.1 di [desain-pcb-hgd.md](pcb/desain-pcb-hgd.md)). Jangan menyambung berdasarkan warna datasheet atau listing.
4. **README repo** masih menulis bahwa belum ada data pengukuran load cell. Diperbarui di v44 (ada data resistansi, dimensi, dan noise E1).
5. **Eksitasi load cell** diturunkan dari catu modul HX711 (3,3 V pada PCB v1), jadi skala penuh jembatan paling tinggi sekitar 6,6 mV (2,0 mV/V x 3,3 V), bukan 10 mV pada 5 V, dan sinyal per kg sekitar 0,083 mV, bukan 0,125 mV. Jumlah counts per kg praktis tetap (rasiometrik), tetapi noise relatif lebih besar. DS-HX1 memuat tabel perkiraan noise per eksitasi; ukur E+ pada modul yang tiba.

### Pertanyaan terbuka

- Apakah pengukuran memakai satu tangan dominan saja atau kedua tangan bergantian (protokol V1 menyebut bergantian)?
- Sumber riwayat sesi untuk perbandingan di layar RINGKASAN (Bagian 7).
- Apakah tugas PCB kelas mensyaratkan alat desain, ukuran maksimum, atau jumlah lapisan tertentu (materi belum tersedia).

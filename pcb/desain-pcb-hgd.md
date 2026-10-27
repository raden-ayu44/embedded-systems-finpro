---
title: Desain PCB dan Skema - Hand Grip Dynamometer

---

# Desain PCB dan Skema - Hand Grip Dynamometer (v8)
**Proyek akhir - embedded system**

> Skema dan PCB (skema termasuk): komponen, alokasi pin, rantai sinyal, load cell, catu daya, keputusan catu LCD, wiring antarmodul, PCB `hgd-v1` (KiCad), netlist, usulan penempatan, pemeriksaan load cell, dan sisa pekerjaan. Berkas netlist dan daftar komponen: `pcb_netlist.xlsx`; berkas KiCad: [kicad/](kicad/README.md); PDF skema: [kicad/pdf/skema-hgd-v1.pdf](kicad/pdf/skema-hgd-v1.pdf); gambar penempatan: [gambar/penempatan-pcb-hgd.svg](gambar/penempatan-pcb-hgd.svg); tangkapan ERC, DRC, Assign Footprints, dan render papan di [gambar/](gambar/); log langkah KiCad: [log-kerja-kicad-hgd.md](log-kerja-kicad-hgd.md). Firmware ada di [alur-program-hgd.md](../firmware/alur-program-hgd.md); induk: [catatan-proyek-hgd.md](../catatan-proyek-hgd.md).

---

## 1. Komponen
| Komponen | Tipe | Peran | Status |
|----------|------|-------|--------|
| Mikrokontroler | ESP32 DevKit V1 30 pin (chip ESP32-D0WD-V3 rev v3.1, flash 4 MB, tanpa PSRAM) | Firmware, state machine | ✔ ada; loopback pin lolos 8 Okt. 2026 |
| Load cell | CZL 601 80 kg, straight-bar, jembatan penuh, 2,0 ±0,2 mV/V | Sensor gaya | ✔ diterima; 130 x 28 x 21,5 mm; resistansi lolos (10 Okt. 2026) |
| Penguat + ADC | Modul HX711 (papan XFW-HX711), kanal A, gain 128, 10 SPS | Digitalisasi | ✔ ada; merespons dan lolos uji noise E1 (15 Okt. 2026); 24 x 15,6 mm terukur |
| Tampilan | LCD 16x2 HS1602A + backpack I2C (PCF8574T, alamat 0x27) | Teks per state | ✔ ada; alamat 0x27 terbaca (U4) |
| Penggeser level | Modul level shifter dua arah 4 kanal berbasis MOSFET BSS138 | SDA/SCL 5 V (LCD) ↔ 3,3 V (ESP32) | ✔ sudah tiba; jarak baris header 10,16 mm (jangka sorong) |
| Indikator | LED 5 mm + R1 **220 ohm** | Sinyal "sedang direkam" | ▣ dari komponen tim |
| Indikator daya | LED merah 3 mm (D2) + R2 **220 ohm** dari +3V3 | Menyala selama papan berdaya, tidak bergantung firmware | ✔ di skema dan PCB; diuji di breadboard (17 Okt. 2026) |
| Input | Tactile push button 6 x 6 mm, batang 13 mm, 1 buah | START, NEXT, RESTART | ▣ dari komponen tim |
| PCB | `hgd-v1` (KiCad), dua lapis, 80 x 55 mm | Dudukan DevKit, modul, tombol, LED | ✔ **v1 selesai, belum disetujui**; versi akhir 26 Okt. 2026 (jalur ulang, judul "Digital Hand Grip Dynamometer"): ERC 0 error, DRC 0 error, breadboard sama dengan skema, Gerber dicek di JLCDFM (Bagian 8) |
| Housing grip | 2 pelat Al 170 x 30 x 10 mm + spacer PLA + M6 x 25 | Mekanik | ◐ desain; sisa pengukuran load cell |

Daya hanya dari USB; **tanpa baterai** (keputusan 15 Okt. 2026).

---

## 2. Alokasi pin ✔ (loopback 8 Okt. 2026)
| Sinyal | GPIO | Pin header DevKit (U2) | Antarmuka | Catatan |
|--------|------|------------------------|-----------|---------|
| Tombol | GPIO 25 | 23 | Input, interrupt FALLING, pull-up internal | Satu tombol untuk semua fungsi |
| LED | GPIO 27 | 25 | Output digital | Via R1 220 ohm (≈ 6 mA pada Vf 2,0 V) |
| HX711 DT | GPIO 18 | 9 | Input (bit-bang) | Aman di WROOM dan WROVER |
| HX711 SCK | GPIO 19 | 10 | Output (bit-bang) | Idem |
| LCD SDA (sisi 3,3 V) | GPIO 21 | 11 | I2C, lewat level shifter | `Wire.begin(21, 22)` ditulis eksplisit |
| LCD SCL (sisi 3,3 V) | GPIO 22 | 14 | I2C, lewat level shifter | Alamat 0x27 (✔ U4) |
| 3V3 | - | 1 | Catu | HX711, sisi LV level shifter |
| VIN (5 V USB) | - | 30 | Catu | LCD dan sisi HV level shifter |
| GND | - | 2 dan 29 | Ground | Keduanya disambung |

> Nomor pin header mengikuti footprint U2 (pin 1 = 3V3, pin 15 = D23, pin 16 = EN, pin 30 = VIN). Pin yang **tidak dipakai** dan dihindari: GPIO 16/17 (PSRAM pada WROVER), strapping (0, 2, 5, 12, 15), input-only (34-39) untuk keluaran.

---

## 3. Rantai sinyal
```
Gaya genggam
   │  pelat A (telapak, tetap) dan pelat B (jari, bergerak) menjepit load cell
   ▼
Load cell CZL 601  ── 4 kabel: eksitasi (hitam, putih), sinyal (merah, hijau)
   │  tegangan diferensial: ≈ 0,083 mV/kg pada eksitasi 3,3 V
   ▼
HX711 (3,3 V) ── gain 128, ADC 24 bit, 10 SPS, keluaran serial DT/SCK
   ▼
ESP32  ── (raw − offset) / skala = kg ── state machine
   ├──► level shifter ──► LCD (I2C, 5 V)
   ├──► LED
   ├──◄ Tombol
   └──► USB-Serial ──► logger.py ──► CSV   (ACK kembali)
```

---

## 4. Load cell ✔ (diperiksa 10 Okt. 2026)
| Pemeriksaan | Hasil |
|-------------|-------|
| Dimensi | 130 x 28 x 21,5 mm (lebar 28 mm menyelesaikan beda 28 vs 30 mm) |
| Kabel | 4 kabel: merah, hitam, putih, hijau |
| Resistansi (multimeter, rentang 2k) | hitam-putih 393 ohm; merah-hijau 347 ohm; pasangan silang 260-306 ohm. Dua kali pengukuran, sama |
| Perbandingan datasheet | masukan 401 ±10 ohm (terukur 393 ✔), keluaran 350 ±5 ohm (terukur 347 ✔) |
| **Pemetaan warna** | **Eksitasi = hitam dan putih; sinyal = merah dan hijau**. Berbeda dari datasheet (merah/hitam eksitasi). Penyambungan memakai pemetaan terukur ini, bukan warna datasheet |
| Polaritas E+/E−, S+/S− | Tertukar hanya membalik tanda; sketsa uji punya perintah `n` untuk membalik tanda |
| Noise diam (E1, 15 Okt. 2026) | σ = 38,2 counts = 0,0007 kg; peak-to-peak 206 counts = 0,004 kg (706 sampel / 60 s). Rinci di Bagian 7 di [alur-program-hgd.md](../firmware/alur-program-hgd.md) |
| Penyimpanan | Kotak kedap udara, bungkus gelembung (sisi gelembung ke sel), silika gel di luar bungkus, laci bawah lemari; jauh dari kulkas; kabel diberi label (selotip kertas), ujung telanjang ditutup isolasi PVC atau selongsong susut |

---

## 5. Catu daya
| Rail | Tegangan | Sumber | Beban |
|------|----------|--------|-------|
| +3V3 | 3,3 V (✔ terukur 3,26-3,34 V) | Regulator papan ESP32 dari USB | ESP32, HX711, sisi LV level shifter, C1, C2 |
| +5V | 5 V (✔ VIN terukur 4,98-5,08 V; 4,93 V di backpack) | VIN papan (USB) | LCD + backpack, sisi HV level shifter, C3, C4 |

- Papan dicatu dari USB saja; tidak ada baterai dan tidak ada catu luar yang disambung ke `VIN` bersamaan dengan USB.
- Estimasi arus: tipikal ≈ 117 mA, puncak ≈ 540 mA (didominasi lonjakan WiFi TX, yang tidak aktif pada jalur logging inti); margin terhadap regulator 1 A. LED kini ≈ 6 mA (R1 220 ohm), bukan 15 mA; LED daya D2 menambah ≈ 6 mA tetap ((3,3 − 2,0) / 220). Diverifikasi di uji S3 (24 Okt. 2026, Bagian 5.1): D2 terukur 6,7 mA, beban luar total 43,5 mA diam / 44,3 mA puncak, total dari USB ≈ 100-150 mA (dengan estimasi papan DevKit).
- Eksitasi load cell diturunkan dari catu HX711 (E+ = AVDD). Pengukuran bersifat **rasiometrik**, jadi jumlah counts per kg praktis sama pada 5 V dan 3,3 V; yang berubah adalah noise relatif. Pada 3,3 V noise terukur σ ≈ 0,7 g (E1), jauh di bawah kebutuhan.
### 5.1 Arus tiap rail, terukur (uji S3, 24 Okt. 2026)
Langkah lengkap untuk mengulang pengukuran: [protokol-arus-rail-hgd.md](../pengukuran/protokol-arus-rail-hgd.md). Multimeter digital, arus DC lewat jack mA, dipasang **seri menggantikan kabel catu** tiap beban (kabel dicabut saat USB tidak tersambung; pemutusan dicek dengan uji kontinuitas sebelum dinyalakan). Rentang 200 mA, kecuali B2 pada rentang 2 mA. Rentang 2 mA terlalu kecil untuk LCD (26 mA): meter menampilkan lewat batas dan tegangan jatuh di meter membuat LCD tidak bekerja. Firmware UTS final (core 3.3.11), catu USB laptop.

- **Diam**: `SIAP` ("Tekan tmbl START"), tare selesai, kedip restart sudah berhenti, alat tidak disentuh.
- **Puncak**: `GENGGAM` dengan genggaman stabil (LED D1 menyala, "Lepas ok"): semua beban aktif bersamaan. Tanpa Wi-Fi/Bluetooth dan tanpa motor, ini keadaan beban tertinggi alat.

| Rail / beban | Isi | Diam (mA) | Puncak (mA) | Sumber |
|---|---|---|---|---|
| A1: +3V3 | LED daya D2 + R2 220 Ω | 6,7 | 6,7 | terukur (puncak sama: D2 langsung dari 3V3) |
| A2: +3V3 | HX711 + eksitasi load cell, sisi LV level shifter | 10,4 | 10,4 | terukur |
| B1: +5V (VIN) | LCD 1602 + backpack PCF8574 (lampu latar) | 26,4 | 27,0 | terukur (puncak berkedip 26,0-27,0: pembaruan layar saat menggenggam) |
| B2: +5V (VIN) | sisi HV level shifter (pull-up 10 kΩ) | < 0,01 | 0,1-0,2 | terukur, rentang 2 mA (arus hanya saat jalur I2C rendah) |
| **Beban luar total** | A1 + A2 + B1 + B2 | **43,5** | **44,3** | terukur |
| Papan DevKit sendiri | ESP32 tanpa radio, CP2102, regulator 3,3 V, LED daya papan; D1 (≈ 6 mA) saat puncak | ≈ 55-105 | ≈ 61-111 | **estimasi**: ESP32 30-68 mA (datasheet ESP32 series, Modem-sleep 240 MHz dual-core, radio mati); CP2102 dan regulator = nilai tipikal, datasheet keduanya tidak ada di repo. Tidak dapat diukur tanpa memutus jalur 5 V USB |
| **Total dari USB** | terukur + estimasi | **≈ 100-150** (tipikal ≈ 115-120) | **≈ 105-155** | |
| S3 (proposal) | | rata-rata ≤ 150 | puncak ≤ 600 | memenuhi |
| Estimasi di atas | | 117 | 540 | puncak 540 mA berasal dari lonjakan Wi-Fi TX, yang tidak dipakai firmware |

- Beban luar praktis sama pada keadaan diam dan puncak, karena tidak ada beban di breadboard yang dinyalakan-dimatikan firmware; LED D1 (GPIO27) menarik arus dari 3V3 di dalam DevKit, bukan dari rail breadboard.
- Batas metode: multimeter memperbarui tampilan ≈ 2-3 kali per detik, jadi lonjakan singkat (mis. saat dinyalakan) tidak tertangkap. Tukar probe merah/hitam hanya membalik tanda (−), bukan besarnya.
- **Wiring breadboard yang terlihat saat pengukuran:** beban 3,3 V diberi dua kabel dari pin 3V3 DevKit (rail atas: D2; rail bawah: HX711 dan sisi LV), LCD (VCC backpack) dan sisi HV diberi kabel langsung dari pin VIN. Secara listrik sama dengan net +3V3 dan +5V di skema (tidak ada perbedaan untuk R8). Kapasitor: saat pengukuran keempatnya masih di 3V3; C3 dan C4 dipindah ke +5V sesudahnya (Bagian 6). Kapasitor tidak menarik arus DC, jadi hasil di atas tetap berlaku.

- Decoupling: C1 100 nF keramik + C2 10 µF elektrolit di +3V3 dekat J1 (HX711); C3 10 µF elektrolit + C4 100 nF keramik di +5V dekat J2 (LCD). Elektrolit: + ke rail.

---

## 6. Keputusan catu LCD ✔ (15 Okt. 2026)
Aturan keputusan v1 (ukur dulu, baru putuskan) sudah dijalankan:

| Langkah | Hasil ukur | Keputusan |
|---------|-----------|-----------|
| 1 | Alamat 0x27 (U4). Dengan backpack di VIN: SDA dan SCL diam di **4,93 V** (U5) | Pull-up backpack terhubung ke VCC 5 V |
| 2b | Backpack di 3V3: tulisan hampir tidak tampak walau trimpot kontras diputar ±135° searah jarum jam (mentok) | LCD HS1602A butuh 4,9-5,1 V (DS-LCD1); 3,3 V tidak cukup |
| 2a (dipilih) | - | **LCD dan backpack dari VIN (5 V), SDA/SCL lewat level shifter dua arah 4 kanal (BSS138)**. Dua kanal dipakai (HV1/LV1 = SDA, HV2/LV2 = SCL); kanal 3-4 kosong |

> **Risiko kerusakan:** 4,93 V langsung ke GPIO 21/22 melebihi batas mutlak ESP32 (sekitar 3,6 V). Pada PCB, SDA/SCL ESP32 hanya tersambung ke sisi LV.

**Hitungan pull-up I2C ✔ (17 Okt. 2026, terukur dengan multimeter, catu mati).** Shifter membagi bus menjadi dua segmen:

| Segmen | Pull-up | Gabungan | Batas 1-11,8 kΩ |
|---|---|---|---|
| LV: GPIO21/22 ↔ LV1/LV2 (3,3 V) | Shifter 9,86 kΩ (+ internal ESP32 ≈ 45 kΩ bila aktif) | 9,86 kΩ (≈ 8,1 kΩ) | ✔ |
| HV: HV1/HV2 ↔ backpack (5 V) | Shifter 9,90 kΩ ‖ backpack 4,67 kΩ | 3,17 kΩ (≈ 1,6 mA pada 4,93 V) | ✔ |

Semua pull-up milik modul; **PCB tanpa pull-up tambahan**. Rincian di DS-LLS1 dan DS-I2C1. Shifter yang sudah disolder: LV1/LV2 = 3,26 V, HV1/HV2 = 4,98 V saat dicatu tanpa LCD. **Uji breadboard 17 Okt. 2026 dengan wiring sesuai skema (termasuk D2/R2): LCD bekerja lewat level shifter dan menampilkan data HX711; D2 menyala dengan R2 220 ohm, terang cukup. Tidak ada perbedaan breadboard dan skema.** Koreksi 24 Okt. 2026: kapasitor berbeda. Di breadboard keempatnya ada di +3V3, padahal C3 dan C4 di skema ada di +5V. Sudah dipindah sesuai skema dan dicek (kaki di 3V3: 3,25 V, kaki di VIN: 4,95 V); gambar wiring di Bagian 2.1 [protokol-arus-rail-hgd.md](../pengukuran/protokol-arus-rail-hgd.md).

Alternatif yang dibahas dan tidak dipakai: penggeser level dari MOSFET 2N7000 THT (V_GS(th) 0,8-3 V, R_DS(on) hanya dijamin pada 4,5 V; datasheet Microchip disimpan di Project), atau melepas pull-up backpack. Modul BSS138 dipilih karena sudah jadi, murah, dan dua arah.

---

## 7. Wiring antarmodul
Semua modul dicolok ke **soket betina** di sisi bawah PCB (Bagian 8).

| Soket | Pin | Ke | Sinyal |
|-------|-----|----|--------|
| J1 (1x4) | 1 GND, 2 DT, 3 SCK, 4 VCC | Modul HX711, **dicolok langsung** (tanpa kabel) | GPIO18, GPIO19, +3V3 |
| J2 (1x4) | 1 GND, 2 VCC, 3 SDA, 4 SCL | Backpack LCD lewat kabel jumper F-M | +5V, SDA/SCL sisi 5 V |
| J3 (1x6, LV) | 1 LV1, 2 LV2, 3 LV, 4 GND, 5 LV3, 6 LV4 | Level shifter sisi LV | SDA (GPIO21), SCL (GPIO22), +3V3 |
| J4 (1x6, HV) | 1 HV1, 2 HV2, 3 HV, 4 GND, 5 HV3, 6 HV4 | Level shifter sisi HV | SDA ke J2.3, SCL ke J2.4, +5V |
| Load cell | Terminal HX711 (E+, E−, A−, A+) | Kabel 4 inti load cell | Dipetakan sesuai Bagian 4 |

- **HX711 langsung di J1:** menghilangkan pembacaan bergeser bit akibat jumper DT/SCK longgar yang merusak percobaan E1 pertama.
- **Urutan pin J1** mengikuti urutan modul (GND, DT, SCK, VCC); cocokkan dengan silkscreen XFW-HX711 sebelum gerber.
- **Soket tidak berkunci:** modul shifter yang terpasang terbalik 180° memberi 5 V ke sisi LV (dan ke GPIO). Tandai pin 1 dan sisi HV/LV di silkscreen sisi bawah (B.Silkscreen).
- LCD berada di tutup housing, jadi tetap memakai kabel (panjang ke PCB minimum 90 mm; lihat [desain-housing-elektronik.md](../housing-elektronik/desain-housing-elektronik.md)).

---

## 8. PCB `hgd-v1` (KiCad) ✔ v1 selesai, belum disetujui

**Status v1 (17 Okt. 2026; versi akhir 26 Okt. 2026).** Skema dan PCB versi 1 selesai dan diverifikasi; menunggu persetujuan sebelum Gerber diunggah dan dipesan. Perubahan 26 Okt. 2026: judul skema dan papan menjadi "Digital Hand Grip Dynamometer" (skema hanya berubah di title block; rangkaian dan netlist sama), jalur sinyal digambar ulang dengan posisi semua footprint tetap (segmen jalur 200 → 186, via 59 → 58: 56 GND + 2 sinyal), teks silkscreen tiga baris, stackup papan diisi (FR4 1,6 mm, inti 1,51 mm, tembaga 35 µm, solder mask 10 µm).

| Bukti | Hasil |
|---|---|
| ERC skema | 0 error, 0 warning; 7 `lib_symbol_mismatch` dikecualikan dengan alasan (26 Okt. 2026; [gambar/erc-skema-hgd-v1.png](gambar/erc-skema-hgd-v1.png)) |
| DRC papan | 0 error, 0 warning, 0 tak tersambung, 0 beda skema; 3 pengecualian beralasan: footprint U2, C2, C3 berbeda dari salinan pustaka (footprint sudah disunting di papan). 5 uji diabaikan: courtyard footprint, ujung jalur tidak di tengah via, geometri profil tuning, filter footprint simbol, tipe komponen footprint (26 Okt. 2026; [gambar/drc-pcb-hgd-v1.png](gambar/drc-pcb-hgd-v1.png), [gambar/drc-uji-diabaikan-hgd-v1.png](gambar/drc-uji-diabaikan-hgd-v1.png)) |
| Breadboard = skema | Wiring sesuai skema termasuk D2/R2; LCD bekerja lewat level shifter (17 Okt. 2026). Kapasitor C3/C4 dipindah ke +5V sesuai skema (24 Okt. 2026); gambar wiring di protokol arus rail Bagian 2.1 |
| Gerber + bor | Diekspor ulang 26 Okt. 2026 20:35 (KiCad 10.0.6), Revision 1, 12 berkas dalam `Gerber_Kelompok-01_hgd-v1.zip`: 2 lapis, 1,6 mm, 80 x 55 mm (job file: 80,05 x 55,05 termasuk tebal garis outline); aturan desain di job file: jalur minimum 0,3 mm, jarak pad/jalur 0,2 mm, jarak ke pour 0,5 mm. Dicek di viewer dan analisis DFM JLCDFM. Belum di repo sampai disetujui |
| Render | Tampak atas dan bawah, papan saja ([atas](gambar/pcb-atas-hgd-v1.png), [bawah](gambar/pcb-bawah-hgd-v1.png)) dan model 3D ([atas](gambar/pcb-3d-atas-hgd-v1.png), [bawah](gambar/pcb-3d-bawah-hgd-v1.png)) |
| Berkas | [kicad/proyek/](kicad/proyek/): proyek (`.kicad_pro`), skema, papan, STEP rakitan (26 Okt. 2026, dari papan utama; tanpa soket 1x15 DevKit, tidak memengaruhi desain housing); PDF skema [kicad/pdf/skema-hgd-v1.pdf](kicad/pdf/skema-hgd-v1.pdf); langkah kerja di [log-kerja-kicad-hgd.md](log-kerja-kicad-hgd.md) |
| Belum | Persetujuan; unggah Gerber; BOM |

### 8.1 Keputusan desain
| Hal | Keputusan | Alasan |
|---|---|---|
| DevKit (U2) | Footprint "ESP32-DEVKIT-V1" (SnapEDA, oleh "Do it"): pitch 2,54 mm, jarak baris 25,4 mm, bor 1,02 mm, pad 1 = 3V3, pad 16 = EN. Dicolok ke dua soket betina 1x15, **di sisi bawah PCB** | Sengaja di bawah: bila di atas, tinggi DevKit + soket membuat tombol tidak cukup menonjol keluar tutup |
| Lubang NPTH footprint U2 | Empat lubang 3 mm bawaan footprint **dihapus** | Tidak dipakai |
| J1-J4 | Soket betina (1x4, 1x4, 1x6, 1x6), sisi bawah | Modul dicolok; bagian yang aus murah |
| C1-C4 | Sisi bawah, dekat soket bebannya | Loop decoupling kecil |
| Sisi atas | SW1, R1, D1, R2, D2 | Komponen yang harus menembus atau terlihat dari tutup |
| D2, R2 | LED daya 3 mm `LED_THT:LED_D3.0mm` + R2 220 ohm (footprint sama dengan R1), dari +3V3 ke GND | Indikator daya yang menyala tanpa firmware (Modul 5 Task 2.5); 3V3 dipakai supaya arus sama dengan D1 |
| SW1 | `Button_Switch_THT:SW_PUSH_6mm_H13mm` | Batang 13 mm mencapai tutup |
| R1 | **220 ohm**, `R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal` | ≈ 6 mA pada Vf 2,0 V ((3,3 − 2,0) / 220); terukur 25 Okt. 2026: **4,9 mA** (GPIO27 3,07 V, Vf D1 1,92 V, R1 213 Ω; [hasil-multimeter-hgd.md](../pengukuran/hasil-multimeter-hgd.md)) |
| Kapasitor | C1, C4: `C_Disc_D7.0mm_W2.5mm_P5.00mm`; C2, C3: `CP_Radial_D5.0mm_P2.50mm` | Komponen lab |
| Lapisan | Dua lapis; **pour GND di F.Cu dan B.Cu** (clearance 0,5 mm, lebar minimum 0,25 mm, thermal relief 0,5/0,5 mm, pulau dibuang), dijahit **56 via GND** 0,6/0,3 mm (57 pada 17 Okt. 2026; 56 setelah jalur ulang 26 Okt. 2026) | Jalur balik GND lebar dan pendek (noise dan jatuh tegangan kecil, penting untuk sinyal mikrovolt HX711); perisai di bawah jalur sinyal; thermal relief supaya kaki THT tetap mudah disolder tangan. Area antena bebas pour di kedua lapis |
| Antena | Area antena modul bebas tembaga di kedua lapisan; zona 15 mm di samping dan depan antena bebas tembaga, komponen, dan via | Lihat Bagian 8.4 |

Kredit footprint U2: SnapEDA, "ESP32-DEVKIT-V1" oleh "Do it" (diunduh dari snapeda.com). Diperiksa terhadap papan: pitch, jarak baris, dan posisi pad 1/16 cocok.

### 8.1a Penetapan footprint (Assign Footprints)

Hasil Tools > Assign Footprints di skema `hgd-v1`: 14 simbol, semuanya punya footprint (ditetapkan 17 Okt. 2026, ditangkap ulang 26 Okt. 2026: [gambar/assign-footprint-hgd-v1.png](gambar/assign-footprint-hgd-v1.png)). Semua footprint dari pustaka standar KiCad, kecuali U2 (SnapEDA, Bagian 8.1).

| # | Ref | Nilai | Footprint |
|---|---|---|---|
| 1 | C1 | 100 nF | `Capacitor_THT:C_Disc_D7.0mm_W2.5mm_P5.00mm` |
| 2 | C2 | 10 µF | `Capacitor_THT:CP_Radial_D5.0mm_P2.50mm` |
| 3 | C3 | 10 µF | `Capacitor_THT:CP_Radial_D5.0mm_P2.50mm` |
| 4 | C4 | 100 nF | `Capacitor_THT:C_Disc_D7.0mm_W2.5mm_P5.00mm` |
| 5 | D1 | RED LED | `LED_THT:LED_D5.0mm` |
| 6 | D2 | RED LED | `LED_THT:LED_D3.0mm` |
| 7 | J1 | HX711 01x04_Socket | `Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical` |
| 8 | J2 | LCD I2C 01x04_Socket | `Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical` |
| 9 | J3 | LV Conn_01x06_Socket | `Connector_PinSocket_2.54mm:PinSocket_1x06_P2.54mm_Vertical` |
| 10 | J4 | HV Conn_01x06_Socket | `Connector_PinSocket_2.54mm:PinSocket_1x06_P2.54mm_Vertical` |
| 11 | R1 | 220 Ω | `Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal` |
| 12 | R2 | 220 Ω | `Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal` |
| 13 | SW1 | SW_Push | `Button_Switch_THT:SW_PUSH_6mm_H13mm` |
| 14 | U2 | ESP32-DEVKIT-V1 | `ESP32-DEVKIT-V1:MODULE_ESP32_DEVKIT_V1` |

Yang tidak ada di daftar ini (tidak punya simbol di skema) dan harus ditambahkan manual ke BOM: dua soket betina 1x15 untuk DevKit, serta modul yang dicolok atau disambung kabel (HX711, LCD + backpack, level shifter, load cell).

### 8.2 Netlist (dari skema KiCad, diperiksa 15 Okt. 2026)
| Net (nama KiCad) | Fungsi | Pin |
|---|---|---|
| +3V3 (`Net-(J1-VCC)`) | Rail 3,3 V | U2.1, J1.4, J3.3 (LV), C1.1, C2.+, R2.1 |
| +5V (`Net-(J2-VCC)`) | Rail 5 V dari VIN | U2.30, J2.2, J4.3 (HV), C3.+, C4.1 |
| GND | Ground bersama | U2.2, U2.29, J1.1, J2.1, J3.4, J4.4, SW1.2, D1.K, D2.K, C1.2, C2.−, C3.−, C4.2 |
| HX_DT (`Net-(J1-DT)`) | GPIO18 | U2.9, J1.2 |
| HX_SCK (`Net-(J1-SCK)`) | GPIO19 | U2.10, J1.3 |
| SDA_3V3 (`Net-(J3-LV1)`) | GPIO21 | U2.11, J3.1 |
| SCL_3V3 (`Net-(J3-LV2)`) | GPIO22 | U2.14, J3.2 |
| SDA_5V (`Net-(J2-SDA)`) | SDA LCD | J4.1 (HV1), J2.3 |
| SCL_5V (`Net-(J2-SCL)`) | SCL LCD | J4.2 (HV2), J2.4 |
| BTN (`Net-(U2-D25)`) | Tombol | U2.23, SW1.1 |
| LED_DRV (`Net-(U2-D27)`) | GPIO27 ke resistor | U2.25, R1.1 |
| LED_A (`Net-(D1-A)`) | Resistor ke anoda | R1.2, D1.A |
| PWR_LED_A (`Net-(D2-A)`) | Resistor ke anoda LED daya | R2.2, D2.A |
| (tidak dipakai) | Kanal 3-4 shifter; pin DevKit lain | J3.5, J3.6, J4.5, J4.6; pin U2 lain diberi tanda no-connect |

ERC 17 Okt. 2026: **0 error, 0 warning**. Pin tak terpakai diberi tanda no-connect, `PWR_FLAG` di GND (daya masuk lewat USB DevKit, yang tidak ada di skema). Tujuh peringatan `lib_symbol_mismatch` dikecualikan dengan sengaja: simbol J1-J4, C1, C4, SW1 sudah diubah (nama pin), dan memperbarui dari pustaka akan menghapus nama pin itu.

### 8.3 Pembersihan berkas `hgd-v1.kicad_pcb` sebelum gerber ✔
- ✔ Dua footprint `REF**` PinSocket_1x15 di pad U2 dihapus dari berkas papan (hanya ada di salinan 3D `hgd-v1-verbatim.kicad_pcb`, yang dihapus 27 Okt. 2026).
- ✔ Empat lubang NPTH 3 mm milik footprint U2 dihapus.
- ✔ Edge.Cuts final 80 x 55 mm (109,75; 93) sampai (189,75; 148); lubang H1-H4 di sudut.

### 8.4 Usulan penempatan v2
**Catatan 26 Okt. 2026:** gambar ini belum diperbarui; jalur dan teks silkscreen di dalamnya masih versi 17 Okt. (posisi komponen sama). Papan terbaru: render di Bagian 8 (baris Render). Gambar [gambar/penempatan-pcb-hgd.svg](gambar/penempatan-pcb-hgd.svg) menunjukkan PCB `hgd-v1` v1 final dari berkas KiCad 17 Okt. 2026 (diperbarui: D2/R2, pour GND kedua lapis, 59 via, silkscreen tim): 80 x 55 mm, lubang H1-H4 Ø3,0 pola 75 x 50 mm, ERC dan DRC 0 error, belum disetujui dan belum dipesan. Teks di gambar diambil dari silkscreen berkas papan (isi, posisi, ukuran, perataan); silkscreen bawah digambar tidak dicerminkan. Gerber dan berkas bor diekspor dan dicek 17 Okt. 2026 (outline 80 x 55, 68 PTH termasuk 2 via, 4 NPTH Ø3,0); belum di repo sampai papan disetujui. **Setelah itu D2 dan R2 ditambahkan** (17 Okt. 2026): D2 K (135,5; 145), A (132,96; 145); R2 pad (119,61; 145) dan (129,77; 145); +3V3 lewat jalur 0,5 mm di x 118,34; teks silkscreen `PWR`. DRC 0 error. Berkas papan [kicad/proyek/hgd-v1.kicad_pcb](kicad/proyek/hgd-v1.kicad_pcb) sudah versi terbaru (17 Okt. 2026; model 3D DevKit tertanam, tanpa jalur lokal). Skema di repo juga terbaru (17 Okt. 2026). **Gerber dan bor diekspor ulang 17 Okt. 2026 19:44** (setelan sama; Revision 1 di job file) dan dicek: outline 80 x 55 mm; 129 PTH (70 kaki komponen termasuk D2/R2, 59 via: 57 GND + 2 sinyal), 4 NPTH Ø3,0; pour GND di kedua lapis, area antena kosong; silkscreen tim ada; tanpa jalur lokal. Dicek di viewer JLCPCB: terdeteksi 2 lapis, 80 x 55 mm, pratinjau sama dengan papan. Belum di repo sampai disetujui.

**Perangkat lunak (17 Okt. 2026):** pemakaian KiCad sebagai pengganti EasyEDA sudah ditanyakan ke tim pengajar dan diizinkan.

**Lubang mounting (keputusan 17 Okt. 2026):** H1-H4 tetap Ø3,0 dengan pusat 2,5 mm dari tepi, pola 75 x 50 mm, sampai ada arahan lain. Alasan: mengikuti pola lubang yang sama dengan PCB LCD dan DevKit, jadi ketiganya memakai ukuran dan jarak lubang yang seragam. Berbeda dari arahan modul (M3 3,2 mm, ≥ 5 mm dari tepi); sisa papan antara lubang dan tepi 1,0 mm, jadi baut jangan dikencangkan berlebihan. Ukuran baut PCB belum dipastikan: Ø3,0 cukup untuk M2,5, terlalu sempit untuk M3.

**Silkscreen identitas (17 Okt. 2026, diubah 26 Okt. 2026):** teks F.Silkscreen di bawah SW1, bebas pad; DRC bersih. Sejak 26 Okt. 2026 tiga baris: "Kelompok 01 / Digital Hand Grip / Dynamometer" (sebelumnya dua baris "Kelompok 01 / Hand Grip Dynamometer").

**Pour GND (17 Okt. 2026):** F.Cu 2852 mm² (3 bagian), B.Cu 2610 mm² (5 bagian, terpotong jalur); setiap bagian di kedua lapis kena minimal satu via jahit; tidak ada tembaga di area antena. Pada 17 Okt. 2026 satu error DRC dikecualikan: pad 2 U2 (GND) hanya mendapat 1 spoke thermal di B.Cu karena jalur 3V3 lewat 0,55 mm di atasnya serta pad 1 dan 3 di kiri-kanan; pad itu tetap tersambung lewat jalur GND F.Cu ke pad 29 dan pour F.Cu. Setelah jalur ulang 26 Okt. 2026 pengecualian ini tidak ada lagi di daftar DRC. Skema ada di [kicad/proyek/hgd-v1.kicad_sch](kicad/proyek/hgd-v1.kicad_sch). Tabel dan batasan di bawah adalah usulan penempatan v2 yang lama; posisi final ada di gambar. Koordinat usulan v2: KiCad (mm, Y ke bawah), relatif terhadap U2 di (131,73; 109,475; rotasi 180; B.Cu). Diturunkan dari tata letak di Fusion (±1 mm). Arah: +Y menuju antena; +X menuju tepi PCB LCD di tutup.

| Ref | Sisi | Posisi | Catatan |
|---|---|---|---|
| J1 HX711 | B | pin 1 (158,5; 86,0), pin ke +Y | Modul 24 x 15,6 menggantung ke +X (di bawah area LCD), sudut terjauh dari antena |
| C2 10 µF | B | pusat (150,8; 88,7) | +3V3 |
| C1 100 nF | B | pusat (150,8; 94,4) | +3V3 |
| J2 LCD | B | pin 1 (159,0; 142,0), pin ke +Y | Di luar zona antena |
| C3 10 µF | B | pusat (164,0; 143,3) | +5V; di sisi LCD dari J2 (posisi di bawah J2 masuk zona antena) |
| C4 100 nF | B | pusat (165,0; 148,6) | +5V |
| J4 HV | B | pin 1 (114,0; 96,0), pin ke +Y | Sisi dalam, dekat pin 30 (5 V) |
| J3 LV | B | pin 1 (103,84; 96,0), pin ke +Y | Jarak J3-J4 10,16 mm, terukur pada modul (jangka sorong) |
| SW1 | A | pusat (158,5; 105,8); pad (156,25/160,75; 102,55/109,05) | Urutan sisi atas searah U2: SW1 → R1 → D1 |
| R1 | A | pad (158,5; 117,1) dan (158,5; 127,26), vertikal | Pad 1 ke GPIO27 |
| D1 | A | K (157,23; 134,5), A (159,77; 134,5) | - |

Batasan penempatan:
1. **Tepi PCB LCD (tutup):** komponen sisi atas harus di luar jejak PCB LCD agar menembus tutup. Courtyard SW1 sampai x 162,25 masih ±3,75 mm dari tepi LCD (±x 166 menurut Fusion).
2. **Zona antena:** x 107,95-155,64, y 128,46-149,95 (15 mm di samping dan depan antena; angka 15 mm dari ringkasan pedoman Espressif oleh NextPCB, belum dicocokkan ke dokumen Espressif). Jalur SDA/SCL, +5V, tombol, dan LED mengitari zona ini.
3. **Modul yang menggantung:** DevKit, HX711, dan shifter sama-sama ±8,5 mm di bawah PCB; badannya tidak boleh saling menimpa (badan shifter x ±102,2-115,6; tepi DevKit x 117,5).
4. **Kaki THT:** komponen sisi bawah disolder dari sisi atas, jadi sisi atas di atas J1-J4 dan C1-C4 tidak kosong. Kaki SW1/R1/D1 tidak boleh jatuh di badan soket.
5. **Lubang M3:** satu di dekat SW1 supaya PCB tidak melentur saat tombol ditekan.

**Aturan desain PCB:**
1. Bagian yang cepat aus berada di bagian murah: soket di PCB, modul dicolok.
2. Jalur sinyal pendek; kapasitor 100 nF paling dekat ke beban, 10 µF sebagai bulk.
3. Antena modul bebas tembaga dan jauh dari kabel sinyal HX711 dan level shifter.
4. Elektrolit berpolaritas (+ ke rail); keramik tidak.
5. Pin 1 dan sisi HV/LV ditandai di B.Silkscreen.
6. Tanpa proteksi polaritas, sekering, atau ESD; cukup untuk perangkat berdaya USB.

---

## 9. PEMERIKSAAN KOMPONEN

### 9.1 Pemeriksaan load cell (CZL 601 80 kg, 10 Okt. 2026)

| Pemeriksaan | Hasil | Arti |
|---|---|---|
| Dimensi | 130 x 28 x 21,5 mm | Cocok dengan Eagle Weigh (130 x 28 x 22); lebar 28 mm, bukan 30 mm; tinggi 0,5 mm kurang dari nilai gambar |
| Kabel | 4 kabel: merah, hitam, putih, hijau | Sama jumlahnya dengan datasheet; warnanya dipetakan ulang |
| Resistansi (multimeter DT9205A, rentang 2k, hasil tampil dalam kohm: 0,306 = 306 ohm) | Hitam-putih 393 ohm; merah-hijau 347 ohm; empat pasangan silang 260-306 ohm. Diulang dua kali, nilai sama | Pasangan masukan (eksitasi): datasheet 401 +-10 ohm, terukur 393, di dalam rentang. Pasangan keluaran (sinyal): datasheet 350 +-5 ohm, terukur 347, di dalam rentang. Pasangan silang memberi angka antara keduanya, sesuai jembatan penuh |
| Pemetaan warna | **Eksitasi = hitam dan putih; sinyal = merah dan hijau** | Bertentangan dengan datasheet (merah/hitam eksitasi, hijau/putih sinyal) dan listing penjual. Jangan menyambung berdasarkan warna datasheet |
| Uji bertegangan | ✔ 15 Okt. 2026 lewat HX711 | Modul merespons; E1 lolos (Bagian 4) |

Penyimpanan: kotak kedap udara dengan gelembung plastik (sisi gelembung menghadap sel), paket silika gel di luar bungkus gelembung, di laci bawah lemari; jauhkan dari kulkas (kondensasi dan getaran); jangan ada beban di atasnya dan jangan dibawa lewat kabel. Ujung kabel telanjang ditutup (isolasi PVC atau selongsong susut; selotip kertas hanya untuk label), dan tiap kabel diberi label peran dan hasil ukur. Biarkan kotak mencapai suhu ruang 10-15 menit sebelum dibuka.

### 9.2 Uji bench modul (15 Okt. 2026)

| Uji | Hasil |
|---|---|
| U4 pemindai I2C | ✔ 0x27 |
| U5 tegangan SDA/SCL backpack (di VIN) | 4,93 V → level shifter wajib (Bagian 6) |
| U7 HX711 | ✔ DT turun, modul merespons |
| U8 tombol dan LED | ◐ 3 TAP dan 1 TAHAN terklasifikasi benar (uji penuh 10/10 belum) |
| E1 noise 60 s | ✔ σ 38,2 counts (0,0007 kg); `THRESHOLD_ONSET_KG` = 1,0 kg |

### 9.3 Kode uji

- [esp32_uji_awal.ino](../firmware/sketches/esp32_uji_awal/esp32_uji_awal.ino): uji loopback pin proyek (DS-ESP1).
- [uji_lcd_i2c](../firmware/sketches/uji_lcd_i2c/uji_lcd_i2c.ino), [uji_hx711](../firmware/sketches/uji_hx711/uji_hx711.ino), [uji_tombol_led](../firmware/sketches/uji_tombol_led/uji_tombol_led.ino): uji per modul (U4, U7, U8).
- [uji_bench_hgd](../firmware/sketches/uji_bench_hgd/uji_bench_hgd.ino): satu sketsa breadboard untuk U4, U7, U8, dan E1 sekaligus; dipakai pada uji 15 Okt. 2026.

---

## 10. Sisa pekerjaan

1. ✔ Jarak baris header level shifter terukur 10,16 mm; J3-J4 memakai jarak ini.
2. Ukur jarak header HX711 ke tepi modul (asumsi 1,5 mm) dan cocokkan urutan pin J1 dengan silkscreen modul.
3. ✔ Berkas KiCad dibersihkan (Bagian 8.3).
4. ✔ Edge.Cuts 80 x 55 mm; lubang H1-H4 di sudut, pola sama dengan LCD dan DevKit (keputusan 17 Okt. 2026; tidak ada M3 dekat SW1).
5. ✔ Jalur dan DRC: dua lapis, sinyal 0,3 mm, rail 0,5 mm; pour GND kedua lapis dan 57 via jahit; jalur mengitari zona antena.
6. ✔ Penanda HV/LV dan label soket di B.Silkscreen; nomor tim dan nama proyek di F.Silkscreen.
7. Uji U8 penuh (10 tap, 10 tahan). U6 (E+, E− modul HX711) ✔ 24 Okt. 2026.
8. Persetujuan PCB v1, lalu unggah ZIP Gerber (batas 27 Okt. 2026, 23:59). Gerber diekspor ulang dan dicek di JLCDFM 26 Okt. 2026.
9. ◐ PDF skema ✔ 26 Okt. 2026 ([kicad/pdf/skema-hgd-v1.pdf](kicad/pdf/skema-hgd-v1.pdf)); BOM belum (kolom sesuai modul; tambahkan 2 soket 1x15 DevKit dan modul yang dicolok, Bagian 8.1a).
10. Gambar [gambar/penempatan-pcb-hgd.svg](gambar/penempatan-pcb-hgd.svg) diperbarui dari berkas papan 17 Okt. 2026 (D2/R2, pour, via); belum memuat jalur ulang dan silkscreen 26 Okt. 2026.
11. ◐ Bahan laporan: tangkapan ERC, DRC, Assign Footprints, dan render papan/3D ✔ 26 Okt. 2026 (`gambar/`); belum: perhitungan lebar jalur 5V, data papan, review papan tim lain.

---

## 11. Perubahan

| Versi | Tanggal | Perubahan |
|---|---|---|
| v8 | 27 Okt. 2026 | Versi akhir papan 26 Okt. 2026: judul "Digital Hand Grip Dynamometer", jalur ulang (posisi footprint tetap; 56 via GND), silkscreen tiga baris, stackup; ERC dan DRC ulang (DRC 3 pengecualian, spoke thermal U2 tidak lagi); Gerber diekspor ulang dan dicek di JLCDFM; Bagian 8.1a daftar Assign Footprints; PDF skema, `.kicad_pro`, tangkapan dan render di repo; catatan bahwa gambar penempatan masih versi 17 Okt.; STEP diganti versi 26 Okt. (tanpa soket DevKit) dan salinan verbatim dihapus |
| v7 | 25 Okt. 2026 | Arus D1 terukur (4,9 mA) di tabel komponen; U6 selesai; tautan ke rangkuman pembacaan multimeter |
| v6 | 24 Okt. 2026 | Breadboard: C3 dan C4 dipindah dari +3V3 ke +5V sesuai skema (Bagian 5.1, Bagian 6, tabel Bagian 8), dengan hasil cek; gambar wiring di protokol arus rail |
| v5 | 24 Okt. 2026 | Bagian 5.1: arus tiap rail terukur (uji S3), diam dan puncak, dengan estimasi papan DevKit dan perbandingan terhadap S3 |
| v4 | 17 Okt. 2026 | **Skema dan PCB v1 selesai (belum disetujui)**; gambar penempatan diperbarui dari papan final (D2/R2, pour GND, via); Gerber diekspor ulang dan dicek di viewer JLCPCB; berkas KiCad, salinan 3D, dan STEP di repo; log kerja KiCad; pembersihan berkas dan sisa pekerjaan diperbarui; pour GND F.Cu dan B.Cu + 57 via jahit; nomor tim dan nama proyek di silkscreen; lubang mounting dipertahankan; KiCad diizinkan sebagai pengganti EasyEDA; LED daya D2 (3 mm) + R2 220 ohm dari +3V3; netlist dan keputusan desain diperbarui; ERC 0 error (no-connect, `PWR_FLAG`); hitungan pull-up I2C terukur (LV 9,86 kΩ, HV 3,17 kΩ gabungan): tanpa pull-up tambahan; tegangan shifter tersolder 3,26 V / 4,98 V |
| v3 | 16-17 Okt. 2026 | 17 Okt.: teks gambar dari silkscreen berkas papan; Gerber diekspor dan dicek (belum di repo). 16 Okt.: gambar `penempatan-pcb-hgd.svg` diganti dengan PCB v1 berjalur final dari berkas KiCad (lolos DRC, belum disetujui); skema `hgd-v1.kicad_sch` ditambahkan di `kicad/proyek/` |
| v2 | 16 Okt. 2026 | R1 100 → 220 ohm; catu LCD diputuskan (5 V + level shifter BSS138 4 kanal); rail +5V; tanpa baterai; DevKit dan semua soket di sisi bawah, modul dicolok ke soket (HX711 langsung); netlist KiCad `hgd-v1`; usulan penempatan v2 dan gambar; hasil uji bench 15 Okt. 2026 |
| v1 | 10-14 Okt. 2026 | Draf PCB v1 take-home: soket DevKit J1/J2, header jantan J3/J4 dengan kabel jumper, rail 3,3 V saja |

---

*Catatan kerja proyek akhir mata kuliah embedded system. Bukan dokumen yang dinilai.*

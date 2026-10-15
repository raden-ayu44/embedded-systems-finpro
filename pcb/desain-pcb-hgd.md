---
title: Desain PCB dan Skema - Hand Grip Dynamometer

---

# Desain PCB dan Skema - Hand Grip Dynamometer (v2)
**Proyek akhir - embedded system**

> Skema dan PCB (skema termasuk): komponen, alokasi pin, rantai sinyal, load cell, catu daya, keputusan catu LCD, wiring antarmodul, PCB `hgd-v1` (KiCad), netlist, usulan penempatan, pemeriksaan load cell, dan sisa pekerjaan. Berkas netlist dan daftar komponen: `pcb_netlist.xlsx`; berkas KiCad: [kicad/](kicad/README.md); gambar penempatan: [gambar/penempatan-pcb-hgd.svg](gambar/penempatan-pcb-hgd.svg). Firmware ada di [alur-program-hgd.md](../firmware/alur-program-hgd.md); induk: [catatan-proyek-hgd.md](../catatan-proyek-hgd.md).

---

## 1. Komponen
| Komponen | Tipe | Peran | Status |
|----------|------|-------|--------|
| Mikrokontroler | ESP32 DevKit V1 30 pin (chip ESP32-D0WD-V3 rev v3.1, flash 4 MB, tanpa PSRAM) | Firmware, state machine | ✔ ada; loopback pin lolos 8 Okt. 2026 |
| Load cell | CZL 601 80 kg, straight-bar, jembatan penuh, 2,0 ±0,2 mV/V | Sensor gaya | ✔ diterima; 130 x 28 x 21,5 mm; resistansi lolos (10 Okt. 2026) |
| Penguat + ADC | Modul HX711 (papan XFW-HX711), kanal A, gain 128, 10 SPS | Digitalisasi | ✔ ada; merespons dan lolos uji noise E1 (15 Okt. 2026); 24 x 15,6 mm terukur |
| Tampilan | LCD 16x2 HS1602A + backpack I2C (PCF8574T, alamat 0x27) | Teks per state | ✔ ada; alamat 0x27 terbaca (U4) |
| Penggeser level | Modul level shifter dua arah 4 kanal berbasis MOSFET BSS138 | SDA/SCL 5 V (LCD) ↔ 3,3 V (ESP32) | ▣ dibeli, belum tiba; jarak baris header diukur saat tiba |
| Indikator | LED 5 mm + R1 **220 ohm** | Sinyal "sedang direkam" | ▣ dari komponen tim |
| Input | Tactile push button 6 x 6 mm, batang 13 mm, 1 buah | START, NEXT, RESTART | ▣ dari komponen tim |
| PCB | `hgd-v1` (KiCad), dua lapis | Dudukan DevKit, modul, tombol, LED | ◐ skema dan netlist benar; penempatan usulan v2 (Bagian 8.4) |
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
- Estimasi arus: tipikal ≈ 117 mA, puncak ≈ 540 mA (didominasi lonjakan WiFi TX, yang tidak aktif pada jalur logging inti); margin terhadap regulator 1 A. LED kini ≈ 6 mA (R1 220 ohm), bukan 15 mA. Diverifikasi di uji S3.
- Eksitasi load cell diturunkan dari catu HX711 (E+ = AVDD). Pengukuran bersifat **rasiometrik**, jadi jumlah counts per kg praktis sama pada 5 V dan 3,3 V; yang berubah adalah noise relatif. Pada 3,3 V noise terukur σ ≈ 0,7 g (E1), jauh di bawah kebutuhan.
- Decoupling: C1 100 nF keramik + C2 10 µF elektrolit di +3V3 dekat J1 (HX711); C3 10 µF elektrolit + C4 100 nF keramik di +5V dekat J2 (LCD). Elektrolit: + ke rail.

---

## 6. Keputusan catu LCD ✔ (15 Okt. 2026)
Aturan keputusan v1 (ukur dulu, baru putuskan) sudah dijalankan:

| Langkah | Hasil ukur | Keputusan |
|---------|-----------|-----------|
| 1 | Alamat 0x27 (U4). Dengan backpack di VIN: SDA dan SCL diam di **4,93 V** (U5) | Pull-up backpack terhubung ke VCC 5 V |
| 2b | Backpack di 3V3: tulisan hampir tidak tampak walau trimpot kontras diputar ±135° searah jarum jam (mentok) | LCD HS1602A butuh 4,9-5,1 V (DS-LCD1); 3,3 V tidak cukup |
| 2a (dipilih) | - | **LCD dan backpack dari VIN (5 V), SDA/SCL lewat level shifter dua arah 4 kanal (BSS138)**. Dua kanal dipakai (HV1/LV1 = SDA, HV2/LV2 = SCL); kanal 3-4 kosong |

> **Risiko kerusakan:** 4,93 V langsung ke GPIO 21/22 melebihi batas mutlak ESP32 (sekitar 3,6 V). Selama level shifter belum tiba, sambungan VIN langsung hanya untuk uji singkat di bench. Pada PCB, SDA/SCL ESP32 hanya tersambung ke sisi LV.

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

## 8. PCB `hgd-v1` (KiCad) ◐

### 8.1 Keputusan desain
| Hal | Keputusan | Alasan |
|---|---|---|
| DevKit (U2) | Footprint "ESP32-DEVKIT-V1" (SnapEDA, oleh "Do it"): pitch 2,54 mm, jarak baris 25,4 mm, bor 1,02 mm, pad 1 = 3V3, pad 16 = EN. Dicolok ke dua soket betina 1x15, **di sisi bawah PCB** | Sengaja di bawah: bila di atas, tinggi DevKit + soket membuat tombol tidak cukup menonjol keluar tutup |
| Lubang NPTH footprint U2 | Empat lubang 3 mm bawaan footprint **dihapus** | Tidak dipakai |
| J1-J4 | Soket betina (1x4, 1x4, 1x6, 1x6), sisi bawah | Modul dicolok; bagian yang aus murah |
| C1-C4 | Sisi bawah, dekat soket bebannya | Loop decoupling kecil |
| Sisi atas | Hanya SW1, R1, D1 | Komponen yang harus menembus atau terlihat dari tutup |
| SW1 | `Button_Switch_THT:SW_PUSH_6mm_H13mm` | Batang 13 mm mencapai tutup |
| R1 | **220 ohm**, `R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal` | ≈ 6 mA pada Vf 2,0 V ((3,3 − 2,0) / 220) |
| Kapasitor | C1, C4: `C_Disc_D7.0mm_W2.5mm_P5.00mm`; C2, C3: `CP_Radial_D5.0mm_P2.50mm` | Komponen lab |
| Lapisan | Dua lapis; GND pour | - |
| Antena | Area antena modul bebas tembaga di kedua lapisan; zona 15 mm di samping dan depan antena bebas tembaga, komponen, dan via | Lihat Bagian 8.4 |

Kredit footprint U2: SnapEDA, "ESP32-DEVKIT-V1" oleh "Do it" (diunduh dari snapeda.com). Diperiksa terhadap papan: pitch, jarak baris, dan posisi pad 1/16 cocok.

### 8.2 Netlist (dari skema KiCad, diperiksa 15 Okt. 2026)
| Net (nama KiCad) | Fungsi | Pin |
|---|---|---|
| +3V3 (`Net-(J1-VCC)`) | Rail 3,3 V | U2.1, J1.4, J3.3 (LV), C1.1, C2.+ |
| +5V (`Net-(J2-VCC)`) | Rail 5 V dari VIN | U2.30, J2.2, J4.3 (HV), C3.+, C4.1 |
| GND | Ground bersama | U2.2, U2.29, J1.1, J2.1, J3.4, J4.4, SW1.2, D1.K, C1.2, C2.−, C3.−, C4.2 |
| HX_DT (`Net-(J1-DT)`) | GPIO18 | U2.9, J1.2 |
| HX_SCK (`Net-(J1-SCK)`) | GPIO19 | U2.10, J1.3 |
| SDA_3V3 (`Net-(J3-LV1)`) | GPIO21 | U2.11, J3.1 |
| SCL_3V3 (`Net-(J3-LV2)`) | GPIO22 | U2.14, J3.2 |
| SDA_5V (`Net-(J2-SDA)`) | SDA LCD | J4.1 (HV1), J2.3 |
| SCL_5V (`Net-(J2-SCL)`) | SCL LCD | J4.2 (HV2), J2.4 |
| BTN (`Net-(U2-D25)`) | Tombol | U2.23, SW1.1 |
| LED_DRV (`Net-(U2-D27)`) | GPIO27 ke resistor | U2.25, R1.1 |
| LED_A (`Net-(D1-A)`) | Resistor ke anoda | R1.2, D1.A |
| (tidak dipakai) | Kanal 3-4 shifter; pin DevKit lain | J3.5, J3.6, J4.5, J4.6 |

### 8.3 Pembersihan berkas `hgd-v1.kicad_pcb` sebelum gerber
- Hapus dua footprint `REF**` PinSocket_1x15 yang menumpuk di pad U2 (lubang ganda).
- Hapus empat lubang NPTH 3 mm milik footprint U2.
- Edge.Cuts sekarang (116,5; 83) sampai (163; 136) adalah sementara; tepi papan dan lubang M3 ditentukan setelah sisi atas selesai.

### 8.4 Usulan penempatan v2
Gambar: [gambar/penempatan-pcb-hgd.svg](gambar/penempatan-pcb-hgd.svg) (tembus pandang dari atas dan tampak bawah). Koordinat KiCad (mm, Y ke bawah), relatif terhadap U2 di (131,73; 109,475; rotasi 180; B.Cu). Diturunkan dari tata letak di Fusion (±1 mm). Arah: +Y menuju antena; +X menuju tepi PCB LCD di tutup.

| Ref | Sisi | Posisi | Catatan |
|---|---|---|---|
| J1 HX711 | B | pin 1 (158,5; 86,0), pin ke +Y | Modul 24 x 15,6 menggantung ke +X (di bawah area LCD), sudut terjauh dari antena |
| C2 10 µF | B | pusat (150,8; 88,7) | +3V3 |
| C1 100 nF | B | pusat (150,8; 94,4) | +3V3 |
| J2 LCD | B | pin 1 (159,0; 142,0), pin ke +Y | Di luar zona antena |
| C3 10 µF | B | pusat (164,0; 143,3) | +5V; di sisi LCD dari J2 (posisi di bawah J2 masuk zona antena) |
| C4 100 nF | B | pusat (165,0; 148,6) | +5V |
| J4 HV | B | pin 1 (114,0; 96,0), pin ke +Y | Sisi dalam, dekat pin 30 (5 V) |
| J3 LV | B | pin 1 (103,84; 96,0), pin ke +Y | Jarak J3-J4 10,16 mm **diukur saat modul tiba** |
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

1. Ukur jarak baris header level shifter saat tiba; tetapkan jarak J3-J4 dari hasil ukur.
2. Ukur jarak header HX711 ke tepi modul (asumsi 1,5 mm) dan cocokkan urutan pin J1 dengan silkscreen modul.
3. Bersihkan berkas KiCad (Bagian 8.3); terapkan penempatan v2.
4. Tentukan Edge.Cuts dan lubang M3 setelah sisi atas selesai (satu M3 dekat SW1).
5. Pemberian jalur dan DRC: dua lapis, sinyal 0,3 mm, rail 0,5 mm, jarak minimum 0,25 mm (usulan; sesuaikan dengan aturan kelas dan pabrik). Jalur mengitari zona antena.
6. Tandai pin 1 dan sisi HV/LV di B.Silkscreen.
7. Uji U8 penuh (10 tap, 10 tahan) dan U6 (E+, E− modul HX711).

---

## 11. Perubahan

| Versi | Tanggal | Perubahan |
|---|---|---|
| v2 | 16 Okt. 2026 | R1 100 → 220 ohm; catu LCD diputuskan (5 V + level shifter BSS138 4 kanal); rail +5V; tanpa baterai; DevKit dan semua soket di sisi bawah, modul dicolok ke soket (HX711 langsung); netlist KiCad `hgd-v1`; usulan penempatan v2 dan gambar; hasil uji bench 15 Okt. 2026 |
| v1 | 10-14 Okt. 2026 | Draf PCB v1 take-home: soket DevKit J1/J2, header jantan J3/J4 dengan kabel jumper, rail 3,3 V saja |

---

*Catatan kerja proyek akhir mata kuliah embedded system. Bukan dokumen yang dinilai.*

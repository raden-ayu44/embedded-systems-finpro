---
title: Desain Housing Elektronik - Hand Grip Dynamometer

---

# Desain Housing Elektronik - Hand Grip Dynamometer (v3)
**Proyek akhir - embedded system**

> Housing elektronik: wadah PCB, ESP32, modul HX711, level shifter, LCD, tombol, dan LED. Housing grip (load cell) ada di [desain-housing-load-cell.md](../housing-load-cell/desain-housing-load-cell.md); ukuran komponen di [tabel-pengukuran-hgd.xlsx](../pengukuran/tabel-pengukuran-hgd.xlsx); PCB di [desain-pcb-hgd.md](../pcb/desain-pcb-hgd.md); parameter Fusion di [fusion/](fusion/); induk: [catatan-proyek-hgd.md](../catatan-proyek-hgd.md).

---

## 1. Susunan ◐
Wadah cetak 3D dua bagian (body dan lid) dimodelkan di Fusion (berkas `electronics_housing_v1`, belum di repo).

| Bagian | Susunan |
|---|---|
| LCD | Dibaut ke **lid** (empat lubang M3 PCB LCD); lid duduk di atas bingkai hitam LCD |
| Jendela lid | Seukuran **area layar** (64,5 x 14,0), bukan bingkai hitam: jendela seukuran bingkai bentrok dengan lubang baut bawah PCB LCD |
| PCB `hgd-v1` | Di bawah LCD, pada ketinggian berbeda dari PCB LCD (kalau sejajar, soket mendorong tombol sehingga tombol tidak menonjol) |
| Tombol dan LED | Di sisi atas PCB, di luar jejak PCB LCD; batang tombol 13 mm dan LED menembus lid |
| DevKit, HX711, level shifter | Menggantung ±8,5 mm di bawah PCB pada soket (sisi bawah) |
| Cutout | Samping untuk kabel USB dan kabel load cell |

Kabel load cell pendek (≈ 28 cm), jadi housing elektronik diletakkan dalam jangkauan kabel. Kabel LCD: dari backpack 52 mm, ke PCB minimum 90 mm, jari-jari belok 5 mm.

### 1.1 Batasan dari penempatan PCB final (16 Okt. 2026)
- **PCB 80 x 55 mm**, empat lubang NPTH Ø3,0 mm, pusat 2,5 mm dari tepi, jarak 75 x 50 mm. Jarak 75 mm segaris dengan lubang PCB LCD (80 x 36, Ø3, 75 x 31), jadi tiang lubang dapat dipakai bersama sesuai model Fusion. Baut M2,5 longgar di Ø3,0; M3 perlu lubang diperbesar (pusat tetap).
- Komponen sisi atas (SW1 → R1 → D1) berada di strip atas papan (y ±96-101 di KiCad) dan harus di luar jejak PCB LCD.
- Ujung USB DevKit di tepi kiri papan: cutout USB di dinding sisi itu.
- Level shifter (J3/J4) berada di luar ujung antena DevKit (bukan lagi di sisi pin 30 seperti usulan v2), ±13 mm dari ujung modul; HX711 (J1) di sudut atas kanan. Arah gantung HX711 belum pasti (Bagian 8.4 butir 3 di [desain-pcb-hgd.md](../pcb/desain-pcb-hgd.md)): ke dalam papan ia bertumpuk dengan C1/C2 dan level shifter, ke luar ia menjorok ±20 mm melewati tepi PCB sehingga dinding housing harus mengikutinya.
- Lubang terdekat ke tombol ±23 mm: batasan lama "satu tiang M3 dekat tombol" tidak dipenuhi. Beri tumpuan di bawah PCB dekat SW1 bila PCB melentur saat tombol ditekan.
- Bagian logam dan modul dijauhkan dari antena sebisanya; WiFi tidak dipakai pada jalur logging inti, jadi ini kebiasaan baik, bukan syarat.

Rinci di Bagian 8.4 [desain-pcb-hgd.md](../pcb/desain-pcb-hgd.md). Gambar [penempatan-pcb-hgd.svg](../pcb/gambar/penempatan-pcb-hgd.svg) masih menunjukkan usulan v2.

---

## 2. Ukuran LCD (terukur 15 Okt. 2026)
Model LCD disederhanakan dari ukuran ini; nilainya ada di berkas parameter.

| Bagian | Ukuran (mm) |
|---|---|
| PCB LCD | 80 x 36, tebal ≈ 1,5; lubang Ø3, tepi lubang 1 mm dari tepi PCB |
| Bingkai hitam | 71,5 x 24; di tengah arah panjang (4,25 per sisi); 7 dari tepi atas (sisi header 16 pin), 5 dari tepi bawah; bingkai + PCB 8,5 |
| Layar | 64,5 x 14,0; 3 dari tepi kiri/kanan bingkai, 5 dari tepi atas/bawah bingkai |
| Kaki trapesium putih | tinggi 4 − tebal PCB; sisi panjang 2,5 dari tepi bingkai; sisi pendek 4 |
| Tinggi total | 16,9 dari muka bingkai ke titik tertinggi backpack; backpack 8,4 di belakang PCB |
| Backpack I2C | 41,6 x 18,8; tepi kanan 47,8 dari tepi kiri PCB; tepi bawah 20 dari tepi atas PCB |

> Terbuka: 64,5 + 2 x 3 = 70,5, bukan 71,5 (selisih 1 mm pada margin layar). Ukur ulang sebelum jendela dicetak.

---

## 3. Berkas parameter Fusion
| Berkas | Isi |
|---|---|
| [fusion/fusion_parameters_lcd.csv](fusion/fusion_parameters_lcd.csv) | Parameter LCD saja (PCB, bingkai, trapesium, tinggi, backpack, kabel) |
| [fusion/fusion_parameters_housing_elektronik.csv](fusion/fusion_parameters_housing_elektronik.csv) | Parameter global housing elektronik: semua parameter LCD, kompensasi susut, dan jendela layar |

Format: `Name,Unit,Expression,Value,Comments,Favorite` (sama dengan berkas housing grip). Parameter baru ditambahkan di baris paling bawah.

---

## 4. Kompensasi susut cetak
Dari hasil cetak modul 3D printing tim sendiri:

| Parameter | Nilai | Dasar (rancang → hasil cetak) | Pemakaian |
|---|---|---|---|
| `comp_xy` | 0,6 mm | LCD 80,6 → 80,0 dan 36,6 → 36,0; USB 16 → 15,4; HX711 12 → 11,3 | Total per dimensi bukaan; offset per sisi = `comp_xy / 2` (mis. `-(0,5 mm + comp_xy / 2)`) |
| `comp_z` | 1 mm | USB 8 → 7,0; HX711 6 → 5,0 | Tinggi bukaan di dinding tegak |
| `comp_hole` | 1 mm | M3 3,4 → 2,4 | Diameter lubang bulat kecil |

Celah lid terhadap bingkai LCD memakai parameter tersendiri (mis. `lid_gap` 0,5 atau 0), **bukan** `comp_z`.

---

## 5. Aturan desain cetak 3D (dari modul 3D printing)
- Sliding fit +0,2-0,3 mm per sisi; press fit +0,0-0,1 mm per sisi.
- Lubang M3 3,4 mm; dinding ≥ 1,2 mm (≥ 2 mm bila menahan beban; tebal dinding usulan lid 2 mm).
- Overhang ≤ 45°; bridge ≤ 10 mm; chamfer 0,3-0,5 mm di tepi bawah; detail ≥ 0,8 mm lebar dan ≥ 0,5 mm tinggi.
- PLA (PETG atas permintaan); lapisan 0,20 mm, infill 15-20 %, 3 dinding bila menahan beban.

---

## 6. Sisa pekerjaan
1. Tetapkan posisi PCB `hgd-v1` (80 x 55 mm, final 16 Okt. 2026) di body; dinding mengikuti level shifter di ujung antena dan arah gantung HX711.
2. Lubang tombol dan LED di lid dari posisi SW1 dan D1 (ukur tombol B3-B4 dan LED L1-L3 di tabel pengukuran).
3. Cutout USB (E8-E9 di tab ESP32) dan kabel load cell.
4. Tiang untuk empat lubang PCB (Ø3,0, segaris PCB LCD); tumpuan dekat tombol bila PCB melentur.
5. Selisih margin layar 1 mm (Bagian 2).

---

## 7. Belum dirancang (housing grip)
- **Stop overload mekanis.** Pada 70 kgf pelat melentur ≈ 0,24 mm ke arah load cell; fitur penahan beban lebih belum didesain. Pada laporan dinyatakan sebagai batasan.
- **Penjepit A ke meja untuk kalibrasi** (memakai sisa batang ≈ 11,5 mm di ujung); sementara alas karet menahan A.

---

## 8. Perubahan
| Versi | Tanggal | Perubahan |
|---|---|---|
| v3 | 16 Okt. 2026 | Batasan Bagian 1.1 diperbarui ke penempatan PCB final: papan 80 x 55, lubang segaris PCB LCD, posisi level shifter dan HX711, lubang jauh dari tombol |
| v2 | 16 Okt. 2026 | Ukuran LCD terukur; berkas parameter Fusion; jendela lid = area layar; kompensasi susut; aturan cetak 3D; batasan dari penempatan PCB v2 |
| v1 | 13 Okt. 2026 | Rencana awal |

---
title: Desain Housing Elektronik - Hand Grip Dynamometer

---

# Desain Housing Elektronik - Hand Grip Dynamometer (v7)
**Proyek akhir - embedded system**

> Housing elektronik: wadah PCB, ESP32, modul HX711, level shifter, LCD, tombol, dan LED. Housing grip (load cell) ada di [desain-housing-load-cell.md](../housing-load-cell/desain-housing-load-cell.md); ukuran komponen di [tabel-pengukuran-hgd.xlsx](../pengukuran/tabel-pengukuran-hgd.xlsx); PCB di [desain-pcb-hgd.md](../pcb/desain-pcb-hgd.md); parameter Fusion di [fusion/](fusion/); model 3D terbaru (lid + body dengan bibir, snap-fit, dan slot-U kabel, ekspor Fusion 30 Okt. 2026): [fusion/electronics_housing_lipped.step](fusion/electronics_housing_lipped.step); model PCB untuk cek pas: [../pcb/kicad/proyek/hgd-v1.step](../pcb/kicad/proyek/hgd-v1.step); induk: [catatan-proyek-hgd.md](../catatan-proyek-hgd.md).

---

## 1. Susunan ✔ desain v1 selesai, belum disetujui

**Status v1 (17 Okt. 2026).** Desain housing elektronik versi 1 selesai di Fusion: lid berdinding dan body baki, jendela USB dan kabel load cell, bibir 45° dan 4 snap-fit (dicek dari STEP tiap fase dan uji parameter), slice uji di Bambu Studio. Model: [fusion/electronics_housing_lipped.step](fusion/electronics_housing_lipped.step). Belum disetujui dan belum dicetak; cetak menunggu PCB tiba. Sisa pekerjaan di Bagian 6.
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

### 1.1 Batasan dari penempatan PCB (usulan v2)
- Komponen sisi atas (SW1 → R1 → D1, searah DevKit) harus di luar jejak PCB LCD. Pada model Fusion, courtyard SW1 masih ±3,75 mm dari tepi PCB LCD.
- Modul level shifter berada di samping DevKit (sisi pin 30) dan menonjol ±15 mm dari tepi DevKit; dinding housing di sisi itu bertambah ±4 mm + tebal dinding dibanding model Fusion saat ini.
- Modul HX711 menggantung ke bawah area LCD (sudut terjauh dari antena); kabel load cell keluar dari ujung modul itu.
- Tidak ada bagian logam (baut, sekrup) dalam 15 mm di samping dan depan antena DevKit.
- Satu tiang M3 dekat tombol supaya PCB tidak melentur saat ditekan.

Rinci di Bagian 8.4 [desain-pcb-hgd.md](../pcb/desain-pcb-hgd.md) dan gambar [penempatan-pcb-hgd.svg](../pcb/gambar/penempatan-pcb-hgd.svg).

### 1.2 Model Fusion 16 Okt. 2026 dan usulan bibir
Gambar: [gambar/housing-elektronik-hgd.svg](gambar/housing-elektronik-hgd.svg), proyeksi STEP `electronics_housing_v1` (ekspor 16 Okt. 2026 22:30) dengan PCB `hgd-v1-verbatim` (soket 1x15 DevKit ikut digambar).

Desain pemilik (dari STEP):
- Lid berdinding: pelat z 9-11, dinding 2,0 sampai z −22; luar 105 x 105.
- Body: baki 105 x 105 x 12 (z −34 sampai −22), dinding dan lantai 2,0.
- Jendela USB di dinding kiri lid: 15,0 x 9,0, y −66,5 sampai −51,5, z −18,2 sampai −9,2 (pusat cocok dengan USB DevKit di PCB v1, y −59,0).
- Jendela kabel load cell di dinding kanan lid: 15,0 x 9,0, y 3,5 sampai 18,5. **Diganti slot-U (Bagian 1.4, 30 Okt. 2026).**

Usulan bibir (Claude; dimodelkan di Fusion 17 Okt. 2026):

| Bagian | Usulan | Alasan |
|---|---|---|
| Bibir | Cincin di body, di dalam dinding lid: tebal 1,6, tinggi 3,0 (z −22 sampai −19), celah 0,25 per sisi ke muka dalam dinding lid | Menahan lid supaya tidak bergeser; sliding fit 0,2-0,3 per sisi (Bagian 5); 4 perimeter 0,4 |
| Tumpuan | Bibir ditopang tumpuan 45° di muka dalam dinding body (bibir 1,85 lebih ke dalam dari dinding body) | Tercetak tanpa support dengan body berdiri di lantainya (overhang ≤ 45°) |
| Tampilan luar | Dinding lid dan body tetap 2,0, rata di luar, sambungan di z −22 | Tidak mengubah desain luar |

Tidak bertabrakan: kedua jendela mulai di z −18,2 (di atas bibir); bagian DevKit terendah z −15,6. Belum diputuskan: penahan kabel load cell di dalam dekat jendela kanan (HX711 hanya ditahan gesekan soket J1).

Gambar (v2) bergaya sama dengan gambar housing load cell: potongan A-A (x = −30, dari STEP), detail B (snap), detail C (lidah), tampak atas, tampak depan, legenda, dan arsiran.

### 1.3 Usulan snap-fit lid ke body (Claude; dimodelkan di Fusion 17 Okt. 2026)
Lid harus bisa dibuka berulang (kabel di sisi bawah PCB dikencangkan dengan membuka lid), jadi dipakai lidah horizontal yang melentur di bidang XY, bukan kait tegak.

| Bagian | Usulan |
|---|---|
| Letak | 4 lidah di bibir dinding depan dan belakang, \|x\| 15 (akar) sampai 35 (ujung); dinding kiri/kanan bebas untuk jendela |
| Lidah | l = 20, b = 5 (z −21 sampai −16), h 1,6 di akar meruncing linear ke 0,8 di ujung (Covestro desain 2); fillet akar R1,0 (R/h ≈ 0,6) |
| Tonjolan | Di muka luar ujung lidah, panjang 6 (\|x\| 29-35), menonjol 1,45, sisi 45°/45° (lead-in di atas, retensi di bawah), puncak datar 0,4 |
| Slot | Tembus di dinding lid 7,6 x 4,3 (z −20,65 sampai −16,35) = 7 x 3,3 + `comp_xy` / `comp_z`; kait y = 1,45 − 0,25 = 1,2; tonjolan berhenti 0,8 sebelum muka luar |
| Bibir di lidah | Ring naik ke z −16 hanya di blok akar 3 dan lidah (\|x\| 12 sampai 35); ring terputus 0,5 di ujung lidah; di tempat lain tetap z −19 |
| Cetak | Celah di bawah lidah 2,0 (1,0 + `comp_z`): tumpuan diturunkan ke z −23 di zona lidah. Dua tiang korban 0,8 x 0,8 (\|x\| 24,6 dan 34,2) memecah jembatan menjadi 9,6 dan 8,8 (aturan modul ≤ 10); tiang dipotong setelah cetak |
| Melepas | Tarik lid lurus ke atas (retensi 45° tidak mengunci) atau tekan tonjolan dengan pin lewat slot |

Perhitungan (Covestro "Snap-Fit Joints for Plastics", Table 1 desain 2; data Bambu Lab PLA Basic TDS V3.0, arah XY):

| Besaran | Nilai |
|---|---|
| Regangan | ε = y·h / (1,09·l²) = 1,2 × 1,6 / (1,09 × 400) = 0,44 % |
| Batas | ε_y ≈ σ/E = 35 / 2580 = 1,36 % (konservatif, TDS tanpa titik luluh); × 0,7 (amorf, Fig. 13) × 0,6 (buka-pasang berulang, Table 2) = 0,57 % → lolos |
| Gaya lentur | P = (b·h²/6)·Eₛ·ε / l ≈ 1,3 N (Eₛ = 2750 MPa, modulus lentur XY) |
| Gaya pasang/lepas | W = P·(µ + tan 45°)/(1 − µ tan 45°) ≈ 5,2 N per lidah, ≈ 21 N untuk 4 (µ ≈ 0,6 diasumsikan); terkunci sendiri bila µ·tan α′ ≥ 1 (α′ ≳ 59°) |

Aturan FDM: Hubs (celah 0,5, lebar ≥ 5, lentur di XY, tidak ada tegangan saat terpasang), Formlabs (kait 1,2, lidah panjang dan meruncing), panduan PLA Bambu Lab (gaya di bidang XY; elongasi Z 7,5 % vs XY 12,2 %). Lidah 15 mm menghasilkan 0,78 %, melebihi batas, jadi panjang tetap 20. Lab memakai Bambu PLA Basic dan Generic PLA (arahan laboran); perhitungan memakai data PLA Basic, jadi cetak uji dan cetakan akhir memakai filamen yang sama, dan preset slicer disamakan dengan filamen yang dipasang. Asumsi terbuka: ε_y dan µ; cetak uji satu lidah dan slot (pasang-lepas 20-30 kali) sebelum mencetak lid dan body.

---

### 1.4 Slot-U kabel load cell (usulan Claude, dimodelkan di Fusion 30 Okt. 2026)
Gambar: [gambar/slot-kabel-load-cell.svg](gambar/slot-kabel-load-cell.svg) (tampak dinding kanan, detail slot, potongan A-A; ukuran housing dari STEP 30 Okt. 2026).

**Masalah.** Kabel load cell menyatu dengan load cell dan disolder ke HX711, jadi HX711 (15,6 x 24) tidak bisa dimasukkan lewat jendela kabel 15 x 9. Sketsa pintu geser dovetail (30 Okt. 2026) diganti usulan di bawah.

| Bagian | Usulan | Alasan |
|---|---|---|
| Slot-U | Potongan di dinding kanan lid dari tepi bawah (z −22) ke atas, lebar `cslot_w` 4,6, tinggi `cslot_h` 9,5; pusat 11 mm dari muka luar dinding belakang (y 11,25) | Lid terbuka di bawah, jadi kabel dimasukkan dari sisi itu; HX711 tidak perlu lewat dinding |
| Lidah | Di tepi atas dinding body, `ctongue_w` 3,5 x `ctongue_h` 5,3, menyatu dengan bibir | Menutup slot dan menjepit kabel 0,3 mm (`c_squeeze`): tarikan kabel dari luar tidak sampai ke sambungan solder HX711 |
| Tinggi kabel | Pusat 7 mm di atas sambungan (`cable_zc`, z −15) | 2 mm di atas puncak bibir (z −19) |
| Cetak | Tepi atas slot menjadi jembatan 4,6 | Memenuhi aturan jembatan ≤ 10 mm (jendela 15 x 9 lama: 15 mm) |

Cara rakit: (1) lid dibalik, PCB sudah di dalam; (2) kabel diletakkan ke slot dari sisi terbuka, HX711 ditancapkan ke J1; (3) body dipasang, lidah masuk ke slot dan menjepit kabel. Bongkar cukup dengan melepas body; tanpa lem dan tanpa bagian geser.

Cek STEP 30 Okt. 2026 22:36: slot y 8,95 sampai 13,55 dan tepi atas z −12,5; lidah y 9,5 sampai 13,0, x 50,25 sampai 52,5 (menyatu dengan bibir, rata di muka luar), puncak z −16,7; jendela 15 x 9 lama sudah dihapus; tidak ada tumpang tindih antara lid, body, dan komponen.

Asumsi: hanya bukaan yang menyusut saat dicetak (`comp_xy`, `comp_z`); lidah tercetak sesuai ukuran. Bila lidah macet, kikir sampai ≈ 3,3. `c_squeeze` diatur setelah cetak uji.

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
| [fusion/fusion_parameters_housing_elektronik.csv](fusion/fusion_parameters_housing_elektronik.csv) | Parameter global housing elektronik: semua parameter LCD, kompensasi susut, jendela layar, bibir dan snap-fit (`lip_cl` sampai `pillar2_x`), slot-U kabel load cell (`cable_d` sampai `ctongue_h`), ukuran housing, jendela, dan lubang (`housing_L` sampai `floor_hole_d`, terukur dari STEP 30 Okt. 2026) |
| [fusion/work_instructions_fusion_housing_elektronik.md](fusion/work_instructions_fusion_housing_elektronik.md) | Langkah Fusion untuk bibir, lidah snap, tiang korban, dan slot; cek; cetak uji satu lidah |

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
1. ✔ Posisi PCB `hgd-v1` v1 di model Fusion (tergantung di lid).
2. ✔ Jendela USB dan kabel load cell 15 x 9 di dinding lid (Bagian 1.2).
3. ✔ Bibir dan 4 snap-fit dimodelkan; sketsa tonjolan, slot, dan lidah dibuat parametrik dan lolos uji parameter `snap_y`.
4. ✔ Slice uji (X1 Carbon, Textured PEI, PLA Basic, 0,20 mm, 3 dinding, infill 15 %, lid dibalik): 90,2 g, 1 j 55 m, tanpa support. Tepi atas jendela USB dan kabel tercetak sebagai overhang 15 mm (> aturan jembatan 10 mm); support di jendela diputuskan laboran. Ukuran 105 mm, 90 g, dan 1 j 55 m melebihi batas modul: perlu izin lab/dosen.
5. Lubang tombol dan LED di lid dari posisi SW1, D1, dan **D2 (LED daya 3 mm, ditambahkan di PCB 17 Okt. 2026)**; ukur tombol B3-B4 dan LED L1-L3 di tabel pengukuran. Di model 30 Okt. 2026 sudah ada lubang tombol Ø5,3, LED D1 Ø6,9, dan LED D2 Ø5,0 (`btn_hole_d`, `led1_hole_d`, `led2_hole_d`), masing-masing ukuran bagian + `comp_hole`. Lantai body punya 2 lubang Ø5,9 untuk menekan tombol EN dan BOOT DevKit (`floor_hole_d`). Lubang baut dari atas Ø5 (`mnt_hole_d`) = lubang PCB Ø3 + 1 mm kelonggaran + `comp_hole`.
6. Tiang PCB di lid sesuai pola H1-H4 (Ø3,0; ukuran baut belum dipastikan, M2,5 atau M3). Belum ada di model v1.
7. Cetak uji satu lidah dan slot (Fase 7 instruksi kerja Fusion) sebelum mencetak lid dan body.
8. Selisih margin layar 1 mm (Bagian 2).
9. Cek pas dengan model PCB [../pcb/kicad/proyek/hgd-v1.step](../pcb/kicad/proyek/hgd-v1.step) (DevKit dan modul di sisi bawah). Sejak 27 Okt. 2026 STEP ini diekspor dari papan utama 26 Okt. dan tidak memuat dua soket 1x15 DevKit; soket tidak memengaruhi desain housing.
10. ✔ Slot-U kabel dan lidah (Bagian 1.4) dimodelkan di Fusion; jendela kabel 15 x 9 dihapus; dicek dari STEP 30 Okt. 2026.

---

## 7. Belum dirancang (housing grip)
- **Stop overload mekanis.** Pada 70 kgf pelat melentur ≈ 0,24 mm ke arah load cell; fitur penahan beban lebih belum didesain. Pada laporan dinyatakan sebagai batasan.
- **Penjepit A ke meja untuk kalibrasi** (memakai sisa batang ≈ 11,5 mm di ujung); sementara alas karet menahan A.

---

## 8. Perubahan
| Versi | Tanggal | Perubahan |
|---|---|---|
| v7 | 30 Okt. 2026 | Bagian 1.4: slot-U kabel load cell + lidah body (menggantikan jendela 15 x 9; HX711 tidak muat lewat jendela); parameter `cable_d` sampai `ctongue_h` di CSV; dimodelkan dan dicek dari STEP 30 Okt. 2026 (STEP di `fusion/` diperbarui); parameter ukuran housing, jendela, dan lubang (`housing_L` sampai `floor_hole_d`) diukur dari STEP; gambar `gambar/slot-kabel-load-cell.svg`; sisa pekerjaan butir 5 dan 10 |
| v6 | 27 Okt. 2026 | Sisa pekerjaan butir 9: model PCB `hgd-v1.step` kini versi 26 Okt. 2026, tanpa soket 1x15 DevKit |
| v5 | 17 Okt. 2026 | **Desain v1 selesai (belum disetujui)**: status di Bagian 1; STEP model terbaru di `fusion/`; sisa pekerjaan disusun ulang (lubang tombol/LED termasuk D2, tiang PCB, cetak uji, cek pas dengan STEP PCB) |
| v4 | 17 Okt. 2026 | Bagian 1.3: usulan snap-fit (4 lidah, perhitungan Covestro, data PLA Basic); celah bawah lidah 2,0 dan tiang korban (jembatan ≤ 10); slot dengan kompensasi susut; parameter snap di CSV; instruksi kerja Fusion; gambar v2 bergaya gambar housing load cell; STEP model terbaru disimpan di `fusion/` |
| v3 | 16 Okt. 2026 | Bagian 1.2: model Fusion 16 Okt. 2026 (lid berdinding, body baki, jendela USB dan kabel) dan usulan bibir; gambar `gambar/housing-elektronik-hgd.svg` dari STEP Fusion |
| v2 | 16 Okt. 2026 | Ukuran LCD terukur; berkas parameter Fusion; jendela lid = area layar; kompensasi susut; aturan cetak 3D; batasan dari penempatan PCB v2 |
| v1 | 13 Okt. 2026 | Rencana awal |

# Instruksi kerja: bibir dan snap-fit housing elektronik di Fusion (v1)

Lingkup: menambahkan bibir (lip), 4 lidah snap, tiang korban, dan slot ke model `electronics_housing_v1` yang sudah ada (lid berdinding dan body baki). Bentuk luar, jendela USB dan kabel, serta posisi PCB dan LCD tidak diubah. Desain dan perhitungan: [../desain-housing-elektronik.md](../desain-housing-elektronik.md), Bagian 1.3; gambar: [../gambar/housing-elektronik-hgd.svg](../gambar/housing-elektronik-hgd.svg) (potongan A-A, detail B, detail C).

Nama perintah ditulis seperti di menu Fusion versi bahasa Inggris; letaknya bisa sedikit berbeda antarversi.

## Acuan koordinat (dari STEP 16 Okt. 2026 22:30)

| Bidang / muka | Nilai |
|---|---|
| Luar lid dan body | x ±52,5; y −82,75 (depan) sampai 22,25 (belakang) |
| Muka dalam dinding | x ±50,5; y −80,75 dan 20,25 |
| Sambungan lid-body (muka atas dinding body) | z −22 |
| Bidang tengah depan-belakang | y −30,25 |

Hasil akhir yang dituju (lidah depan-kiri; tiga lainnya dicerminkan):

| Fitur | Ukuran |
|---|---|
| Bibir | muka luar 0,25 di dalam muka dalam dinding; tebal 1,6; z −22 sampai −19 |
| Tumpuan | dari muka dalam dinding body ke muka dalam bibir (1,85); turun dengan chamfer 45° sampai z −25,7 |
| Lidah | x −15 (akar) sampai −35 (ujung); z −21 sampai −16; tebal 1,6 di akar, 0,8 di ujung |
| Blok akar | x −12 sampai −15; z −22 sampai −16; tebal 2,6 |
| Tonjolan | x −29 sampai −35; menonjol 1,45 keluar; puncak datar 0,4 di z −18,5; sisi 45° |
| Kantong di bawah lidah | x −15 sampai −35,5; tumpuan dipotong turun ke z −23 (celah 2,0) |
| Tiang korban | 2 buah, 0,8 x 0,8 x 2,0, di x −24,6 sampai −25,4 dan −34,2 sampai −35,0 |
| Slot lid | 7,6 x 4,3, tembus dinding depan lid, tengah x −32, z −20,65 sampai −16,35 |

## Fase 1: persiapan

- [ ] Simpan versi model sekarang dengan deskripsi "sebelum snap-fit", supaya bisa kembali.
- [ ] Buka **Modify > Change Parameters**, impor `fusion_parameters_housing_elektronik.csv` (baris baru `lip_cl` sampai `pillar2_x` di bagian bawah). Bila nama bentrok dengan parameter yang sudah ada di model, ganti nama di model, bukan di CSV.
- [ ] Cek kolom Value: `ledge_w` 1,85; `snap_h1` 0,8; `snap_gap_under` 2; `bump_d` 1,45; `bump_zc` 3,5; `bump_xc` 32; `slot_L` 7,6; `slot_H` 4,3; `pillar2_x` 34,2.
- [ ] Buat bidang konstruksi (**Construct > Offset Plane**) dari muka atas dinding body (z −22), di komponen Body:
  - `J`: offset 0 (z −22), sambungan.
  - `T0`: offset `snap_z0` (z −21), dasar lidah.
  - `P`: offset `snap_z0 - snap_gap_under` (z −23), lantai kantong.
- [ ] Buat bidang tengah depan-belakang: **Construct > Midplane**, pilih muka luar depan dan muka luar belakang (y −30,25). Bidang tengah kiri-kanan: bidang YZ asal (x = 0).

## Fase 2: bibir dan tumpuan (komponen Body)

- [ ] Aktifkan komponen Body.
- [ ] **Sketch** pada bidang `J`. **Project** tepi dalam muka atas dinding body (loop persegi dalam, x ±50,5).
- [ ] **Offset** loop itu ke dalam sebesar `lip_cl` (muka luar bibir), lalu sekali lagi ke dalam sebesar `ledge_w` dari loop asli (muka dalam bibir). Hasilnya tiga loop: tepi dinding, 0,25 ke dalam, 1,85 ke dalam.
- [ ] **Extrude** profil dua pita (0,25 + 1,6 = 1,85) ke bawah sejauh `2 * ledge_w`, operasi **Join**. Ini blok tumpuan, z −22 sampai −25,7.
- [ ] **Chamfer** tepi bawah-dalam blok tumpuan (seluruh loop), **Equal distance** `ledge_w`. Hasilnya muka datar 1,85 dari z −22 ke −23,85, lalu miring 45° ke dinding di z −25,7.
- [ ] **Extrude** pita bibir saja (antara loop 0,25 dan 1,85) ke atas sejauh `lip_h`, **Join**. Bibir z −22 sampai −19.
- [ ] Cek: **Inspect > Measure** dari muka luar bibir ke muka dalam dinding lid = 0,25 di keempat sisi.

## Fase 3: kantong dan lidah depan-kiri (komponen Body)

Kerjakan satu lidah dulu (depan-kiri, dekat x −15 sampai −35), lalu cerminkan di Fase 5.

### 3a. Kantong di bawah lidah
- [ ] **Sketch** pada bidang `P`. Persegi panjang x dari `-snap_x_root` sampai `-(snap_x_root + snap_l + snap_end_gap)` (−15 sampai −35,5), y dari muka dalam dinding body (−80,75) sampai muka dalam bibir (−78,9). Kunci tepinya dengan **Project** muka-muka tersebut.
- [ ] **Extrude** ke atas sejauh `lip_h + snap_gap_under - snap_z0` (4 mm, z −23 sampai −19), operasi **Cut**, **Objects To Cut** = body komponen Body saja (jangan Lid).
- [ ] Cek: di zona ini bibir hilang dan tumpuan tinggal z −23 sampai −25,7 (chamfer tetap).

### 3b. Blok akar
- [ ] **Sketch** pada bidang `J`. Persegi panjang x `-(snap_x_root - snap_root_L)` sampai `-snap_x_root` (−12 sampai −15), y dari muka luar bibir (−80,5) ke dalam sejauh `lip_t + snap_root_ext` (sampai −77,9).
- [ ] **Extrude** ke atas `snap_z0 + snap_b` (6 mm, sampai z −16), **Join**.

### 3c. Lidah meruncing
- [ ] **Sketch** pada bidang `T0`. Empat garis:
  - muka luar: dari (−15, −80,5) ke (−35, −80,5), segaris dengan muka luar bibir (**Project** lalu **Collinear**);
  - ujung: dari (−35, −80,5) ke (−35, −80,5 + `snap_h1`);
  - muka dalam (miring): dari ujung itu ke (−15, −80,5 + `snap_h0`);
  - akar: menempel ke muka blok akar di x −15.
- [ ] Dimensi dengan parameter: panjang `snap_l`, tebal akar `snap_h0`, tebal ujung `snap_h1`, jarak akar dari x = 0 `snap_x_root`.
- [ ] **Extrude** ke atas `snap_b` (z −21 sampai −16), **Join**.
- [ ] **Fillet** tepi tegak di pertemuan muka dalam lidah dengan muka blok akar, jari-jari `snap_fillet`.
- [ ] Cek: celah ujung lidah ke ujung bibir di x −35,5 adalah `snap_end_gap` (0,5); di bawah lidah ada celah 2,0 ke lantai kantong.

### 3d. Tonjolan
- [ ] **Sketch** pada muka ujung lidah (x −35). Profil trapesium di sisi luar lidah (arah −y):
  - alas di muka luar lidah, tinggi `bump_flat + 2 * bump_d` (3,3), tengahnya di z `bump_zc` dari bidang `J` (z −18,5);
  - puncak datar `bump_flat` (0,4), sejauh `bump_d` (1,45) keluar dari muka luar lidah;
  - kedua sisi miring: konstrain sudut 45° terhadap muka lidah.
- [ ] **Extrude** ke arah +x (ke dalam zona lidah) sejauh `bump_L` (6, sampai x −29), **Join**.
- [ ] Cek: puncak tonjolan di y −81,95, yaitu 1,2 (`snap_y`) melewati muka dalam dinding lid. Ini disengaja; slot di Fase 4 membuka tempatnya.

### 3e. Tiang korban
- [ ] **Sketch** pada bidang `P`. Dua persegi `pillar_w` x `pillar_w`, sisi luar segaris muka luar lidah (y −80,5 sampai −79,7):
  - tiang 1: x `-pillar1_x` sampai `-(pillar1_x + pillar_w)` (−24,6 sampai −25,4);
  - tiang 2: x `-pillar2_x` sampai `-(pillar2_x + pillar_w)` (−34,2 sampai −35,0).
- [ ] **Extrude** ke atas `snap_gap_under` (z −23 sampai −21), **Join**. Tiang menyambung lantai kantong ke dasar lidah.
- [ ] Ganti nama fitur di timeline menjadi `tiang_korban` supaya mudah dikenali.

## Fase 4: slot di lid (komponen Lid)

- [ ] Aktifkan komponen Lid.
- [ ] **Sketch** pada muka luar dinding depan lid (y −82,75). Persegi panjang **Center Rectangle** dengan tengah di x `-bump_xc` (−32) dan z −18,5 (`bump_zc` di atas bidang `J`); lebar `slot_L`, tinggi `slot_H`.
- [ ] **Extrude** menembus dinding (**To Object** muka dalam dinding lid, atau jarak 2 mm), **Cut**, Objects To Cut = Lid saja.
- [ ] Cek: dinding lid di bawah slot tinggal 1,35 (z −22 sampai −20,65).
- [ ] Opsional, bila lid seret saat dipasang: **Chamfer** 0,5 tepi bawah-dalam dinding lid.

## Fase 5: cerminkan ke empat lidah

- [ ] Body: **Create > Mirror**, Type **Features**, pilih fitur Fase 3 (kantong, blok akar, lidah, fillet, tonjolan, tiang korban), bidang cermin YZ (x = 0). Hasil: lidah depan-kanan.
- [ ] Body: **Mirror** lagi, pilih fitur asli dan hasil cermin tadi, bidang = midplane depan-belakang (y −30,25). Hasil: dua lidah belakang.
- [ ] Lid: lakukan hal yang sama untuk fitur slot (YZ, lalu midplane).
- [ ] Bila **Mirror** fitur gagal (Fusion kadang gagal untuk fillet), cerminkan tanpa fillet lalu buat ulang fillet di lidah hasil cermin.

## Fase 6: pemeriksaan di Fusion

- [ ] **Inspect > Interference**, pilih Lid dan Body. Harapan: tidak ada tumpang tindih (tonjolan berada di dalam slot).
- [ ] **Inspect > Section Analysis** pada bidang x = −30: bandingkan dengan detail B di gambar (celah 0,25, tonjolan 1,45, slot 4,3, celah bawah 2,0).
- [ ] **Section Analysis** pada bidang z −18,5: bandingkan dengan detail C (lidah meruncing 1,6 → 0,8, R1,0, tiang di bawah lidah).
- [ ] Cek bentrok dengan isi: tidak ada bagian PCB, DevKit, atau LCD di bawah z −16 di dekat dinding depan dan belakang (DevKit terendah z −15,6, 5,4 dari lidah ke arah y).
- [ ] Uji parameter: ubah `snap_y` ke 1,0, pastikan tonjolan menjadi 1,25 dan slot 3,9; lalu kembalikan ke 1,2.
- [ ] Simpan versi dengan deskripsi "bibir + snap-fit v1".

## Fase 7: cetak uji satu lidah (sebelum mencetak lid dan body penuh)

- [ ] Buat potongan uji: salin Body dan Lid ke komponen baru (**Copy**, **Paste New**), lalu **Combine > Intersect** dengan kotak sekitar satu lidah (misal x −8 sampai −42, y −83 sampai −70, seluruh tinggi masing-masing bagian). Hasil: sudut body dengan satu lidah, dan potongan dinding lid dengan satu slot.
- [ ] Ekspor kedua potongan sebagai 3MF atau STL; cetak dengan pengaturan yang sama dengan cetakan akhir (PLA Basic, lapisan 0,20, 3 dinding).
- [ ] Orientasi: potongan body berdiri di lantainya; potongan lid dengan pelat di bawah. Support **off**; di preview slicer, pastikan dasar lidah tercetak sebagai jembatan antara blok akar, tiang 1, dan tiang 2.
- [ ] Setelah cetak: potong kedua tiang korban rata dengan dasar lidah memakai flush cutter, dari sisi dalam; buang serabut di celah ujung 0,5. Lidah harus bisa ditekan ke dalam dengan jari.
- [ ] Pasang dan lepas 20 sampai 30 kali. Catat: retak di akar, tonjolan aus, lid goyang atau terlalu keras dilepas.
- [ ] Ukur dengan jangka sorong: tonjolan (rancang 1,45), slot (rancang 7,6 x 4,3; hasil diharapkan mendekati 7 x 3,3), celah bawah lidah (rancang 2,0).
- [ ] Bila lid goyang: naikkan `snap_y` 0,1-0,2. Bila terlalu keras atau akar retak: turunkan `snap_y` 0,1-0,2 atau perpanjang `snap_l`. Setiap perubahan dicek ulang regangannya (Bagian 1.3 desain).

## Aturan cetak yang dipakai

- Overhang ≤ 45° (sisi tonjolan, chamfer tumpuan); bridge ≤ 10 mm (jembatan di bawah lidah 9,6 dan 8,8 berkat tiang korban); detail ≥ 0,8 mm (tiang 0,8).
- Kompensasi susut: slot memakai `comp_xy` dan `comp_z`; celah bawah lidah memakai `comp_z`. Bagian positif (lidah, tonjolan) tidak dikompensasi; dicek di cetak uji.
- Slice review harus disetujui sebelum mencetak lid dan body penuh.

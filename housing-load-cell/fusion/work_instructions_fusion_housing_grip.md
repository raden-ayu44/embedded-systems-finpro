# Instruksi kerja: housing load cell di Fusion (v6)

Lingkup: memodelkan load cell acuan, Spacer A dengan dinding klem kabel, Spacer B, serta pelat A dan B. Ridge, balok kalibrasi, dan bentuk tambahan lain dirancang bebas oleh tim. Desain dan alasan: [../desain-housing-load-cell.md](../desain-housing-load-cell.md) dan [../asumsi.md](../asumsi.md); parameter: [fusion_parameters_housing_grip.csv](fusion_parameters_housing_grip.csv) (parameter bertanda WAJIB di kolom keterangan tidak boleh diubah tanpa ukur ulang).

Nama perintah ditulis seperti di menu Fusion versi bahasa Inggris; letaknya bisa sedikit berbeda antarversi. Ketik nama parameter di kotak ukuran, bukan angka.

## Acuan koordinat

Titik asal di tengah load cell, muka bawah load cell di z 0. x sepanjang load cell (ujung tetap, sisi kabel, di x negatif), y melintang (kabel keluar di sisi −y), z ke atas (pelat A di atas, pelat B di bawah).

| Bidang / muka | Nilai |
|---|---|
| Muka ujung load cell | x ±65 (`LC_L / 2`) |
| Muka samping load cell | y ±14 (`LC_W / 2`) |
| Muka atas load cell | z 21,5 (`LC_H`) |
| Muka bawah pelat A | z 24 (`LC_H + gap`) |
| Muka atas pelat B | z −2,5 (`-gap`) |
| Sumbu lubang baut | x ±53 (`hole_x`), y ±7,5 (`hole_y`) |

Hasil akhir yang dituju:

| Bagian | Ukuran |
|---|---|
| Load cell acuan | 130 x 28 x 21,5; 4 lubang di (±53, ±7,5); lubang kabel Ø5 di sisi −y, x −59,7, z 10,75 |
| Spacer A (atas, ujung tetap) | pelat 27 x 34 x 2,5 di z 21,5 sampai 24, x −68 sampai −41; bidang kontak 24 x 28 |
| Bibir Spacer A | 2,5 x 2,5 di muka ujung dan sisi +y, turun ke z 19; lebar dalam 29; bibir ujung 0,5 dari muka ujung load cell |
| Dinding klem Spacer A | sisi −y, tebal 2,5, sepanjang spacer, turun ke z 2,5 |
| Slot jepit kabel | lubang Ø5 di x −59,7, z 10,75; mulut 3,8 dari tepi bawah dinding ke lubang |
| Lubang Spacer A | Ø7,6 di (−53, ±7,5) |
| Spacer B (bawah, ujung bebas) | pelat 27 x 34 x 2,5 di z −2,5 sampai 0, x 41 sampai 68; bibir di muka ujung dan kedua sisi, naik ke z 2,5; lubang Ø7,6 di (53, ±7,5) |
| Pelat A | 170 x 30 x 10 di z 24 sampai 34; lubang Ø6,6 hanya di (−53, ±7,5) |
| Pelat B | 170 x 30 x 10 di z −12,5 sampai −2,5; lubang Ø6,6 hanya di (53, ±7,5) |

## Fase 1: persiapan

- [ ] Buat desain baru dengan jenis **Hybrid Design**, satuan mm, lalu simpan dengan nama yang jelas.
- [ ] Impor parameter: **Modify > Change Parameters**, klik ikon import (ikon keempat dihitung dari ikon tanda +), pilih `fusion_parameters_housing_grip.csv` dari komputer, **Import**, lalu **OK**.
- [ ] Cek kolom Value: `hole_x` 53; `hole_y` 7,5; `hole_sp_d` 7,6; `sp_in_W` 29; `spacer_L` 27; `spacer_W` 34; `clamp_hole_d` 5; `clamp_mouth_w` 3,8; `bar_hole_edge` 32; `total_t` 51,5.
- [ ] Buat bidang konstruksi (**Construct > Offset Plane**) dari bidang XY (z 0):
  - `LC_TOP`: offset `LC_H` (z 21,5), muka atas load cell.
  - `A_BOT`: offset `LC_H + gap` (z 24), muka bawah pelat A.
  - `B_TOP`: offset `-gap` (z −2,5), muka atas pelat B.
  - `WALL_BOT`: offset `wall_bottom_z` (z 2,5), tepi bawah dinding klem.

## Fase 2: load cell acuan (tidak dicetak)

- [ ] Komponen baru `Load_Cell`, aktifkan.
- [ ] **Sketch** pada bidang XY: **Center Rectangle** di titik asal, `LC_L` x `LC_W`. **Extrude** ke atas `LC_H`.
- [ ] **Sketch** pada muka atas: empat titik di (±`hole_x`, ±`hole_y`). **Hole** Ø5, **All** (tembus), sebagai acuan ulir M6.
- [ ] **Sketch** pada muka samping −y: titik di x = `-LC_L / 2 + cable_x`, z = `cable_z`. **Hole** `cable_hole_d`, dalam 5 mm, sebagai acuan lubang kabel.
- [ ] Klik kanan komponen di Browser, **Ground**. Jangan diekspor untuk dicetak.

## Fase 3: Spacer A (ujung tetap, di atas load cell)

- [ ] Komponen baru `Spacer_A`, aktifkan.
- [ ] **Sketch** pada `LC_TOP`: persegi panjang x dari `-LC_L / 2 - sp_end_cl - sp_lip_t` (−68) sampai `-LC_L / 2 + boss_L` (−41), y dari `-spacer_W / 2` sampai `spacer_W / 2` (±17). **Extrude** ke atas `gap` (z 21,5 sampai 24), **New Body**.
- [ ] **Sketch** pada `LC_TOP`, dua pita bibir:
  - bibir ujung: x dari −68 sampai `-LC_L / 2 - sp_end_cl` (−65,5), selebar spacer;
  - bibir sisi +y: y dari `sp_in_W / 2` (14,5) sampai `spacer_W / 2` (17), sepanjang spacer.
- [ ] **Extrude** kedua pita ke bawah `sp_lip_h` (sampai z 19), **Join**.
- [ ] **Sketch** pada `LC_TOP`: pita dinding klem, y dari `-spacer_W / 2` (−17) sampai `-sp_in_W / 2` (−14,5), sepanjang spacer. **Extrude** ke bawah **To Object** bidang `WALL_BOT` (z 2,5), **Join**.
- [ ] **Sketch** pada muka atas Spacer A (z 24): dua titik di (`-hole_x`, ±`hole_y`). **Hole** `hole_sp_d`, **All**, Objects = `Spacer_A` saja.
- [ ] Cek: **Inspect > Measure** muka dalam bibir sisi +y ke muka samping load cell = 0,5; muka dalam dinding klem ke muka samping −y = 0,5; muka dalam bibir ujung ke muka ujung load cell = 0,5; ujung dalam spacer di x −41.

## Fase 4: slot jepit kabel (dinding klem Spacer A)

- [ ] **Sketch** pada muka luar dinding klem (y −17):
  - lingkaran Ø`clamp_hole_d` dengan pusat di x = `-LC_L / 2 + cable_x` (−59,7), z = `cable_z` (10,75);
  - persegi panjang selebar `clamp_mouth_w`, berpusat di x yang sama, dari tepi bawah dinding (z 2,5) sampai pusat lingkaran.
- [ ] **Trim** garis di dalam lingkaran supaya lubang dan mulut menjadi satu profil.
- [ ] **Extrude** profil itu menembus dinding (**Through All** ke arah +y, atau jarak `clamp_wall_t`), **Cut**, Objects = `Spacer_A` saja.
- [ ] Opsional: **Chamfer** 0,4 di kedua tepi mulut pada tepi bawah dinding supaya kabel mudah ditekan masuk.
- [ ] Cek: lubang slot segaris dengan lubang kabel load cell acuan (lihat dari sisi −y); mulut terbuka ke bawah; tepi bawah dinding 5 mm di atas muka atas pelat B.

## Fase 5: Spacer B (ujung bebas, di bawah load cell)

- [ ] Komponen baru `Spacer_B`, aktifkan.
- [ ] **Sketch** pada bidang XY: persegi panjang x dari `LC_L / 2 - boss_L` (41) sampai `LC_L / 2 + sp_end_cl + sp_lip_t` (68), y ±`spacer_W / 2`. **Extrude** ke bawah `gap` (z 0 sampai −2,5), **New Body**.
- [ ] **Sketch** pada bidang XY, tiga pita bibir: bibir ujung x dari `LC_L / 2 + sp_end_cl` (65,5) sampai 68; bibir sisi y dari ±`sp_in_W / 2` sampai ±`spacer_W / 2`, sepanjang spacer. **Extrude** ke atas `sp_lip_h` (z 2,5), **Join**.
- [ ] **Sketch** pada muka bawah Spacer B (z −2,5): dua titik di (`hole_x`, ±`hole_y`). **Hole** `hole_sp_d`, **All**, Objects = `Spacer_B` saja.

## Fase 6: pelat A dan B (aluminium, tidak dicetak)

- [ ] Komponen baru `Bar_A`. **Sketch** pada `A_BOT`: **Center Rectangle** di titik asal, `bar_L` x `bar_W`. **Extrude** ke atas `bar_t`.
- [ ] **Sketch** pada muka atas `Bar_A`: dua titik **hanya** di (`-hole_x`, ±`hole_y`). **Hole** `hole_d`, **All**.
- [ ] Komponen baru `Bar_B`. **Sketch** pada `B_TOP`: **Center Rectangle** `bar_L` x `bar_W`. **Extrude** ke bawah `bar_t`.
- [ ] **Sketch** pada muka bawah `Bar_B`: dua titik **hanya** di (`hole_x`, ±`hole_y`). **Hole** `hole_d`, **All**.
- [ ] Beri material aluminium. Cek massa (klik kanan komponen > **Properties**): tiap pelat sekitar 136 g. Selisih besar berarti ada parameter yang salah.

## Fase 7: ridge dan bagian bebas

- [ ] Rancang ridge di muka luar `Bar_B` sesuai ide tim, di komponen sendiri.
- [ ] Syarat: tidak ada bagian ridge yang mendekati sisi A kurang dari `ab_min_cl`, dan tebal total tetap `total_t` bila `ridge_h` diubah.

## Fase 8: pemeriksaan di Fusion

- [ ] **Inspect > Interference**, pilih semua body. Harapan: tidak ada tumpang tindih (spacer hanya menyentuh muka pelat dan load cell).
- [ ] **Inspect > Measure**:
  - muka bawah `Bar_A` ke muka atas load cell di ujung bebas = 2,5;
  - muka atas `Bar_B` ke muka bawah load cell di ujung tetap = 2,5;
  - tepi bawah dinding klem ke muka atas `Bar_B` = 5 (minimal `ab_min_cl`);
  - jarak `Spacer_A` ke `Bar_B` dan `Spacer_B` ke `Bar_A` ≥ `ab_min_cl`;
  - tebal total dari muka atas `Bar_A` ke dasar ridge = `total_t`.
- [ ] **Inspect > Section Analysis** pada bidang x = −53 dan x = 53: lubang pelat, spacer, dan load cell segaris; bandingkan dengan gambar Spacer A potongan c.
- [ ] Uji parameter: ubah `cable_x` ke 5,0, pastikan slot ikut bergeser; ubah `fit_cl` ke 0,3, pastikan lebar dalam spacer menjadi 29,2; lalu kembalikan.
- [ ] Simpan versi dengan deskripsi.

## Fase 9: ekspor

- [ ] Pastikan nama komponen: `Load_Cell`, `Spacer_A`, `Spacer_B`, `Bar_A`, `Bar_B` (ridge boleh bernama bebas).
- [ ] **File > Export**, format STEP, satu berkas untuk seluruh rakitan.
- [ ] Kirim STEP untuk dicek sebelum mencetak spacer atau membawa gambar pelat ke bengkel.

## Fase 10: cetak uji klem kabel (sebelum mencetak Spacer A penuh)

- [ ] Salin `Spacer_A` ke komponen baru (**Copy**, **Paste New**), lalu **Combine > Intersect** dengan kotak di sekitar dinding klem (misal x −68 sampai −50, y −17 sampai −10, seluruh tinggi).
- [ ] Cetak dengan pengaturan cetakan akhir (PLA, lapisan 0,20, 3 dinding), dinding berdiri.
- [ ] Tekan kabel hitam load cell ke mulut slot dengan jari. Harapan: masuk dengan tekanan ringan, tidak jatuh sendiri, dan tidak bisa ditarik keluar ke samping.
- [ ] Bila terlalu keras: naikkan `clamp_mouth_w` 0,2. Bila kabel lepas sendiri: turunkan `clamp_mouth_w` 0,2. Bila lubang longgar: turunkan `clamp_hole_d` 0,2.

## Aturan cetak yang dipakai

- Lubang bulat PLA memakai `+ comp_hole`; bukaan dan celah pas memakai `comp_xy` dan `fit_cl`. Lubang pelat aluminium dibor tanpa kompensasi.
- Spacer dicetak dengan pelat (muka 27 x 34) di bawah; tanpa support. Overhang ≤ 45°, detail ≥ 0,8 mm, chamfer 0,3-0,5 di tepi bawah.
- Slice review harus disetujui sebelum mencetak.

## Gerbang keputusan

1. Sebelum mencetak klem: karet kabel di badan load cell dicek apakah menonjol (`cable_x` sudah terukur 1 Nov. 2026).
2. Sebelum mengebor pelat: pola lubang dari CSV (12 / 106 / 15, terukur 31 Okt. 2026) dan STEP sudah dicek.
3. Sebelum mencetak: cetak uji klem lolos dan slice review disetujui.
4. Sebelum kalibrasi: ikuti protokol kalibrasi; beban menekan pelat B ke arah pelat A.

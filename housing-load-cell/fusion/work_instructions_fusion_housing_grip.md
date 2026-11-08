# Instruksi kerja: housing load cell di Fusion (v9)

Lingkup: memodelkan load cell acuan, Spacer A dengan dinding klem kabel, Spacer B, serta pelat A dan B. Ridge, balok kalibrasi, dan bentuk tambahan lain dirancang bebas oleh tim. Desain dan alasan: [../desain-housing-load-cell.md](../desain-housing-load-cell.md) dan [../asumsi.md](../asumsi.md); parameter: [fusion_parameters_housing_grip.csv](fusion_parameters_housing_grip.csv) (parameter bertanda WAJIB di kolom keterangan tidak boleh diubah tanpa ukur ulang).

Nama perintah ditulis seperti di menu Fusion versi bahasa Inggris; letaknya bisa sedikit berbeda antarversi. Ketik nama parameter di kotak ukuran, bukan angka.

## Acuan koordinat

Titik asal di tengah load cell, muka bawah load cell di z 0. x sepanjang load cell (ujung tetap, sisi kabel, di x negatif), y melintang (kabel keluar di sisi −y), z ke atas (pelat A di atas, pelat B di bawah).

| Bidang / muka | Nilai |
|---|---|
| Muka ujung load cell | x ±65 (`LC_L / 2`) |
| Muka samping load cell | y ±14 (`LC_W / 2`) |
| Muka atas load cell | z 21,5 (`LC_H`) |
| Muka bawah pelat A | z 24,5 (`LC_H + gap`) |
| Muka atas pelat B | z −3 (`-gap`) |
| Sumbu lubang baut | x ±53 (`hole_x`), y ±7,5 (`hole_y`) |

Hasil akhir yang dituju:

| Bagian | Ukuran |
|---|---|
| Load cell acuan | 130 x 28 x 21,5; 4 lubang di (±53, ±7,5); lubang kabel Ø5 di sisi −y, x −59,7, z 10,75 |
| Spacer A (atas, ujung tetap) | pelat 27 x 34 x 3 di z 21,5 sampai 24,5, x −68 sampai −41; bidang kontak 24 x 28 |
| Bibir Spacer A | 2,5 x 2,5 di muka ujung dan sisi +y, turun ke z 19; lebar dalam 29; bibir ujung 0,5 dari muka ujung load cell |
| Dinding klem Spacer A | sisi −y, tebal 2,5, sepanjang spacer, turun ke z 2,5 |
| Klem kabel | takik 4,6 terbuka ke bawah di x −59,7, puncak bulat di z 13,25; bantalan 12 x 6 di muka luar dinding, z 13,25 sampai 24,5; terowongan zip tie 3,5 x 2,4 tembus arah x, lantai 1,5 |
| Lubang Spacer A | Ø7,6 di (−53, ±7,5) |
| Spacer B (bawah, ujung bebas) | pelat 27 x 34 x 3 di z −3 sampai 0, x 41 sampai 68; bibir di muka ujung dan kedua sisi, naik ke z 2,5; lubang Ø7,6 di (53, ±7,5) |
| Pelat A | 140 x 40 x 8 di z 24,5 sampai 32,5; lubang Ø6,6 hanya di (−53, ±7,5) |
| Pelat B | 140 x 40 x 8 di z −11 sampai −3; lubang Ø6,6 hanya di (53, ±7,5) |

## Fase 1: persiapan

- [ ] Buat desain baru dengan jenis **Hybrid Design**, satuan mm, lalu simpan dengan nama yang jelas.
- [ ] Impor parameter: **Modify > Change Parameters**, klik ikon import (ikon keempat dihitung dari ikon tanda +), pilih `fusion_parameters_housing_grip.csv` dari komputer, **Import**, lalu **OK**.
- [ ] Cek kolom Value: `hole_x` 53; `hole_y` 7,5; `hole_sp_d` 7,6; `sp_in_W` 29; `spacer_L` 27; `spacer_W` 34; `notch_w` 4,6; `notch_top_z` 13,25; `tun_w` 3,5; `bar_hole_edge` 17; `total_t` 48,5.
- [ ] Buat bidang konstruksi (**Construct > Offset Plane**) dari bidang XY (z 0):
  - `LC_TOP`: offset `LC_H` (z 21,5), muka atas load cell.
  - `A_BOT`: offset `LC_H + gap` (z 24,5), muka bawah pelat A.
  - `B_TOP`: offset `-gap` (z −3), muka atas pelat B.
  - `WALL_BOT`: offset `wall_bottom_z` (z 2,5), tepi bawah dinding klem.

## Fase 2: load cell acuan (tidak dicetak)

- [ ] Komponen baru `Load_Cell`, aktifkan.
- [ ] **Sketch** pada bidang XY: **Center Rectangle** di titik asal, `LC_L` x `LC_W`. **Extrude** ke atas `LC_H`.
- [ ] **Sketch** pada muka atas: empat titik di (±`hole_x`, ±`hole_y`). **Hole** Ø5, **All** (tembus), sebagai acuan ulir M6.
- [ ] **Sketch** pada muka samping −y: titik di x = `-LC_L / 2 + cable_x`, z = `cable_z`. **Hole** `cable_hole_d`, dalam 5 mm, sebagai acuan lubang kabel.
- [ ] Klik kanan komponen di Browser, **Ground**. Jangan diekspor untuk dicetak.

## Fase 3: Spacer A (ujung tetap, di atas load cell)

- [ ] Komponen baru `Spacer_A`, aktifkan.
- [ ] **Sketch** pada `LC_TOP`: persegi panjang x dari `-LC_L / 2 - sp_end_cl - sp_lip_t` (−68) sampai `-LC_L / 2 + boss_L` (−41), y dari `-spacer_W / 2` sampai `spacer_W / 2` (±17). **Extrude** ke atas `gap` (z 21,5 sampai 24,5), **New Body**.
- [ ] **Sketch** pada `LC_TOP`, dua pita bibir:
  - bibir ujung: x dari −68 sampai `-LC_L / 2 - sp_end_cl` (−65,5), selebar spacer;
  - bibir sisi +y: y dari `sp_in_W / 2` (14,5) sampai `spacer_W / 2` (17), sepanjang spacer.
- [ ] **Extrude** kedua pita ke bawah `sp_lip_h` (sampai z 19), **Join**.
- [ ] **Sketch** pada `LC_TOP`: pita dinding klem, y dari `-spacer_W / 2` (−17) sampai `-sp_in_W / 2` (−14,5), sepanjang spacer. **Extrude** ke bawah **To Object** bidang `WALL_BOT` (z 2,5), **Join**.
- [ ] **Sketch** pada muka atas Spacer A (z 24,5): dua titik di (`-hole_x`, ±`hole_y`). **Hole** `hole_sp_d`, **All**, Objects = `Spacer_A` saja.
- [ ] Cek: **Inspect > Measure** muka dalam bibir sisi +y ke muka samping load cell = 0,5; muka dalam dinding klem ke muka samping −y = 0,5; muka dalam bibir ujung ke muka ujung load cell = 0,5; ujung dalam spacer di x −41.

## Fase 4: klem kabel zip tie (dinding klem Spacer A)

Kabel duduk di takik yang terbuka ke arah pelat B, lalu diikat zip tie lewat terowongan di bantalan. Takik tidak menjepit; zip tie yang menahan kabel ke segala arah.

- [ ] **Sketch** pada muka luar dinding klem (y −17), profil takik:
  - dua garis tegak selebar `notch_w`, berpusat di x = `-LC_L / 2 + cable_x` (−59,7), dari tepi bawah dinding (z 2,5) ke atas;
  - setengah lingkaran di atasnya, jari-jari `notch_w / 2`, puncaknya di z = `notch_top_z` (13,25).
- [ ] **Extrude** profil takik menembus dinding (**Through All** ke arah +y, atau jarak `clamp_wall_t`), **Cut**, Objects = `Spacer_A` saja.
- [ ] **Sketch** pada muka luar dinding klem: persegi panjang bantalan, x dari `-LC_L / 2 + cable_x - pad_L / 2` sampai `-LC_L / 2 + cable_x + pad_L / 2` (−65,7 sampai −53,7), z dari `notch_top_z` sampai `LC_H + gap` (13,25 sampai 24,5, menyatu dengan pelat spacer).
- [ ] **Extrude** bantalan keluar (arah −y) sejauh `pad_t`, **Join**.
- [ ] **Sketch** pada muka ujung bantalan (x −65,7): persegi panjang terowongan `tun_w` (arah y) x `tun_h` (arah z), berpusat `tun_y` dari muka luar dinding (y −20), tepi bawah di z = `notch_top_z + tun_floor` (14,75).
- [ ] **Extrude** terowongan menembus bantalan sepanjang `pad_L` (**Through All** ke arah +x, Objects = `Spacer_A` saja), **Cut**.
- [ ] Opsional: **Chamfer** 0,4 di kedua tepi takik pada tepi bawah dinding supaya kabel mudah masuk.
- [ ] Cek: puncak takik segaris dengan lubang kabel load cell acuan (lihat dari sisi −y); takik terbuka ke bawah; terowongan tembus di kedua ujung bantalan; tepi bawah dinding 5,5 mm di atas muka atas pelat B.

## Fase 5: Spacer B (ujung bebas, di bawah load cell)

- [ ] Komponen baru `Spacer_B`, aktifkan.
- [ ] **Sketch** pada bidang XY: persegi panjang x dari `LC_L / 2 - boss_L` (41) sampai `LC_L / 2 + sp_end_cl + sp_lip_t` (68), y ±`spacer_W / 2`. **Extrude** ke bawah `gap` (z 0 sampai −3), **New Body**.
- [ ] **Sketch** pada bidang XY, tiga pita bibir: bibir ujung x dari `LC_L / 2 + sp_end_cl` (65,5) sampai 68; bibir sisi y dari ±`sp_in_W / 2` sampai ±`spacer_W / 2`, sepanjang spacer. **Extrude** ke atas `sp_lip_h` (z 2,5), **Join**.
- [ ] **Sketch** pada muka bawah Spacer B (z −3): dua titik di (`hole_x`, ±`hole_y`). **Hole** `hole_sp_d`, **All**, Objects = `Spacer_B` saja.

## Fase 6: pelat A dan B (aluminium, tidak dicetak)

- [ ] Komponen baru `Bar_A`. **Sketch** pada `A_BOT`: **Center Rectangle** di titik asal, `bar_L` x `bar_W`. **Extrude** ke atas `bar_t`.
- [ ] **Sketch** pada muka atas `Bar_A`: dua titik **hanya** di (`-hole_x`, ±`hole_y`). **Hole** `hole_d`, **All**.
- [ ] Komponen baru `Bar_B`. **Sketch** pada `B_TOP`: **Center Rectangle** `bar_L` x `bar_W`. **Extrude** ke bawah `bar_t`.
- [ ] **Sketch** pada muka bawah `Bar_B`: dua titik **hanya** di (`hole_x`, ±`hole_y`). **Hole** `hole_d`, **All**.
- [ ] Beri material aluminium. Cek massa (klik kanan komponen > **Properties**): tiap pelat sekitar 120 g. Selisih besar berarti ada parameter yang salah.

## Fase 7: ridge dan bagian bebas

- [ ] Rancang ridge di muka luar `Bar_B` sesuai ide tim, di komponen sendiri.
- [ ] Syarat: tidak ada bagian ridge yang mendekati sisi A kurang dari `ab_min_cl`, dan tebal total tetap `total_t` bila `ridge_h` diubah.

## Fase 8: pemeriksaan di Fusion

- [ ] **Inspect > Interference**, pilih semua body. Harapan: tidak ada tumpang tindih (spacer hanya menyentuh muka pelat dan load cell).
- [ ] **Inspect > Measure**:
  - muka bawah `Bar_A` ke muka atas load cell di ujung bebas = 3;
  - muka atas `Bar_B` ke muka bawah load cell di ujung tetap = 3;
  - tepi bawah dinding klem ke muka atas `Bar_B` = 5,5 (minimal `ab_min_cl`); bantalan juga jauh dari `Bar_B` dan `Spacer_B`;
  - jarak `Spacer_A` ke `Bar_B` dan `Spacer_B` ke `Bar_A` ≥ `ab_min_cl`;
  - tebal total dari muka atas `Bar_A` ke dasar ridge = `total_t`.
- [ ] **Inspect > Section Analysis** pada bidang x = −53 dan x = 53: lubang pelat, spacer, dan load cell segaris; bandingkan dengan gambar Spacer A potongan c.
- [ ] Uji parameter: ubah `cable_x` ke 5,0, pastikan takik dan bantalan ikut bergeser; ubah `fit_cl` ke 0,3, pastikan lebar dalam spacer menjadi 29,2; lalu kembalikan.
- [ ] Simpan versi dengan deskripsi.

## Fase 9: ekspor

- [ ] Pastikan nama komponen: `Load_Cell`, `Spacer_A`, `Spacer_B`, `Bar_A`, `Bar_B` (ridge boleh bernama bebas).
- [ ] **File > Export**, format STEP, satu berkas untuk seluruh rakitan.
- [ ] Kirim STEP untuk dicek sebelum mencetak spacer atau membawa gambar pelat ke bengkel.

## Fase 10: cetak uji klem kabel (sebelum mencetak Spacer A penuh)

- [ ] Salin `Spacer_A` ke komponen baru (**Copy**, **Paste New**), lalu **Combine > Intersect** dengan kotak di sekitar dinding klem (misal x −68 sampai −50, y −17 sampai −10, seluruh tinggi).
- [ ] Cetak dengan pengaturan cetakan akhir (PLA, lapisan 0,20, 3 dinding), dinding berdiri.
- [ ] Masukkan kabel hitam load cell ke takik dari bawah: harus masuk tanpa dipaksa dan duduk di puncak takik.
- [ ] Masukkan zip tie 2,5 x 100 ke terowongan, lingkarkan di bawah kabel, tarik sampai kabel menempel ke puncak takik, kunci di samping bantalan (tidak mengarah ke pelat B).
- [ ] Tarik kabel ke berbagai arah dengan tangan. Harapan: kabel tidak bergeser di takik dan bantalan tidak retak.
- [ ] Bila zip tie tidak masuk terowongan: naikkan `tun_w` atau `tun_h` 0,2. Bila kabel seret di takik: naikkan `notch_w` 0,2. Bila lantai terowongan retak: naikkan `tun_floor` ke 2.

## Aturan cetak yang dipakai

- Lubang bulat PLA memakai `+ comp_hole`; bukaan dan celah pas memakai `comp_xy` dan `fit_cl`. Lubang pelat aluminium dibor tanpa kompensasi.
- Bantalan zip tie menyatu dengan pelat spacer, jadi saat dicetak (pelat di bawah) tidak ada overhang; atap terowongan adalah jembatan 3,5 (≤ 10). Takik terbuka ke atas saat dicetak.
- Spacer dicetak dengan pelat (muka 27 x 34) di bawah; tanpa support. Overhang ≤ 45°, detail ≥ 0,8 mm, chamfer 0,3-0,5 di tepi bawah.
- Slice review harus disetujui sebelum mencetak.

## Gerbang keputusan

1. Sebelum mencetak klem: posisi kabel dan pangkal kabel sudah dicek 1 Nov. 2026 (`cable_x` 5,3; tanpa karet yang menonjol, kabel keluar dari lubang berchamfer). Klem memakai zip tie 2,5 x 100 (belum dibeli; tidak ada di pesanan baut 31 Okt. 2026); tinggal cetak uji Fase 10.
2. Sebelum mengebor pelat: pola lubang dari CSV (12 / 106 / 15, terukur 31 Okt. 2026) dan STEP sudah dicek. Pelat 8 mm tanpa cadangan: tandai dari spacer tercetak atau gambar 1:1, center punch, bor pilot, lalu Ø6,6.
3. Sebelum mencetak: cetak uji klem lolos dan slice review disetujui.
4. Sebelum kalibrasi: ikuti protokol kalibrasi; beban menekan pelat B ke arah pelat A.

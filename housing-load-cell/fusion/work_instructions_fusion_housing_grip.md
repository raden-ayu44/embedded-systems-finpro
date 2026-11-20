# Instruksi kerja: housing grip di Fusion (v13)

Lingkup: memodelkan load cell acuan, Spacer A (dengan dinding dan klem kabel), Spacer B, dan Clamp_Pad untuk susunan **tekan telapak**: ujung tetap load cell dijepit ke meja dengan klem C, telapak menekan ujung bebas. Pelat aluminium, ridge pelat, dan balok kalibrasi dari versi sebelumnya **tidak dipakai lagi** (progress report 18 Nov. 2026).

Bagian yang sama dipakai untuk kalibrasi dan untuk UAS. Alasan setiap ukuran dan perhitungan gayanya ada di [../../kalibrasi/protokol-kalibrasi-hgd.md](../../kalibrasi/protokol-kalibrasi-hgd.md) (bagian "Ukuran bagian cetak dan alasannya" dan "Perhitungan gaya dan tekanan"). Parameter: [fusion_parameters_housing_grip.csv](fusion_parameters_housing_grip.csv).

Hasil acuan: [load_cell_housing.step](load_cell_housing.step) (20 Nov. 2026) adalah model hasil instruksi ini yang sudah dicek: semua ukuran cocok dan tidak ada tumpang tindih. Bandingkan model Anda dengan berkas ini bila ragu.

Nama perintah ditulis seperti di menu Fusion versi bahasa Inggris. Ukuran ditulis sebagai **angka**, dengan nama parameter CSV di dalam tanda kurung untuk penelusuran. Mengimpor CSV boleh, tetapi tidak wajib.

**Koordinat bukan ukuran sketsa.** Posisi ditulis sebagai koordinat (mis. x −59,7) supaya jelas letaknya. Ukuran sketsa di Fusion adalah **jarak** dan tidak boleh negatif. Di sketsa, beri ukuran jarak positif dari titik asal, sumbu, atau tepi, mis. tepi Spacer A berjarak 68 dari sumbu Y.

**Jangan biarkan garis sketsa menempel (snap) ke tepi load cell.** Muka dalam bibir dan dinding harus 0,5 dari load cell. Tanpa celah ini spacer tidak masuk ke load cell setelah dicetak. Bila terlanjur menempel: **Modify > Press Pull** muka dalam itu sejauh 0,5 menjauh dari load cell.

## Acuan koordinat

Titik asal di tengah load cell, muka bawah load cell di z 0. Sumbu x sepanjang load cell: **ujung tetap** (sisi kabel, dijepit ke meja) di x negatif, **ujung bebas** (ditekan telapak) di x positif. Sumbu y melintang: kabel keluar di sisi −y. Sumbu z ke atas.

**Orientasi:** panah stiker di ujung bebas menghadap **ke bawah** (−z), searah beban. Dengan kabel dan faktor skala yang dipakai sekarang, beban searah panah terbaca positif.

| Bidang / muka | Nilai |
|---|---|
| Muka ujung load cell | x ±65 |
| Muka samping load cell | y ±14 |
| Muka bawah / atas load cell | z 0 / z 21,5 |
| Sumbu lubang baut M6 | x ±53, y ±7,5 |
| Pusat lubang kabel | sisi −y, x −59,7, z 10,75 |

Hasil akhir yang dituju:

| Bagian | Letak (mm) | Bentuk |
|---|---|---|
| Load_Cell (acuan, tidak dicetak) | x −65 sampai 65, y ±14, z 0 sampai 21,5 | 4 lubang Ø5 di (±53, ±7,5); lubang kabel Ø5 di sisi −y |
| Spacer A (di bawah ujung tetap) | x −68 sampai −41, y −23 sampai 17, z −14 sampai 17 | blok 27 x 34 x 14; bibir dan dinding naik ke z 17; kantong kepala baut dari muka bawah; klem kabel di sisi −y |
| Spacer B (di atas ujung bebas) | x 41 sampai 68, y ±17, z 19 sampai 37 | blok 27 x 34 x 14; bibir turun ke z 19; kantong kepala baut dari muka atas; dua ridge penahan sling |
| Clamp_Pad (di atas ujung tetap) | x −68 sampai −41, y ±17, z 19,5 sampai 24,5 | pelat 27 x 34 x 3; bibir turun 2 |

```
   tampak dari sisi −y (FRONT)

                 Clamp_Pad                                   ridge   ridge
   z 24,5     ┌────────────┐                               ┌─┐       ┌─┐   z 37
   z 21,5 ┌───┴────────────┴───────────────────────────────┴─┴───────┴─┴┐ z 21,5
          │                       LOAD CELL                              │
   z 0    └───┬────────────┬─────────────────────────────────────────────┘
              │  Spacer A  │                                Spacer B di atas ujung bebas
   z −14      └────────────┘ ← di atas meja                 (z 21,5 sampai 35,5)
           x −68        x −41                             x 41          x 68
```

## Fase 1: persiapan

- [ ] **Preferences > General > Design > Default modeling orientation: Z up.** Pengaturan ini hanya berlaku untuk desain baru, jadi atur sebelum membuat desain.
- [ ] Buat desain baru (**Hybrid Design**), cek **Document Settings > Units: mm**, lalu simpan dengan nama yang jelas, tanpa nomor versi.
- [ ] Opsional: impor parameter lewat **Modify > Change Parameters**, ikon import, pilih `fusion_parameters_housing_grip.csv`, **Import**, **OK**.

## Fase 2: load cell acuan (tidak dicetak)

- [ ] Komponen baru `Load_Cell`, aktifkan.
- [ ] **Sketch** pada bidang **XY di folder Origin**: **Center Rectangle** di titik asal, 130 x 28 (`LC_L` x `LC_W`). **Extrude** ke atas 21,5 (`LC_H`).
- [ ] **Sketch** pada muka atas: empat titik di (±53, ±7,5) (`hole_x`, `hole_y`). **Hole** Ø5, **All**, sebagai acuan ulir M6.
- [ ] **Sketch** pada muka samping −y: satu titik 5,3 dari tepi muka ujung kiri (`cable_x`, jadi x −59,7) dan 10,75 dari tepi bawah (`cable_z`). **Hole** Ø5, dalam 5, sebagai acuan lubang kabel.
- [ ] Klik kanan komponen, **Ground**. Jangan diekspor untuk dicetak.

## Fase 3: Spacer A (di bawah ujung tetap)

Spacer A ada di antara muka bawah load cell (z 0) dan meja. Tebalnya 14 ke bawah. Bibir dan dinding naik ke atas di samping load cell dan memegang sisinya.

- [ ] Komponen baru `Spacer_A`, aktifkan.
- [ ] **Sketch** pada bidang XY (z 0): persegi panjang x dari −68 sampai −41, y dari −17 sampai 17 (`spacer_L` 27 x `spacer_W` 34). **Extrude ke bawah** 14 (`sp_t`; z 0 sampai −14), **New Body**.
- [ ] **Sketch** pada bidang XY, tiga pita:
  - bibir ujung: x dari −68 sampai −65,5, selebar spacer;
  - bibir sisi +y: y dari 14,5 sampai 17, sepanjang spacer;
  - dinding klem sisi −y: y dari −17 sampai −14,5, sepanjang spacer.
- [ ] **Extrude** ketiga pita **ke atas** sampai z 17, **Join**. Puncak dinding 4,5 di bawah muka atas load cell (`wall_top_gap`), jadi ada celah 2,5 ke bibir Clamp_Pad.
- [ ] **Sketch** pada **muka bawah** Spacer A (z −14): **persegi panjang** kantong kepala baut x dari −60 sampai −46 (lebar 14, `pk_W`), y dari −14,5 sampai 14,5 (panjang 29, `pk_L`). Kantong dan takik dibuat persegi panjang, bukan slot bulat, sesuai aturan cetak.
- [ ] **Extrude** kantong **ke atas** 7 (`pk_D`; z −14 sampai −7), **Cut**, Objects = `Spacer_A` saja.
- [ ] **Sketch** pada lantai kantong (z −7): **Project** (`P`) dua lubang load cell acuan di (−53, ±7,5), taruh titik di pusatnya. **Hole** Ø7,6 (`hole_sp_d`), extent **Distance 7** (sampai z 0). **Jangan All**: lubang load cell acuan ikut membesar. Objects = `Spacer_A` saja.
- [ ] Cek dengan **Inspect > Measure**:
  - muka bawah z −14, lantai kantong z −7, puncak bibir dan dinding z 17;
  - muka dalam bibir ujung x −65,5, muka dalam bibir +y y 14,5, muka dalam dinding y −14,5 (masing-masing 0,5 dari load cell);
  - ujung dalam spacer di x −41 (tidak masuk ke bagian tengah load cell, tempat strain gauge);
  - lubang Spacer A segaris dengan lubang load cell.

## Fase 4: klem kabel (takik, bantalan, terowongan zip tie)

Gunanya: kabel load cell diikat ke Spacer A, jadi tarikan pada kabel berhenti di spacer dan tidak terbaca sebagai gaya. Kabel keluar dari lubang kabel (x −59,7, z 10,75) ke arah −y, lewat **takik** di dinding, lalu berbaring di atas **bantalan**. Zip tie masuk ke **terowongan** di dalam bantalan (tembus arah x), naik di kedua ujung bantalan, lalu melingkar di atas kabel.

```
   tampak dari sisi −y (muka luar dinding klem, y −17)

   z 17   ┌──────────┐    ┌──────────────┐   puncak dinding
          │          │    │              │
          │          │ () │ ← kabel      │   takik 4,6, terbuka ke atas
   z 8,25 │     ┌────┴────┴────┐         │   dasar takik = muka atas bantalan
          │     │ [terowongan] │         │   z 4,35 sampai 6,75
          │     │   BANTALAN   │         │   menonjol 6 ke arah −y
   z −14  └─────┴──────────────┴─────────┘   muka bawah Spacer A
        x −68  −65,7  −59,7  −53,7     x −41
```

- [ ] **Sketch** pada muka luar dinding klem (y −17). **Project** lubang kabel load cell acuan supaya pusatnya terlihat.
- [ ] Takik: persegi panjang x dari −62 sampai −57,4 (lebar 4,6, `notch_w`, berpusat di x −59,7), z dari 8,25 (`notch_bot_z`) sampai puncak dinding (z 17). **Extrude** 2,5 ke arah +y menembus dinding, **Cut**, Objects = `Spacer_A` saja. Dasar takik 0,5 di bawah kabel (kabel Ø4, titik terendahnya z 8,75).
- [ ] **Sketch** pada muka luar dinding klem: persegi panjang bantalan x dari −65,7 sampai −53,7 (panjang 12, `pad_L`, berpusat di garis tengah takik), z dari −14 sampai 8,25. Bantalan dibuat sampai muka bawah spacer supaya tercetak tanpa penyangga.
- [ ] **Extrude** bantalan keluar (arah −y) 6 (`pad_t`; y −17 sampai −23), **Join**.
- [ ] **Sketch** pada muka ujung kiri bantalan (x −65,7): persegi panjang terowongan y dari −21,75 sampai −18,25 (lebar 3,5, `tun_w`, di tengah tebal bantalan, 1,25 dari tiap sisi), z dari 4,35 sampai 6,75 (tinggi 2,4, `tun_h`). Atap terowongan 1,5 di bawah muka atas bantalan (`tun_floor`).
- [ ] **Extrude** terowongan 12 ke arah +x menembus bantalan, **Cut**, Objects = `Spacer_A` saja.
- [ ] Cek: dasar takik dan muka atas bantalan sama-sama di z 8,25; lubang kabel acuan terlihat di tengah takik dari sisi −y; terowongan tembus di kedua ujung bantalan; bantalan tidak menyentuh load cell.

## Fase 5: Spacer B (di atas ujung bebas)

Spacer B adalah tempat telapak menekan saat dipakai dan tempat sling digantung saat kalibrasi.

```
   tampak dari sisi −y, ujung bebas di kanan

   z 37   ┌─┐                  ┌─┐   ← dua ridge 1,5 (penahan sling)
   z 35,5 │ └──────────────────┘ │   ← muka atas Spacer B (telapak menekan di sini)
          │      SPACER B        │      tebal 14
   z 21,5 ├──────────────────────┤   ← muka atas load cell
   z 19   └─┘   (load cell)    └─┘   ← bibir turun 2,5 di kedua sisi dan ujung
        x 41                   x 68
```

- [ ] Komponen baru `Spacer_B`, aktifkan.
- [ ] **Sketch** pada muka atas load cell (z 21,5): persegi panjang x dari 41 sampai 68, y dari −17 sampai 17. **Extrude ke atas** 14 (z 21,5 sampai 35,5), **New Body**.
- [ ] **Sketch** pada muka atas load cell, tiga pita bibir:
  - bibir ujung: x dari 65,5 sampai 68, selebar spacer;
  - bibir sisi −y: y dari −17 sampai −14,5, x 41 sampai 68;
  - bibir sisi +y: y dari 14,5 sampai 17, x 41 sampai 68.
- [ ] **Extrude** ketiga pita **ke bawah** 2,5 (`sp_lip_h`; z 21,5 sampai 19), **Join**.
- [ ] **Sketch** pada **muka atas** Spacer B (z 35,5): persegi panjang kantong x dari 46 sampai 60, y dari −14,5 sampai 14,5. **Extrude ke bawah** 7 (z 35,5 sampai 28,5), **Cut**, Objects = `Spacer_B` saja.
- [ ] **Sketch** pada lantai kantong (z 28,5): **Project** dua lubang load cell acuan di (53, ±7,5). **Hole** Ø7,6, extent **Distance 7** (sampai z 21,5), Objects = `Spacer_B` saja.
- [ ] **Sketch** pada muka atas Spacer B: dua persegi panjang ridge selebar spacer (y −17 sampai 17), x dari 41 sampai 43,5 dan dari 65,5 sampai 68 (`rg_w` 2,5). **Extrude ke atas** 1,5 (`rg_h`; z 35,5 sampai 37), **Join**.
- [ ] Cek: alur di antara ridge 22 (`groove`; sling 17 hanya bisa bergeser ±2,5); ridge tidak menutupi kantong; lubang Spacer B segaris dengan lubang load cell.

## Fase 6: Clamp_Pad (di atas ujung tetap)

Rahang baja klem C menekan Clamp_Pad, bukan aluminium. Bibir membuat pad duduk seperti tutup dan tidak bergeser sebelum klem dikencangkan.

- [ ] Komponen baru `Clamp_Pad`, aktifkan.
- [ ] **Sketch** pada muka atas load cell (z 21,5): persegi panjang x dari −68 sampai −41, y dari −17 sampai 17 (sama dengan garis luar Spacer A). **Extrude ke atas** 3 (`cp_t`; z 21,5 sampai 24,5), **New Body**.
- [ ] **Sketch** pada muka atas load cell, tiga pita bibir: ujung x −68 sampai −65,5; sisi −y y −17 sampai −14,5; sisi +y y 14,5 sampai 17. Tidak ada bibir di sisi dalam (ke arah tengah load cell).
- [ ] **Extrude** ketiga pita **ke bawah 2** (`cp_lip_h`; z 21,5 sampai 19,5), **Join**.
- [ ] **Bibir Clamp_Pad harus tetap pendek (2).** Bibir ini tepat di atas dinding Spacer A (puncak z 17). Bila bibir menumpu dinding, gaya klem lewat pad, dinding, Spacer A, lalu meja, **tanpa lewat load cell**: ujung tetap tidak terjepit, bisa terangkat saat dibebani, dan bacaan bergeser.

## Fase 7: pemeriksaan di Fusion

- [ ] **Inspect > Interference**, pilih semua body. Harapan: tidak ada tumpang tindih. Spacer A, Spacer B, dan Clamp_Pad hanya menyentuh muka load cell.
- [ ] **Inspect > Measure**: bibir Clamp_Pad ke puncak dinding Spacer A = **2,5**.
- [ ] Cek volume (klik kanan body > **Properties**) terhadap model acuan:

  | Body | Volume |
  |---|---|
  | `Spacer_A` | ±14.300 mm³ |
  | `Spacer_B` | ±10.150 mm³ |
  | `Clamp_Pad` | ±3.170 mm³ |

  Selisih lebih dari puluhan mm³ berarti ada fitur yang salah ukuran atau terlewat.
- [ ] Simpan versi dengan deskripsi.

## Fase 8: ekspor

- [ ] Pastikan nama komponen: `Load_Cell`, `Spacer_A`, `Spacer_B`, `Clamp_Pad`.
- [ ] **File > Export**, format STEP, satu berkas untuk seluruh rakitan. Kirim untuk dicek sebelum mencetak.
- [ ] STL untuk dicetak: klik kanan `Spacer_A`, `Spacer_B`, dan `Clamp_Pad` satu per satu, **Save as Mesh**, format STL, satuan mm. `Load_Cell` tidak diekspor.

## Aturan cetak yang dipakai

- PLA, **infill 100 %**: Spacer A menahan gaya terbesar (lihat perhitungan di protokol kalibrasi).
- Lubang bulat PLA memakai kompensasi `comp_hole` (Ø7,6 di model, tercetak ±Ø6,6). Bukaan dan celah pas memakai `comp_xy` dan `fit_cl` (muka dalam 0,5 dari load cell di model).
- Kantong dan takik berbentuk persegi panjang, bukan slot bulat.
- Arah cetak:
  - `Clamp_Pad`: muka atas di bed, bibir menghadap ke atas; tanpa overhang.
  - `Spacer_A`: muka bawah (sisi kantong) di bed; dinding, takik, dan bantalan tumbuh ke atas; atap terowongan adalah jembatan 3,5.
  - `Spacer_B`: pilih di slicer. Bibir di bed: muka yang duduk di load cell menjadi jembatan ±29, dan lendutannya membuat spacer duduk tidak rata. Ridge di bed (terbalik): muka telapak menjadi jembatan ±22 di antara ridge (hanya tampilan), muka yang duduk di load cell bersih, tetapi hanya dua ridge tipis yang menempel ke bed, jadi mungkin perlu brim. Cek di pratinjau slicer.
- Slice review harus disetujui sebelum mencetak.

## Gerbang keputusan

1. Sebelum mencetak: STEP sudah dicek (ukuran dan tumpang tindih), dan tim sudah setuju.
2. Setelah mencetak: ukur tebal Spacer A (14), tebal Spacer B di ridge (15,5 dari muka load cell ke puncak ridge), dan kepala baut tenggelam 0,5-0,7 di kantong. Hasilnya dicatat di Laporan Kalibrasi Tabel 8.
3. Saat merakit: flange bolt M6 x 25 **tanpa ring**, panah stiker menghadap ke bawah, kabel diikat zip tie lewat terowongan. Baut lebih panjang dari 25 atau ditambah ring bisa mentok di muka seberang load cell.
4. Sebelum kalibrasi: ikuti [protokol kalibrasi](../../kalibrasi/protokol-kalibrasi-hgd.md).

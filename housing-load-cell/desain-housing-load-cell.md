# Desain Housing Load Cell (v11)
**Final Project - Embedded Systems Course**

> Dokumen ini memuat seluruh desain housing grip yang berisi load cell: layout dua batang, dampak ke pembacaan gaya, temuan kekakuan batang, kalibrasi, pengadaan pelat, rujukan, dan asumsi gambar skematik (ringkasan di Lampiran A; rinci di [asumsi.md](asumsi.md)). Dipisah dari [catatan proyek](../catatan-proyek-hgd.md) (v39) supaya catatan proyek tidak melebar. Housing elektronik (ESP32, LCD, HX711, tombol, LED) akan ada di dokumen terpisah: [desain-housing-elektronik.md](../housing-elektronik/desain-housing-elektronik.md) (rencana, belum dibuat).

Tautan balik: [catatan proyek](../catatan-proyek-hgd.md) · tabel pengukuran jangka sorong: [tabel pengukuran](../pengukuran/tabel-pengukuran-hgd.xlsx) · spesifikasi S1-S5: [Bagian 2.4 di catatan proyek](../catatan-proyek-hgd.md)

Penomoran: bagian di dokumen ini memakai awalan HL (HL1 sampai HL20) dan Lampiran A, supaya tidak tertukar dengan nomor bagian di catatan proyek. Gambar skematik dan berkas instruksi yang berlaku ada di folder `gambar/` (potongan v11, exploded v12, setup kalibrasi v11, dua tampak klem v1); versi lama (v2 sampai v10) hanya riwayat. Lihat HL12.

## Daftar isi

- [HL1. Keputusan: dua housing terpisah](#hl1-keputusan-dua-housing-terpisah)
- [HL2. Pembanding alat komersial (dokumen pabrikan, bukan peer-review)](#hl2-pembanding-alat-komersial-dokumen-pabrikan-bukan-peer-review)
- [HL3. Pengamatan foto produk (enam gambar rujukan)](#hl3-pengamatan-foto-produk-enam-gambar-rujukan)
- [HL4. Tata letak terpilih: dua batang dengan load cell di antaranya](#hl4-tata-letak-terpilih-dua-batang-dengan-load-cell-di-antaranya)
- [HL5. Dampak housing terhadap pembacaan gaya](#hl5-dampak-housing-terhadap-pembacaan-gaya)
- [HL6. Setup parametrik di Fusion 360](#hl6-setup-parametrik-di-fusion-360)
- [HL7. Aturan cetak dari modul 3D printing](#hl7-aturan-cetak-dari-modul-3d-printing)
- [HL8. Hal yang harus diukur sebelum cetak](#hl8-hal-yang-harus-diukur-sebelum-cetak)
- [HL9. Temuan kekakuan batang dan usulan bahan](#hl9-temuan-kekakuan-batang-dan-usulan-bahan)
- [HL10. Kalibrasi dan arah beban](#hl10-kalibrasi-dan-arah-beban)
- [HL11. Pengadaan pelat logam](#hl11-pengadaan-pelat-logam)
- [HL12. Berkas gambar skematik dan catatan pernyataan AI](#hl12-berkas-gambar-skematik-dan-catatan-pernyataan-ai)
- [HL13. Alternatif dan keputusan yang dibatalkan](#hl13-alternatif-dan-keputusan-yang-dibatalkan)
- [HL14. Urutan prototipe: kardus sampai cetak](#hl14-urutan-prototipe-kardus-sampai-cetak)
- [HL15. Mockup modul 3D printing](#hl15-mockup-modul-3d-printing)
- [HL16. Cowork: cara kerja dan brief awal](#hl16-cowork-cara-kerja-dan-brief-awal)
- [HL17. Rujukan untuk memodel di Fusion](#hl17-rujukan-untuk-memodel-di-fusion)
- [HL18. Sumber rujukan housing (bukan peer-review)](#hl18-sumber-rujukan-housing-bukan-peer-review)
- [HL19. Riwayat koreksi dan audit](#hl19-riwayat-koreksi-dan-audit)
- [HL20. Pertanyaan terbuka untuk dosen atau TA dan konsultasi teknik mesin](#hl20-pertanyaan-terbuka-untuk-dosen-atau-ta-dan-konsultasi-teknik-mesin)
- [Lampiran A. Asumsi gambar skematik housing grip (pointer)](#lampiran-a-asumsi-gambar-skematik-housing-grip)

*Status (v8): layout dan bahan sudah dipilih: pelat aluminium polos 10 mm (aluminium 6061-T6), spacer PLA 28 x 28 x 2,5 mm di tiap ujung load cell, baut M6 x 25 + washer logam. Pelat PLA 7 mm terbukti terlalu lentur (HL9). Load cell adalah CZL 601 80 kg (130 x 28 x 21,5 mm terukur 10 Okt. 2026; gambar penjual 130 x 28 x 22 mm; lihat DS-LC1 di `datasheets/datasheet-library.md`); ukuran ini menggantikan load cell 180 kg (147 x 30 x 22 mm) pada rencana lama. Metode kalibrasi mengikuti protokol dumbbell (HL10). Hasil ukur 10-11 Okt. 2026: 130 x 28 x 21,5 mm, ulir M6 tembus dari kedua muka (diameter dalam 5,0 sampai 5,2 mm, bibir kisi Ø8 mm), kabel Ø4 mm; sisa pengukuran ada di ([tabel pengukuran](../pengukuran/tabel-pengukuran-hgd.xlsx)). Proposal 3.6 hanya menjelaskan housing elektronik; housing grip di bawah ini adalah pengembangan setelah proposal.*

## HL1. Keputusan: dua housing terpisah

- **Housing grip**: hanya load cell, dua batang penjepit, dan jepit kabel. Kecil, ringan, mudah dilap.
- **Housing elektronik**: PCB kustom dengan ESP32, tombol, LED, resistor, header ke HX711 dan LCD, dengan cutout USB dan kabel load cell (proposal 3.6).
- Alasan: elektronik tidak perlu berada di tangan; elektronik terlindung dari keringat dan benturan; dua part kecil lebih mudah dijadwalkan di printer lab (jika satu gagal cetak, yang lain tidak ikut terbuang). Catatan v7: alasan "kabel load cell 110 cm" milik load cell 180 kg lama tidak berlaku lagi. Kabel CZL 601 pendek (sekitar 28 cm menurut listing, 0,42 m menurut GJ Impex; belum diukur; asumsi desain 28 cm, nilai terpendek), jadi housing elektronik harus berada dalam jangkauan kabel itu, atau dipakai kabel sambungan (di luar lingkup dokumen ini).
- Konsekuensi: tombol, LED, dan LCD ada di housing elektronik, bukan di grip, sehingga START dan NEXT ditekan dengan tangan lain atau oleh rekan; LED harus terlihat dari posisi duduk protokol; LCD diletakkan di meja dalam jangkauan kabel (jangkauan kabel load cell sekitar 28 cm, tersisa sekitar 24 cm dari penjepit; kabel sambungan di luar lingkup dokumen ini).
- Pola serupa ada di alat komersial: Biopac dan Vernier memakai grip tanpa elektronik yang tersambung kabel ke unit terpisah (HL18).

## HL2. Pembanding alat komersial (dokumen pabrikan, bukan peer-review)

| Kode | Perangkat | Bentuk dan mekanisme | Rentang | Ukuran dan berat |
|---|---|---|---|---|
| KG1 | Kinvent K-Grip | Silinder, satu tangan, nirkabel | Maks 90 kgF | Tinggi 141 mm, 47 x 61 mm, 170 g |
| GA1 | GripAble | Rangka "C" lebar dan bulat untuk posisi konsisten | Teruji berulang sampai 90 kg | Lingkar 141 mm, 240 g plastik |
| VN1 | Vernier HD-BTA | Strain gauge isometrik, bodi memanjang dengan bantalan di kiri-kanan | 0-600 N (sekitar 61 kgf), aman sampai 850-900 N | Berkabel, tanpa layar |
| BP1 | Biopac SS25LA | Transduser isometrik, batang dengan posisi tangan ditentukan | 0-90 kg, rated 100 kg | Berkabel ke amplifier |
| PP1 | Push-pull aluminium (listing penjual) | Strain gauge, dua tangan | 0-100 kgF | 114 x 217 x 38 mm, 1,1 kg |

Pengamatan: pelat grip kita (170 mm) lebih panjang dari seluruh K-Grip (141 mm) dan load cell-nya sendiri 130 mm, jadi alat kita berada di keluarga transduser lab (Biopac, Vernier, push-pull sekitar 217 mm), bukan alat saku. Prinsip isometrik (sensor nyaris tidak bergerak) menyarankan grip yang kaku dengan defleksi minimal. Diameter dan bentuk grip memengaruhi angka (GripAble membaca sekitar 69% dari Jamar PLUS+), jadi catat diameter grip di laporan dan jangan membandingkan angka mentah dengan norma berbasis Jamar (selaras dengan 13.2).

## HL3. Pengamatan foto produk (enam gambar rujukan)

1. Biopac (diagram sistem MP200): tangan menggenggam tabung tegak, blok sensor di kepala dengan dua lubang bulat; kabel ke amplifier lalu laptop; grafik remasan dengan puncak cukup datar.
2. Biopac batang bercelah: dua batang sejajar (satu lebih tebal, satu lebih tipis dan menonjol), keduanya bercelah panjang; kabel dari ujung bawah. Celah panjang kemungkinan membuat bagian itu melentur terkendali (inferensi). Secara bentuk paling dekat dengan load cell beam kita.
3. Vernier HD-BTA: bodi hitam memanjang dengan bantalan teal di kiri-kanan sebagai titik tekan; kabel dari bawah; tanpa layar.
4. Kemungkinan Kinvent K-Grip: silinder biru tua dengan celah vertikal yang membelah badan jadi dua sisi, tutup datar beraksen oranye di atas. Remasan kemungkinan menutup celah, dengan sensor di antara dua sisi (inferensi).
5. Kemungkinan GripAble: rangka C hijau dengan pelat biru di tengah tempat jari menekan, strip LED di ujung atas dan bawah, tali pergelangan tangan.
6. Dial analog (model Walmart): kepala bulat dengan dial kg, tombol RESET, pegangan berbantalan; tombol RESET setara tare.

Pola: pembaca di kepala dan grip di bawah; kabel keluar dari ujung bawah; dua keluarga mekanisme (batang bercelah dan bodi terbelah atau berbingkai C). Load cell beam kita paling natural di keluarga batang.

## HL4. Tata letak terpilih: dua batang dengan load cell di antaranya

Gambar yang berlaku (skematik dari Cowork, tidak berskala; celah digambar diperbesar; ukuran load cell dari gambar penjual Automa-88; tinggi 21,5 mm terukur) ada di folder `gambar/`: `potongan_memanjang.svg` (potongan dan tampak depan dari sisi kabel), `exploded_isometrik.svg` (dengan stiker panah beban menunjuk ke arah B), `klem_kabel_tampak_depan.svg` dan `klem_kabel_tampak_kiri.svg` (bentuk klem kabel, potongan A-A di x = 4,2). Nomor callout mengikuti legenda di dalam gambar. Gambar versi lama (v4, v5) tidak disertakan di repo ini karena sudah usang.



Telapak menekan A dari satu sisi dan jari menekan B dari sisi berlawanan. Satu-satunya jalur gaya dari A ke B adalah load cell (callout 16).

Ukuran kunci (gambar penjual CZL 601 80 kg): load cell 130 x 28 x 21,5 mm (terukur), empat lubang M6 berulir tembus lurus dari kedua muka, dua per ujung, pusat lubang 12 mm dari ujung, 106 mm antar pasangan, 15 mm melintang (terukur 31 Okt. 2026 dan cocok, [asumsi.md](asumsi.md) Bagian 8.1). Pelat 170 x 30 mm dengan load cell di tengah, jadi tersisa 20 mm pelat di tiap ujung. Spacer 28 mm, bentang bebas antar spacer 74 mm, lengan tuas a = 37 mm, ujung jauh x = 102 mm, zona genggam sekitar 85 mm.

1. (callout 4, 6, 18) Ujung tetap load cell dibaut ke pelat A lewat Spacer A (PLA 28 x 28 x 2,5 mm, terpisah dari pelat). Dua baut M6 x 25 + washer logam menembus pelat dan spacer langsung ke ulir load cell, tanpa mur. Spacer menutup kedua lubang di ujungnya dan membuat celah 2,5 mm di atas sisi bebas.
2. (callout 8 dan 15) Celah antara tiap pelat dan badan load cell harus tetap terbuka pada beban penuh. Pelat melentur 0,24 mm di ujung jauh pada 70 kgf (HL9), jadi sisa celah minimal 2,26 mm sebelum anggaran lenturan load cell (0,5 mm), tinggi penutup strain gauge (1,0 mm), dan margin rakit (0,3 mm) dikurangkan; sisa setelah anggaran sekitar 0,46 mm (HL9). Celah ini bukan stop overload; stop overload belum dirancang.
3. (callout 5 dan 7) Ujung bebas dibaut ke pelat B dari sisi berlawanan lewat Spacer B (sama dengan A). Di ujung tetap, B tidak boleh menyentuh badan load cell.
4. (callout 10 dan 11) Ridge penahan jari dan bidang tumpu kalibrasi (permukaan datar selebar sekitar 22 mm di antara dua ridge) berada di pusat jari di sisi luar B. Kalibrasi memakai balok PLA dengan lidah yang masuk di antara dua ridge (HL10). Titik gantung (eyelet) tidak dipakai karena beban gantung menarik B menjauhi A, kebalikan remasan.
5. (callout 12 sampai 14) Kabel load cell Ø 4 mm (dilaporkan 11 Okt. 2026; cocok dengan dokumen distributor). Ø 4 mm lebih besar daripada celah 2,5 mm, jadi kabel tidak boleh lewat celah. Kabel keluar **menyamping** dari muka samping panjang load cell (130 x 21,5 mm), di dekat ujung berkabel, lewat lubang Ø5 mm di tengah tinggi (13,25 mm di atas permukaan pelat A); kabel Ø4 mm agak longgar di lubang. Pusat lubang 4,2 mm dari ujung load cell (diukur 11 Okt. 2026: tepi lubang ke arah tengah 6,7 mm dari ujung). Ujung berkabel menjadi ujung tetap. Kabel langsung masuk blok penjepit PLA yang **menyatu dengan Spacer A** (satu cetakan; terjepit baut, tanpa lem ke pelat aluminium): rak setebal 2,5 mm di sisi kabel dan blok selebar 8,2 mm setinggi sekitar 17,4 mm dengan lubang Ø4,1 mm; kabel direkat lem tembak di lubang. Setelah blok, kabel melengkung bebas menjauhi genggaman (radius tekuk 10 mm; asumsi). Panjang kabel diasumsikan 28 cm, tersisa sekitar 27 cm dari blok (A4.1).
6. Baut heksagonal ISO 4017 M6 x 25 (kelas 8.8) dengan washer ISO 7089 (Ø12 x 1,6 mm): kepala 4 mm + washer 1,6 mm = menonjol 5,6 mm di luar pelat A dan B, 32 mm dari ujung pelat, di luar zona genggam. Tebal total di bawah belum menghitung kepala baut.

Dua batang harus kaku: satu-satunya bagian yang boleh melentur secara berarti adalah load cell. Persyaratan ini tidak dipenuhi oleh pelat PLA 7 mm (HL9). Berbeda dari referensi batang bercelah (Biopac), celah panjang tidak perlu ditiru di jalur beban; celah atau lubang pada batang hanya di luar jalur beban.

Tebal total grip = 2 x tebal batang + 2 x tebal spacer (celah) + sisi load cell yang searah beban + ridge sekitar 5 mm = 2 x 10 + 2 x 2,5 + 21,5 + 5 = 51,5 mm, tanpa kepala baut (tinggi load cell terukur 21,5 mm). Bila sisi 28 mm yang ternyata searah beban, tambah 6,5 mm. Kisaran 40-48 mm di draf awal berasal dari pelat PLA 7 mm yang ternyata terlalu lentur, dan itu bukan batas dari spesifikasi. Cek kenyamanannya dengan prototipe kardus lebih dulu. Pelat aluminium 8 mm (total sekitar 47,5 mm) dihitung sebagai pembanding dan tidak dipilih (HL9). Arah beban load cell harus tegak lurus panjangnya, sesuai panah di badannya; cek panah itu sebelum menentukan sisi mana yang menghadap telapak.

## HL5. Dampak housing terhadap pembacaan gaya

| Efek | Pengaruh ke bacaan | Cara menangani |
|---|---|---|
| Rasio tuas (jarak tekan jari terhadap titik beban load cell) | Mengubah skala | Terserap di `calibration_factor` selama geometri tetap; kalibrasi pada rakitan jadi |
| Posisi tekan jari bergeser antar remasan | Galat acak (rasio berubah) | Ridge atau alur penahan jari; uji dengan beban di beberapa posisi; tidak dikoreksi di firmware karena posisi jari tidak diketahui sensor |
| Jalur gaya paralel (A dan B bersentuhan atau bergesek, kabel tertarik, baut mengenai bagian lain) | Bacaan terlalu kecil dan histeresis | Celah di semua titik selain spacer; kabel dijepit di sisi tetap (Ø4 mm tidak lewat celah 2,5 mm) |
| Kelenturan batang (dan creep bila PLA) di jalur beban | Batang yang melentur menyentuh load cell (gaya lewat jalur selain elemen ukur, bacaan terlalu kecil) atau patah; creep PLA menambah drift saat tahan 3-5 detik | Batang kaku, usulan pelat aluminium (HL9); cek residual kalibrasi. PETG bukan solusi: modulusnya sebanding dengan PLA |
| Massa bagian bergerak di ujung bebas | Offset berubah bila orientasi berubah setelah tare. Perkiraan kasar: batang PLA 170 x 30 x 7 mm berbobot sekitar 20-45 g; batang aluminium 10 mm sekitar 138 g; baja 8 mm sekitar 320 g. Semuanya sebanding atau jauh di atas tolerance ±36 g di S1 | Tare dan remas di orientasi yang sama; batang B seringan mungkin yang masih kaku |
| Spacer PLA di bawah baut: relaksasi preload (creep) | Baut mengendur, sambungan bergeser, bacaan drift (kompresi spacer saat remasan hanya sekitar 0,0008 mm, jadi bukan masalah kekakuan) | Washer logam di bawah kepala baut; cek kekencangan berkala; alternatif shim aluminium 3 mm |
| Kekakuan rangka mengubah sensitivitas sistem | `calibration_factor` tidak bisa diambil dari nilai generik atau datasheet | Kalibrasi ulang setelah rakit (FP1) |

**Perlu rumus tambahan di firmware?** Tidak, selama housing kaku, geometri tetap, dan kalibrasi dilakukan pada rakitan jadi: konversi linear `(raw - offset) / scale` yang sudah ada cukup (format yang sama dengan kalibrasi pabrik Vernier: slope dan intercept, VN1). Prosedur: kalibrasi dengan dumbbell sampai 42 kg ([protokol](../kalibrasi/protokol-kalibrasi-hgd.md), HL10), fit linear, hitung residual dan RMSE. Bagian 42 sampai 70 kgf adalah ekstrapolasi. Jika residual dalam tolerance, firmware tetap linear. Jika tidak, fit polinomial orde 2 atau tabel piecewise secara offline dan tempel koefisiennya ke firmware. Efek yang paling berbahaya (posisi jari, jalur gaya paralel) tidak bisa diperbaiki dengan matematika dan harus ditangani lewat desain mekanik.

## HL6. Setup parametrik di Fusion 360

1. Buka Change Parameters (Design > Solid > Modify), lalu tambah user parameter lewat Add User Parameter (satu dialog per parameter). Cara cepat: impor [`fusion_parameters_housing_grip.csv`](fusion/fusion_parameters_housing_grip.csv) lewat Import Parameters; format kolom Name, Unit, Expression, Value, Comments, Favorite (contoh format ada di halaman bantuan "Parameters in Fusion"). Tulis satuan di dalam ekspresi (misalnya `130 mm`), pilih satuan mm sejak awal (mengganti satuan parameter yang sudah dipakai bisa bermasalah), dan cek kolom Value setelah memasukkan angka desimal karena pemisah desimal mengikuti pengaturan regional. Parameter (v7, sesuai gambar penjual dan v5): `LC_L` = 130, `LC_W` = 28, `LC_H` = 21,5, `bar_t` = 10 (pelat aluminium, sudah diputuskan), `gap` = 2,5, `boss_L` = 28 (panjang spacer), `lever_arm` = `LC_L / 2 - boss_L` (37 mm), `spA_c` = `-( LC_L / 2 ) + ( boss_L / 2 )` dan `spB_c` = `( LC_L / 2 ) - ( boss_L / 2 )` (posisi pusat spacer, -51 dan +51 mm), `grip_zone` = 85, `hole_edge` = 12 (pusat lubang dari ujung load cell), `hole_pitch` = 106 (antar pasangan lubang, memanjang), `hole_cross` = 15 (antar dua lubang melintang), dan `hole_d` = 6,6 (Ø lubang lewat di pelat; lubang spacer PLA memakai `hole_d` + 0,4 = 7,0 mm).
2. Component `LoadCell_ref`: kotak 130 x 28 x 21,5 mm dengan dua lubang M6 di tiap ujung (pola 2 x 2) memakai parameter di langkah 1. Hanya acuan, tidak dicetak.
3. Component `Bar_A` dan `Bar_B` berupa pelat polos `bar_L` x `bar_W` x `bar_t` (tanpa tonjolan). Component `Spacer_A` dan `Spacer_B` (PLA) terpisah, masing-masing `boss_L` x `LC_W` x `gap`: spacer A di ujung tetap load cell (di atasnya, di bawah pelat A) dan spacer B di ujung bebas (di bawahnya, di atas pelat B).
4. Rakit dengan joint, lalu Inspect > Interference: A dan B tidak boleh bersentuhan satu sama lain, dan tidak boleh menyentuh badan load cell selain lewat spacer.
5. Lubang lewat baut M6 di pelat dan spacer dimodelkan lebih besar dari baut (Ø 6,6 mm di pelat, lubang lewat M6 seri sedang menurut ISO 273; Ø 7,6 mm di spacer PLA = 6,6 mm + `comp_hole` 1 mm, supaya tercetak sekitar Ø6,6 mm (sejak 31 Okt. 2026; sebelumnya Ø7,0); kasus hitung 5,5 dan 6,5 mm tinggal sebagai pembanding, dan Ø 5,5 mm tidak mungkin dipakai untuk baut M6). Aturan modul "M3 menjadi 3,4 mm" hanya berlaku untuk bagian modul. Chamfer 0,3-0,5 mm di tepi bawah spacer; spacer menjadi alas di Z = 0 saat dicetak.
6. Cetak kupon kecil dulu (spacer dengan pola lubang saja) sebelum bagian lain; itu cara termurah memastikan baut dan load cell benar-benar masuk.
7. Bila batang berupa pelat logam, modelnya hanya untuk cek kecocokan dan gambar kerja bengkel; pelat tidak dicetak. Ridge, bidang tumpu, dan jepit kabel tetap bagian cetak PLA.

## HL7. Aturan cetak dari modul 3D printing

- Bahan PLA; printer lab dipakai bergantian; slot dipesan; hanya cetak setelah slice review disetujui; sertifikasi (Lampiran D) wajib selesai paling lambat Minggu 11 dan menjadi syarat G3 (Minggu 12).
- Anggaran modul untuk bagian utama: maksimal 60 menit dan 40 g per minggu. Itu batas modul; anggaran cetak untuk proyek akhir tidak tercatat di sini.
- Desain: kelonggaran 0,2-0,3 mm per sisi; lubang lewat M3 dimodelkan 3,4 mm; dinding minimal 1,2 mm (gunakan sekitar 2 mm di bagian yang menahan beban); overhang maksimal 45 derajat; chamfer 0,3-0,5 mm di tepi bawah.
- Slicer (Bambu Studio): preset proses 0.20mm Standard, dinding 3 untuk part penahan beban, infill 15-20%, support dimatikan bila desain mengikuti aturan 45 derajat.
- Orientasi: part FDM lebih lemah antar-layer daripada searah layer. Spacer PLA dicetak rebah (permukaan 28 x 28 mm di atas plate) sehingga tidak butuh support; bagian PLA yang panjang (jika ada) dicetak rebah supaya layer sejajar panjang bagian. Sumbu lubang baut vertikal.
- Bila batang berupa pelat logam, itu bukan bagian cetak. Bagian yang tetap dicetak PLA: ridge, bidang tumpu atau lapisan genggam, jepit kabel, dan housing elektronik. Cek ke TA apakah grip hibrida masih memenuhi syarat bagian cetak (G3).

## HL8. Hal yang harus diukur sebelum cetak

Lebar (terukur 28 mm, selesai; gambar penjual 28, GJ Impex 30), panjang blok ujung (sekitar 27-28 mm hanya taksiran), jarak pusat lubang ke ujung (12 mm), jarak antar pasangan lubang (106 mm) dan melintang (15 mm) (ketiganya terukur 31 Okt. 2026, cocok), ulir (terukur M6, tembus dari kedua muka; uji dengan baut M6 di tangan masih dikerjakan), arah panah beban (terukur 11 Okt. 2026: searah tebal 21,5 mm), ujung tetap atau bebas secara fisik (ujung berkabel dipakai sebagai ujung tetap), posisi lubang kabel (terukur: Ø5 mm, muka samping, tengah tinggi, pusat 4,2 mm dari ujung), panjang kabel (asumsi desain 28 cm; listing 28 cm, GJ Impex 0,42 m; diameter 4 mm sudah dilaporkan), serta tinggi penutup strain gauge. Daftar lengkap pengukuran jangka sorong ada di [tabel pengukuran](../pengukuran/tabel-pengukuran-hgd.xlsx).

## HL9. Temuan kekakuan batang dan usulan bahan

*Status (v10): pelat aluminium 10 mm diputuskan (aluminium 6061-T6); angka di bawah dari `asumsi.md` (ringkasan di Lampiran A). Hitungan ini kasar untuk arah desain, bukan verifikasi.*

Masalah: tiap batang bekerja sebagai kantilever dari spacernya. Batang A hanya ditopang di ujung tetap load cell dan batang B hanya di ujung bebas, jadi beban genggam melenturkan batang sepanjang lengan tuas. Dua akibat yang harus dicegah: (1) batang menyentuh badan load cell, sehingga sebagian gaya lewat jalur selain elemen ukur dan bacaan terlalu rendah; (2) batang patah atau meluluh.

Model (teori balok kasar). F = 686 N (70 kgf, batas atas S1), lebar batang b = 30 mm, I = b·h³/12, E aluminium 70 GPa, baja 200 GPa, PLA sekitar 3 GPa:
- Lenturan di titik beban: δ = F·a³ / (3·E·I), a = jarak pusat beban dari tepi dalam spacer.
- Lenturan di titik x ≥ a (ujung batang yang berada di atas ujung load cell sebelahnya): δ(x) = F·a²·(3x - a) / (6·E·I). Nilai ini yang menentukan sentuhan, bukan lenturan di titik beban.
- Tegangan maksimum di pangkal: σ = F·a·(h/2) / I. Momen pangkal = F x a, jadi selama pusat beban tetap, penyebaran beban di zona genggam tidak menurunkannya.
- Penampang berlubang: σ_net = σ · b / (b - n·d) untuk n lubang berdiameter d sejajar lebar (konservatif, karena lubang berada 16 mm di dalam spacer, bukan di tepi dalamnya).
- Geometri v7: a = 130/2 - 28 = 37 mm (pusat zona genggam sekitar 85 mm, di tengah load cell), x = 130 - 28 = 102 mm, bentang bebas 74 mm. Spacer dan baut dianggap kaku, beban terpusat di tengah load cell (konservatif untuk momen, karena zona genggam 85 mm sebagian jatuh di atas spacer).

Hasil (F = 686 N, lebar 30 mm, celah 2,5 mm):

| Pelat | boss_L | a / x | Lenturan titik beban | Lenturan ujung jauh | Sisa celah 2,5 mm* | Tegangan pangkal | Tegangan penampang berlubang (2 lubang Ø5,5 / Ø6,5 / Ø6,6 mm; Ø6,6 dipakai) |
|---|---|---|---|---|---|---|---|
| **Aluminium 10 mm (dipilih)** | **28 mm** | **37 / 102 mm** | **0,066 mm** | **0,241 mm** | **minimal 2,26 mm** | **50,8 MPa** | **80 / 90 / 91 MPa** |
| Aluminium 8 mm (pembanding) | 28 mm | 37 / 102 mm | 0,129 mm | 0,470 mm | minimal 2,03 mm | 79,3 MPa | 125 / 140 / 142 MPa |
| Aluminium 10 mm | 24 mm | 41 / 106 mm | 0,090 mm | 0,304 mm | minimal 2,20 mm | 56,3 MPa | 89 / 99 / 100,5 MPa |
| Aluminium 8 mm | 24 mm | 41 / 106 mm | 0,176 mm | 0,594 mm | minimal 1,91 mm | 87,9 MPa | 139 / 155 / 157 MPa |

*Sisa celah = 2,5 mm dikurangi lenturan ujung jauh, sebelum lenturan load cell, tinggi penutup strain gauge, dan margin rakit diketahui. Baris boss_L 24 mm hanya untuk melihat kepekaan bila blok ujung ternyata lebih pendek dari 28 mm. Kasus Ø5,5 mm tidak mungkin dipakai untuk baut M6 (diameter nominal 6 mm); kasus Ø6,5 lebih realistis, dan dengan Ø7,0 mm tegangan penampang berlubang Al 10 / boss 28 sekitar 95 MPa. Diameter yang dipilih adalah Ø6,6 mm di pelat (ISO 273 seri sedang): Al 10 / boss 28 sekitar 91 MPa.

Pembacaan:
- Aluminium 10 mm: lenturan ujung jauh 0,24 sampai 0,30 mm, jadi celah 2,5 mm tidak terlampaui oleh lenturan pelat. Tegangan penampang berlubang 80 sampai 100,5 MPa (dengan Ø6,6 mm yang dipilih: 91 MPa untuk boss 28 dan 100,5 MPa untuk boss 24), jauh di bawah kuat luluh aluminium 6061-T6 (minimum sekitar 240 MPa), faktor aman sekitar 2,6 pada boss 28 dan 2,4 pada boss 24.
- Aluminium 8 mm juga lolos di atas kertas dengan spacer 28 mm, tetapi marginnya lebih tipis (125 sampai 155 MPa) dan tidak dipilih; keputusan pelat 10 mm sudah diambil (tebal total sekitar 51,5 mm).
- Dibanding v4 (tonjolan menyatu, a = 60 mm): lenturan ujung jauh turun dari 0,80 mm menjadi 0,24 mm karena a turun dari 60 ke 37 mm.
- Pelat PLA tetap gagal jauh (hitungan di riwayat di bawah).

Anggaran celah 2,5 mm (Al 10, boss 28):

| Komponen | Nilai |
|---|---|
| Lenturan ujung jauh pelat | 0,24 mm |
| Lenturan load cell sendiri pada 70 kgf | 0,5 mm (anggaran asumsi; tidak ada di spesifikasi, ukur saat uji) |
| Tinggi penutup strain gauge di atas permukaan | 1,0 mm (anggaran asumsi; ukur) |
| Margin rakit (kerataan pelat, spacer, baut) | 0,3 mm (anggaran asumsi) |
| **Jumlah anggaran** | **2,04 mm (0,24 + 0,5 + 1,0 + 0,3)** |
| **Sisa celah setelah anggaran** | **sekitar 0,46 mm (boss 24 mm: sekitar 0,40 mm)** |

Pemeriksaan spacer PLA dan baut:
- Kompresi spacer (E PLA sekitar 3 GPa, 686 N, tebal 2,5 mm): sekitar 0,0008 mm untuk spacer 28 x 28 dengan 2 lubang Ø7,0 (luas bersih 707,0 mm2, tekanan rata-rata 0,97 MPa); 0,0009 mm bila dihitung dengan 4 lubang (630,1 mm2). Dapat diabaikan terhadap celah.
- Tekanan tepi bila seluruh momen ditahan kontak spacer: M = F·a = 25 382 N·mm, S = 28·28²/6 = 3 659 mm³, jadi sekitar 6,9 MPa ditambah tekanan rata-rata sekitar 1 MPa, sekitar 7,9 MPa. Angka ini hanya berlaku bila preload baut cukup menjaga seluruh permukaan spacer tetap tertekan. Baut hanya 12 mm dari ujung (16 mm dari tepi dalam spacer), jadi tanpa preload pelat cenderung berungkit di tepi dalam: tarik baut sekitar 0,8 kN per baut dan gaya kontak sekitar 2,3 kN di tepi dalam. Tanpa preload, tekanan lokalnya bisa jauh di atas 7,9 MPa. Dengan torsi 3 N·m (K = 0,2; asumsi desain) preload tiap baut sekitar 2,5 kN, sekitar 3 kali tarik kerja 0,8 kN, jadi pelat tidak berungkit. Tekanan rata-rata spacer akibat preload sekitar 7 MPa (5 kN per ujung / 707 mm2), sehingga tekanan tepi gabungan sekitar 14 MPa (kasar): jauh di bawah kuat tekan PLA, tetapi cukup tinggi untuk creep jangka panjang.
- Risiko yang lebih nyata: relaksasi preload karena creep PLA. Washer logam di bawah kepala baut mengurangi tekanan di bawah kepala, tetapi tidak mencegah creep spacer itu sendiri; cek ulang torsi 3 N·m sebelum tiap sesi kalibrasi dan setelah 10 sesi pengukuran (beri tanda cat pada kepala baut untuk melihat putaran). Alternatif bila ragu: shim aluminium 3 mm (celah menjadi 3 mm, total tebal naik 1 mm), dibor bersama pelat.
- Baut M6 x 25: panjang ulir masuk sekitar 25 - washer 1,6 - pelat 10 - spacer 2,5 = 10,9 mm (sekitar 1,8 x d). Ujung baut tidak menembus muka seberang load cell (tinggi terukur 21,5 mm). Lubang berulir tembus lurus dari kedua muka (dilaporkan 11 Okt. 2026), jadi baut tidak mentok di dasar lubang; ulir tersedia sepanjang tinggi load cell, 21,5 mm.

Yang belum diketahui dan mengurangi margin: tinggi penutup strain gauge, lenturan load cell sendiri pada 70 kgf (ujung bebasnya naik mendekati batang), diameter lubang, kekakuan spacer dan sambungan baut, dan grade aluminium. Proteksi overload membutuhkan fitur terpisah yang belum dirancang: pelat sudah melentur 0,24 mm ke arah load cell pada 70 kgf, jadi celah tidak bisa sekaligus menjadi stop overload. Batas overload load cell menurut dokumen pabrikan: 120% (96 kgf) dan 150% (120 kgf), dari kapasitas 80 kg (DS-LC1 di `datasheets/datasheet-library.md`); 70 kgf adalah 87,5% kapasitas.

Pilihan alternatif yang tidak lolos: PLA setebal sekitar 18 mm (genggaman terlalu lebar); memperpendek lengan tuas (zona genggam sekitar 85 mm tidak muat). Proposal (risiko enclosure) menyebut fallback "PETG atau bracket logam": PETG tidak menyelesaikan masalah karena modulusnya sebanding dengan PLA; yang relevan adalah bracket atau pelat logam. Validasi model lentur dengan uji sederhana: pelat PLA 30 x 7 mm dijepit satu ujung, beban 5 kg di jarak 120 mm; prediksi sekitar 11 mm, dan bila hasil jauh berbeda maka nilai E perlu diperbaiki.

Bahan alternatif untuk batang (tebal minimum agar lenturan ujung jauh tidak lebih dari 1 mm pada 686 N dengan a = 37 mm dan x = 102 mm; model sama dengan di atas, dihitung ulang di v7; modulus nilai umum yang bervariasi antar produk; kekuatan bahan non-logam tidak diperiksa):

| Bahan | Modulus (kira-kira) | Tebal minimum | Cocok? |
|---|---|---|---|
| PLA | 3 GPa | sekitar 17,8 mm | Tidak |
| Multipleks/kayu | 6-12 GPa | sekitar 11-14 mm | Tidak (juga lembap dan creep) |
| Fiberglass epoksi (FR4/G10) | 18-24 GPa | sekitar 9-10 mm | Kekakuan mendekati, tetapi tidak lebih tipis dari aluminium, lebih mahal, dan debu bor berbahaya; kekuatan tidak diperiksa |
| Aluminium | 70 GPa | sekitar 6,2 mm | Ya, 8 atau 10 mm (dipilih 10 mm) |
| Baja | 200 GPa | sekitar 4,4 mm | Ya, tetapi margin tegangan penampang berlubang tipis pada baja lunak |

Kesimpulan: bahan non-logam tidak lebih baik dari aluminium pada tebal yang tersedia; yang tersisa adalah aluminium atau baja, dengan cara pengadaan yang berbeda (HL11). Pilihan yang tidak disarankan: PLA dengan inti logam di dalam (beban dan momen di pangkal harus lewat PLA di sekitar baut), dan menurunkan beban desain S1 (mengubah spesifikasi di proposal).

### Riwayat hitungan (tidak berlaku untuk desain terbaru)

Blok di bawah dipertahankan sebagai riwayat dan penjelasan metode. Angkanya memakai load cell 147 mm yang tidak jadi dipakai.

**Spacer PLA terpisah 30 x 30 mm (hitungan lama sebelum v5; load cell 147 mm, a = 43,5 mm, x = 117 mm):**

| Pelat | Lenturan titik beban | Lenturan ujung jauh | Sisa celah 2,5 mm | Tegangan pangkal | Tegangan penampang berlubang (2 lubang Ø5,5 / Ø6,5 mm) | Berat per batang | Tebal total |
|---|---|---|---|---|---|---|---|
| Aluminium 8 mm | 0,21 mm | 0,74 mm | minimal 1,76 mm | 93 MPa | 147 / 165 MPa | 110 g | 48 mm |
| Aluminium 10 mm | 0,11 mm | 0,38 mm | minimal 2,12 mm | 60 MPa | 94 / 105 MPa | 138 g | 52 mm |
| Baja 6 mm | 0,17 mm | 0,62 mm | minimal 1,88 mm | 166 MPa | 262 / 293 MPa | 240 g | 44 mm |
| Baja 8 mm | 0,07 mm | 0,26 mm | minimal 2,24 mm | 93 MPa | 147 / 165 MPa | 320 g | 48 mm |
| PLA 7 mm | 7,3 mm | 25,9 mm | tidak lolos | 122 MPa | 192 / 215 MPa | 44 g | 46 mm |

**Desain awal dengan tonjolan menyatu (a = 60 mm, x = 133 mm, celah 1,5 mm):** pelat PLA 7 mm (kantilever 120 mm, beban merata) melentur sekitar 17 mm pada 200 N, 34 mm pada 400 N, dan 58 mm pada 686 N (tegangan sekitar 168 MPa); agar lenturan di titik beban di bawah 0,5 mm pada 686 N, PLA pejal perlu tebal sekitar 24 mm. Aluminium 8 mm melentur 1,56 mm di ujung jauh dan gagal di celah 1,5 mm; aluminium 10 mm 0,80 mm (tegangan penampang berlubang sekitar 145 MPa); baja 6 mm 1,29 mm (sekitar 404 MPa, melewati luluh baja lunak); baja 8 mm 0,55 mm (sekitar 227 MPa, hampir sama dengan luluh baja lunak sekitar 235 MPa, dan 640 g untuk dua batang).

## HL10. Kalibrasi dan arah beban

**Tahap kalibrasi.** Kalibrasi dikerjakan dalam dua tahap. Tahap 1 sampai 20 kg (dumbbell tunggal 1 sampai 6 kg, pasangan 6 kg = 12 kg, lalu pasangan 4 kg di atas 6 kg = 20 kg, tumpukan sekitar 19 cm) dikerjakan lebih dulu dan digambar di `img/kalibrasi_setup_v5_20kg.svg`. Tahap 2 sampai 42 kg menyusul dengan susunan di bawah. 20 kg baru sekitar 29 persen dari 70 kgf dan baru menjangkau batas bawah rentang alat (20 sampai 70 kgf), sehingga sisanya ekstrapolasi.

*Diperbarui di v7: metode kalibrasi mengikuti [protokol-kalibrasi-hgd.md](../kalibrasi/protokol-kalibrasi-hgd.md) (dumbbell berpasangan, papan kayu, balok PLA dengan lidah). Setup lama (rakitan dibalik, A dijepit ke meja, beban acuan 20/45/70 kg ditumpuk di bidang tumpu; `kalibrasi_setup_v4.svg`) digantikan dan hanya dipertahankan sebagai riwayat. Protokol belum diuji.*

**Arah beban.** Remasan mendorong B ke arah A. Beban yang digantung pada eyelet di sisi luar B menarik B menjauhi A, jadi arahnya kebalikan remasan dan tidak menguji kontak celah pada beban tinggi. Karena itu kalibrasi dilakukan dalam arah remasan: A di bawah di atas alas karet antiselip, B di atas, dan beban menekan B ke arah A. Gaya mengalir: beban, papan, balok PLA, ridge, B, load cell, A, meja.

![Setup kalibrasi: beban menekan batang B ke arah A (arah remasan)](../kalibrasi/gambar/kalibrasi_protokol.svg)

**Metode (ringkas; langkah rinci ada di protokol):**
1. Alat: rakitan grip lengkap yang terhubung ke HX711 dan ESP32 final, alas karet antiselip, papan kayu sekitar 480 x 350 mm tebal 12 mm, balok PLA dengan lidah yang masuk di antara dua ridge (lebar lidah sekitar 21 mm; bidang tumpu di antara ridge sekitar 22 mm; balok tidak boleh menyentuh puncak ridge), dua pemandu kayu, dan dumbbell berpasangan 1, 2, 3, 4, 5, 6 kg (total 42 kg).
2. Timbang tiap dumbbell (12 buah) dengan timbangan yang sama; label massa biasanya meleset beberapa persen.
3. Seri A: dumbbell tunggal 1 sampai 6 kg di tengah papan, satu per satu, dengan papan kosong dan catatan nol di antaranya.
4. Seri B: pasangan ditumpuk, terberat dulu (12, 22, 30, 36, 40, 42 kg), lalu diturunkan dalam urutan terbalik; masing-masing seri minimal dua kali, pembacaan rata-rata 10 sampel setelah 10 detik.
5. Hasil: faktor skala (kemiringan, counts per kg), linearitas (R-kuadrat dan sisa terbesar), histeresis (naik lawan turun), drift (nol awal lawan nol akhir); verifikasi dengan massa yang tidak dipakai untuk fitting.

**Yang berubah dari setup lama:**

| Aspek | Setup lama (v4) | Protokol dumbbell (v7) |
|---|---|---|
| Beban | acuan 20 / 45 / 70 kg ditumpuk | dumbbell sampai 42 kg |
| A | dijepit ke meja (titik jepit tidak pernah ditetapkan; dibatalkan) | di atas alas karet antiselip, tidak dijepit (tepi meja lab tipis dan ada yang gompal) |
| Titik tekan | bidang tumpu sekitar 22 mm, tumpukan di atas alas kecil | lidah balok PLA di antara dua ridge, papan kayu menyebarkan beban, pemandu kayu mencegah tumpukan jatuh |
| 70 kgf | diuji langsung | ekstrapolasi dari 42 kg |

**Keterbatasan dan hal yang belum terjawab:**
1. Kalibrasi hanya sampai 42 kg (60% dari 70 kgf). Nilai di atas itu adalah ekstrapolasi, dan bagian yang paling kritis secara mekanis (lenturan pelat sekitar 0,24 mm, kemungkinan kontak celah) tidak teruji. Catat di laporan; cara menguji sampai 70 kgf (misalnya pembanding dengan timbangan referensi, FP1) belum dirancang. 42 kg adalah sekitar 52% kapasitas load cell 80 kg, di dalam batas overload.
2. Kepala baut dan washer menonjol di sisi luar pelat A (32 mm dari ujung, tinggi tonjolan 5,6 mm: washer ISO 7089 1,6 mm + kepala heksagonal ISO 4017 M6 4,0 mm; nilai standar, belum diukur pada baut yang dibeli). Dengan A di bawah, A bertumpu pada kepala baut di ujung tetap dan tidak rata terhadap alas. Gambar protokol belum menunjukkan ini. Keputusan: alas karet antiselip setebal 6 mm (lebih tebal dari tonjolan 5,6 mm) dengan satu lubang lonjong 29 x 14 mm di bawah kedua kepala baut ujung tetap (washer Ø12 mm + kelonggaran 2 mm; jarak baut 15 mm), sehingga A rata terhadap alas. Ini inferensi dari gambar v5, bukan hasil pengukuran.
3. A bertumpu pada alas, sehingga lenturan A saat digenggam tidak teruji; yang teruji hanya jalur beban lewat load cell.
4. Tumpukan penuh 42 kg tingginya sekitar 25 sampai 28 cm di atas papan dengan tumpuan lidah sekitar 21 mm, jadi goyah (peringatan di protokol). Langkah keselamatan protokol berlaku: turunkan pelan, jangan taruh tangan di bawah tumpukan, berhenti bila tumpukan miring.
5. Soal orientasi: kemiringan faktor skala (slope) tidak bergantung orientasi pada pendekatan pertama, sedangkan offset berubah sekitar massa B (sekitar 138 g untuk aluminium 10 mm). Karena auto-tare terjadi di SIAP pada tiap percobaan, kalibrasi dalam orientasi terbalik tetap berlaku untuk slope; yang perlu dijaga adalah orientasi alat antara tare dan remasan.
6. Kalibrasi memakai beban statis di titik tengah, sedangkan genggaman nyata memberi beban di empat jari dengan posisi sedikit berbeda (HL5).

## HL11. Pengadaan pelat logam

- Kata kunci marketplace (listing Tokopedia yang ditemukan memakai ejaan beragam): `plat strip aluminium 10mm`, `plat almunium tebal 10mm lebar 30mm`, `aluminium flat bar 30x10`, tambahkan `potong custom`; untuk jasa: `jasa potong plat aluminium`, `jasa bor plat`. Untuk baja (listing belum diverifikasi): `besi strip`, `plat strip besi 30x6`, `plat besi 6mm potong`. Ukuran stok yang terlihat berlebar 40 mm; lebar 30 mm mungkin perlu dipotong.
- Minta ukuran 170 x 30 x 10 mm (tebal 10 mm sudah diputuskan), 2 pcs plus 1 cadangan, paduan 6061-T6 (alternatif 5052-H32; grade dicantumkan di listing), tepi di-deburr, tanpa lubang (dibor setelah jarak lubang load cell terukur; rencana: 12 mm dari ujung, 106 mm antar pasangan, 15 mm melintang, Ø6,6 mm).
- Anggaran: BOM proposal Rp253.875 sebelum ongkir, sisa sekitar Rp46.125 dari batas Rp300.000; harga plat dan jasa belum diverifikasi. Tambahkan ke BOM dan simpan bukti beli.
- Tebal pelat sudah diputuskan (10 mm). Tahan pengeboran sampai lubang load cell terukur; pelat polos tanpa lubang bisa dibeli lebih dulu.
- Cek listing strip aluminium (gambar promosi bukan spesifikasi): lebar yang tersedia untuk tebal 10 mm (butuh 30 mm; 35-40 mm masih mungkin dengan penyesuaian gambar), panjang dan apakah bisa dipotong 170 mm (stok per batang panjang bisa melewati sisa anggaran), serta grade dan temper paduan (misalnya 6061-T6 atau 5052-H32). Jika penjual tidak bisa menyebutkan grade, anggap paduannya lunak (luluh puluhan sampai sekitar 100 MPa) dan hitung ulang atau cari penjual yang mencantumkan grade.
- Penjual strip umumnya hanya menjual stok atau potong panjang; pengeboran dikerjakan di tempat lain. Opsi: bengkel bubut atau jasa bor (kata kunci `bengkel bubut`, `jasa bor plat aluminium`, `jasa CNC milling aluminium`); bengkel atau lab kampus dengan bor duduk; atau bor sendiri dengan templat bor PLA yang lubangnya sesuai hasil ukur. Jasa CNC atau laser dengan file gambar kemungkinan melewati anggaran; minta penawaran dulu.
- Untuk pengeboran: diameter lubang pelat 6,6 mm (0,6 mm lebih besar dari baut); hindari counterbore di pangkal pelat 10 mm (menipiskan bagian yang menahan momen terbesar); jangan mengebor menembus lubang berulir load cell, tandai posisi dari lubangnya.
- Rencana: beli strip sudah dipotong 170 mm tanpa lubang, bor setelah ukuran lubang load cell pasti, bawa gambar kerja ke bengkel. Lembar gambar kerja satu halaman (posisi dan diameter lubang, ukuran pelat, grade, toleransi) disiapkan setelah ukuran terukur.
- Dengan spacer PLA terpisah, yang dibeli adalah strip polos tanpa tonjolan, jadi tidak perlu frais. Spacer dicetak (28 x 28 x 2,5 mm, dua per rakitan); pelat dan spacer dibor bersama (templat bor PLA atau dijepit bersama) setelah ukuran lubang load cell diketahui.

## HL12. Berkas gambar skematik dan catatan pernyataan AI

- Gambar yang berlaku (folder `gambar/`): `potongan_memanjang.svg`, `exploded_isometrik.svg`, `kalibrasi_setup.svg`, `klem_kabel_tampak_depan.svg`, `klem_kabel_tampak_kiri.svg`. Semua dibuat dengan Claude (Cowork) dari instruksi tertulis. Gambar v2 sampai v10 tetap ada di Project sebagai riwayat dan tidak dipakai untuk memodel. `kalibrasi_protokol.svg` (HL10) ada di `img/`.
- Instruksi untuk Cowork digabung pada 12 Okt. 2026 menjadi satu berkas koreksi dan satu berkas tugas dua gambar klem; keduanya tidak disertakan di repo ini. Asumsi digabung menjadi `asumsi.md` (v2 sampai v4 dilipat ke Bagian 9). Parameter Fusion: `fusion_parameters_housing_grip.csv` (tidak ada CSV v9 sampai v12).
- Untuk Pernyataan Penggunaan AI (Lampiran C), kolom "anything the tool got wrong" yang ditekankan panduan tugas: (1) layout awal mengasumsikan batang kaku tanpa menghitung kekakuan, ditemukan lewat pemeriksaan asumsi Cowork; (2) instruksi koreksi menyatakan penyebaran beban menurunkan tegangan baja di pangkal, salah karena momen pangkal ditentukan pusat beban; (3) titik gantung kalibrasi arahnya kebalikan remasan; (4) hitungan awal hanya melaporkan lenturan di titik beban, bukan di ujung jauh yang menentukan sentuhan; (5) instruksi koreksi v3 ke v4 memuat dua instruksi yang bertentangan (catatan "stop overload belum dirancang" wajib ada, tetapi kata itu juga dilarang muncul), ditemukan oleh Cowork yang lalu mengikuti instruksi pertama; (6) ukuran load cell 147 x 30 mm dari rencana lama (load cell 180 kg) sempat terbawa ke instruksi dan dokumen sampai gambar penjual CZL 601 tersedia; (7) instruksi memuat satu nilai acuan yang tidak konsisten (lenturan titik beban Al 8 mm, boss 24 mm: tertulis 0,16 mm, hitungan 0,176 mm), kasus lubang Ø5,5 mm yang tidak mungkin dipakai untuk baut M6, dan "empat lubang" per spacer padahal tiap spacer hanya menutup dua lubang; ketiganya ditemukan Cowork saat menghitung ulang; (8) dalam peninjauan, penjepit kabel sempat ditulis berada di Spacer A, padahal terpasang di pelat A.

## HL13. Alternatif dan keputusan yang dibatalkan

- **Kantilever tunggal** (load cell sendirian, satu ujung tetap, pegangan di ujung bebas): konsep awal. Posisi tekan tangan mengubah momen sehingga bacaan bergantung titik genggam, dan bentuk pegangan belum jelas. Digantikan layout dua batang (HL4), yang memberi satu jalur gaya dan posisi tangan yang bisa dikendalikan dengan ridge.
- **Dua batang paralel dengan load cell berdampingan** (usulan pengguna forum, FP1): lebih tidak peka terhadap posisi genggam, tetapi butuh beberapa elemen sejajar dengan presisi, lebih rumit dicetak dan dirakit. Tidak dipilih untuk cakupan proyek, dan belum dicoba.
- **Sensor sentuh kapasitif TTP223 di titik tumpu ibu jari** untuk memvalidasi posisi jari sebagai syarat mulai: dibatalkan oleh tim, alasan tidak dicatat. Pertimbangan yang sempat muncul: logika deteksinya tersembunyi di dalam modul sehingga kurang bisa dijelaskan saat penilaian, dan sensor ini tidak menambah cakupan Sub-CPMK. Akibatnya teknik genggam hanya dijaga lewat instruksi verbal ([Bagian 2.5 di catatan proyek](../catatan-proyek-hgd.md)).
- **Pelat PLA 7 mm** sebagai batang (HL9), **celah sebagai stop overload** (HL9), dan **titik gantung kalibrasi** (HL10): dibatalkan.
- **Elektronik di dalam pegangan** (tombol, LED, LCD di grip): dibatalkan, sejak keputusan dua housing terpisah (HL1).
- **Tonjolan menyatu dengan pelat** (sekitar 13,5 mm): diganti spacer PLA terpisah. Alasan: tonjolan 2,5 mm pada pelat aluminium sulit dibuat tanpa frais, dan tonjolan 13,5 mm terlalu pendek untuk menutup lubang yang pusatnya 12 mm dari ujung load cell. Ukuran spacer berubah dari 30 x 30 x 2,5 mm (load cell 147 mm lama) menjadi 28 x 28 x 2,5 mm (CZL 601).
- **Setup kalibrasi dibalik dengan beban acuan 20/45/70 kg dan A dijepit ke meja** (`kalibrasi_setup_v4.svg`): digantikan protokol dumbbell sampai 42 kg (HL10). Alasan: tumpukan 70 kg tidak stabil di atas bidang tumpu sekitar 22 mm, beban acuan 70 kg tidak tersedia, dan tepi meja lab tidak boleh dijepit.
- **Load cell generik 180 kg (147 x 30 x 22 mm, kabel 110 cm)**: diganti CZL 601 80 kg (DS-LC1 di `datasheets/datasheet-library.md`). Semua ukuran dan angka kekakuan di dokumen ini sudah dihitung ulang untuk load cell baru.
- **Pelat aluminium 8 mm**: dihitung sebagai pembanding (HL9) dan tidak dipilih; tebal 10 mm diputuskan.
- **Sabuk PLA yang membungkus batang** sebagai pengganti celah: tidak dilanjutkan. Risikonya jalur gaya paralel yang memintas load cell (PLA kaku). Sabuk longgar yang tidak menahan beban mungkin berguna sebagai pelindung jepit jari atau penahan saat dibawa, belum dirancang.

## HL14. Urutan prototipe: kardus sampai cetak

- **Kardus** menguji geometri dan ergonomi: posisi tangan, tebal total sekitar 51,5 mm, panjang sekitar 170 mm, jangkauan zona genggam, dan titik keluar kabel. Load cell asli bisa ditempel pada prototipe kardus untuk cek penyaluran gaya. Kardus tidak menguji kekuatan, kekakuan, toleransi lubang, atau pas baut, dan melentur jauh lebih besar dari pelat sungguhan.
- **Urutan:** prototipe kardus, ukur load cell ([tabel pengukuran](../pengukuran/tabel-pengukuran-hgd.xlsx)), model parametrik di Fusion (HL6), kupon uji pas cetak (spacer dan pola lubang), uji lentur pelat (HL9), rakit, lalu kalibrasi dengan protokol dumbbell (HL10).
- Kekakuan hanya bisa divalidasi lewat hitungan (HL9) dan uji lentur, bukan lewat kardus.

## HL15. Mockup modul 3D printing

- Mockup 3D untuk modul 3D printing dikerjakan dengan bantuan Copilot (bukan Claude); catat di Pernyataan Penggunaan AI. Berkas dan isinya tidak tercatat di catatan ini.
- Acuan dari modul 3D printing: pelat dasar modul praktikum berupa grid 8 x 5 lubang Ø3,4 mm dengan pitch 20 mm pada pelat 170 x 110 x 4 mm; bagian modul harus tersekrup ke pelat itu lewat dua lubang M3 yang jatuh pada kelipatan 20 mm (jarak 20, 40, atau 60 mm); lubang M3 dimodelkan 3,4 mm. Aturan ini berlaku untuk bagian modul, bukan otomatis untuk housing grip proyek.

## HL16. Cowork: cara kerja dan brief awal

- **Kemampuan** (Help Center Claude, "Can Claude produce images?" dan "Custom visuals in chat and Cowork"): Claude tidak membuat foto atau ilustrasi realistis; yang bisa dibuat adalah diagram, grafik, dan visual interaktif berbasis HTML dan SVG. Hal ini berlaku juga di Cowork; hasilnya bisa diunduh sebagai `.svg` atau `.html`, dan model yang lebih kuat disarankan untuk visual kompleks. Render realistis diperoleh dari Render workspace di Fusion setelah modelnya jadi.
- **Cara kerja Cowork:** ia bekerja pada folder yang diizinkan, bukan attachment per pesan; ia bisa membaca gambar (.png, .jpg, .svg). Isi folder yang disarankan: foto load cell asli (kedua sisi, panah beban, lubang baut, tempat kabel keluar), tangkapan layar model Fusion, dan gambar referensi gaya. Jangan memasukkan seluruh catatan proyek.
- **Alur yang dipakai:** brief awal (di bawah), gambar v2 sampai v5, lalu perbaikan bertahap v6 sampai v12 (kabel Ø4, klem menyatu dengan Spacer A, arah kabel, panah stiker). Seluruh instruksi digabung dalam satu berkas koreksi (tidak disertakan di repo).
- **Brief awal (ringkas):** gambar teknis skematik (bukan foto) untuk housing grip yang hanya berisi load cell. Dua batang A (telapak, tetap) dan B (jari, bergerak) dengan load cell di antaranya; ujung tetap load cell dibaut ke tonjolan A, ujung bebas dibaut ke tonjolan B dari sisi berlawanan; celah antar bagian dan jalur gaya tunggal lewat load cell; ridge dan titik beban kalibrasi di sisi luar B; kabel keluar dari ujung tetap dan dijepit di A; nilai yang belum diketahui ditulis "belum diketahui" pada gambar dan tidak diisi angka (instruksi awal); keluaran tiga SVG (potongan, exploded, tampak atas) dan daftar asumsi.

## HL17. Rujukan untuk memodel di Fusion

- **Dipakai:** `potongan_memanjang.svg` (layout dan dimensi), `exploded_isometrik.svg` (arah baut, pola lubang, urutan rakit, stiker), `klem_kabel_tampak_depan.svg` dan `klem_kabel_tampak_kiri.svg` (klem kabel), `asumsi.md` (angka yang pasti dan nilai keputusan, hitungan kekakuan), `fusion_parameters_housing_grip.csv`, catatan ini (HL6 parameter, HL7 aturan cetak, [tabel pengukuran](../pengukuran/tabel-pengukuran-hgd.xlsx) pengukuran), dan `protokol-kalibrasi-hgd.md` dengan `kalibrasi_setup.svg` serta `kalibrasi_protokol.svg` hanya untuk merancang fixture kalibrasi.
- **Tidak dipakai:** semua gambar v2 sampai v10, `kalibrasi_setup_v4.svg`, berkas instruksi untuk Cowork (tidak disertakan di repo), dan `referensi_layout.png` (sketsa konsep lama).
- Gambar tidak berskala, jadi jangan di-import sebagai sketsa; ambil hanya angka yang tertulis. Parameter awal: `bar_t` = 10, `gap` = 2,5, `boss_L` = 28, `lever_arm` = 37, `grip_zone` = 85. Yang dimodelkan: load cell sebagai acuan (tidak dicetak), Bar_A dan Bar_B sebagai pelat aluminium (model untuk cek kecocokan dan gambar kerja bengkel, bukan dicetak), dan bagian cetak PLA (Spacer_A, Spacer_B, ridge, bidang tumpu, jepit kabel).
- **Langkah kerja dan parameter siap impor:** parameter yang berlaku adalah [fusion_parameters_housing_grip.csv](fusion/fusion_parameters_housing_grip.csv) (6 kolom; termasuk parameter klem; CSV lama dihapus). [work_instructions_fusion_housing_grip.md](fusion/work_instructions_fusion_housing_grip.md) masih menurut tata letak v5 (tebal total 52 mm, belum ada klem) dan perlu disesuaikan dengan v8 sebelum dipakai; impor belum diuji di Fusion.

## HL18. Sumber rujukan housing (bukan peer-review)

### HL18.1 Dokumentasi alat komersial (dokumen pabrikan dan penjual, BUKAN peer-review)

*Dipakai hanya sebagai pembanding bentuk dan rentang untuk desain housing (HL2). Halaman produk Biopac diblokir bot detection dan halaman produk Vernier terkena rate limit saat dibuka; data keduanya diambil dari datasheet dan manual yang tampil di hasil pencarian.*

| Kode | Isi (parafrase) | Sumber |
|---|---|---|
| KG1 | Kinvent K-Grip: bentuk silinder; tinggi 141 mm, lebar 47 mm, kedalaman 61 mm, 170 g; gaya maksimum 90 kgF; akuisisi 2000 Hz; nirkabel Bluetooth; baterai 12 jam | kinvent.com/kinvent-product/hand-dynamometer-k-grip/ dan jlwforce.com/products/kinvent-grip-dynamometer |
| GA1 | GripAble (Able Care): bentuk "C" yang lebih lebar dan bulat untuk menjaga posisi genggaman konsisten; lingkar 141 mm (Jamar 128 mm pada posisi 2); plastik 240 g (Jamar logam 490 g); diklaim tahan genggam berulang sampai 90 kg dan benturan jatuh; rata-rata membaca sekitar 69% dari Jamar PLUS+ untuk orang yang sama karena beda ukuran, bentuk, dan berat | able-care.co/blog/hand-dynamometer-guide |
| VN1 | Vernier HD-BTA: sensor gaya isometrik berbasis strain gauge; rentang 0-600 N (sekitar 61 kgf); resolusi 0,2159 N (halaman lain 0,2141 N); batas aman 850 N (halaman lain 900 N); akurasi ±0,6 N; daya 7 mA pada 5 VDC; kalibrasi pabrik berupa rumus linear slope dan intercept (contoh untuk kg: 18,0320 dan -1,9890); bisa untuk genggam atau jepit | vernier.com/hd-bta dan vernier.com/til/1431 |
| BP1 | Biopac SS25LA: transduser genggam isometrik; rentang isometrik 0-90 kg (kit BSL menyebut 0-50 kgf), rated 100 kg, sensitivitas 0,75 kg; posisi tangan didefinisikan (telapak melintang di batang yang lebih pendek; pada model lama di bagian atas busa, tepat di bawah lubang); rancangan isometrik disebut meningkatkan keterulangan | seas.upenn.edu/~belab/equipment/Biopac_sensors/SS25LA_hand_dynamometer.pdf dan biopac.com/?p=9303 |
| PP1 | Listing penjual (IndiaMART), push-pull aluminium berbasis strain gauge untuk dua tangan: 0-100 kgF, diameter pegangan 25 mm, 114 x 217 x 38 mm, 1,1 kg. Kualitas sumber rendah | m.indiamart.com/proddetail/push-pull-dynamometer-7472334988.html |
| SP1 | Dynamometer tipe pegas (entri database Rehadat): rumah aluminium, pegangan dapat disetel menurut ukuran tangan, sampai 100 kg | eastin.eu (database Rehadat, entri id-tec 101346.0) |
| WM1 | Listing Walmart: model dial analog pegas berbahan ABS (dial 42 mm); model digital di daftar serupa hanya menyebut kapasitas 90-120 kg tanpa detail mekanis. Hanya konteks pasar | walmart.com (listing produk dynamometer genggam) |

### HL18.2 Diskusi komunitas (BUKAN sumber akademik)

| Kode | Isi (parafrase) | Sumber |
|---|---|---|
| FP1 | Utas Arduino Forum tentang kalibrasi load cell straight-bar untuk dynamometer genggam: (a) kapasitas 20 kg terlalu kecil untuk genggam, disarankan 50-100 kg; (b) load cell dibaut di kedua ujung ke batang kaku, lalu beban berbobot diketahui digantung di titik genggam untuk kalibrasi; (c) load cell mengukur deformasi, bukan gaya langsung, dan menambah batang kaku mengubah kekakuan sistem sehingga `calibration_factor` harus dikalibrasi ulang setelah dirakit; (d) dua pendekatan kalibrasi: beban acuan diketahui, atau perbandingan statistik terhadap alat referensi | forum.arduino.cc/t/calibrating-a-straight-bar-load-cell/501923 |

## HL19. Riwayat koreksi dan audit

- v11 (31 Okt. 2026): pola lubang pasang terukur dan cocok dengan gambar penjual (12 / 106 / 15; `asumsi.md` 8.1); lubang spacer di model Ø7,6 (`hole_d` + `comp_hole`, parameter `comp_hole` ditambahkan ke CSV).
- v10 (12 Okt. 2026): dirapikan terhadap seluruh catatan. Lampiran A diganti pointer ke `asumsi.md` dan Lampiran B (v4, riwayat) dihapus (riwayatnya ada di `asumsi.md` Bagian 9 dan di entri di bawah). Gambar yang berlaku: potongan v11, exploded v12, setup kalibrasi v11, dua tampak klem v1. Panah beban stiker menunjuk ke arah B (A di atas); gaya nyata B ke A berlawanan, jadi pembacaan bertanda negatif dan tandanya diatur di firmware. Jarak lubang kabel Ø5 ke ujung 4,2, tinggi 13,25, kedalaman ≈14 (asumsi, belum terukur). Daftar berkas Cowork diganti `koreksi.md`. Referensi versi diselaraskan (catatan proyek v39).
- v9 (11 Okt. 2026, malam): kabel ternyata keluar dari muka samping panjang (bukan muka ujung). Lubang kabel Ø5 mm di tengah tinggi, pusat 4,2 mm dari ujung load cell (terukur: tepi ke arah tengah 6,7 mm dari ujung); kabel Ø4 mm longgar. Panah beban di muka ujung menunjuk ke bawah sepanjang tebal 21,5 mm (C5 selesai; aturan pasang: panah searah gaya B pada ujung bebas). Klip kabel tidak lagi ditempel ke pelat aluminium, tetapi menyatu dengan Spacer A (rak + blok, lubang Ø4,1 mm, lem tembak). Radius tekuk 20 mm menjadi 10 mm pada kabel bebas (asumsi); sisa kabel dari blok sekitar 27 cm. Angka lain tidak berubah.
- v8 (11 Okt. 2026): semua butir yang sebelumnya terbuka diganti nilai keputusan atau asumsi desain berlabel (Lampiran A6): paduan 6061-T6; lubang Ø6,6 mm (pelat) dan Ø7,0 mm (spacer); ulir load cell M6 tembus dari kedua muka (terukur); baut ISO 4017 M6 x 25 dengan torsi 3 N·m; anggaran celah 0,5 / 1,0 / 0,3 mm (sisa sekitar 0,46 mm); kabel Ø4 mm, panjang asumsi 28 cm, radius tekuk 20 mm, penjepit blok PLA; load cell terukur 130 x 28 x 21,5 mm (tebal total 51,5 mm); alas karet 6 mm dengan lubang lonjong. Lampiran B (v4) dan blok riwayat hitungan di HL9 tidak diubah. Gambar SVG v5 belum digambar ulang.
- v7: disesuaikan dengan load cell CZL 601 80 kg (130 x 28 x 22 mm) dan gambar v5: spacer 28 x 28 x 2,5 mm, a = 37 mm, x = 102 mm, pelat aluminium 10 mm diputuskan, baut M6 x 25, kabel Ø5 mm (asumsi); HL1, HL2, HL4 sampai HL14, HL16, HL17, HL20 diperbarui; HL9 memuat hitungan v5 dan riwayat; metode kalibrasi diganti protokol dumbbell (HL10); Lampiran A diganti isi `asumsi.md` dan asumsi v4 dipindah ke Lampiran B.
- v1: dipisah dari catatan proyek (v31); asumsi gambar skematik v4 digabung sebagai Lampiran A.
- v2: diagram ASCII di HL4 diganti SVG mandiri (konsep, nomor callout 1-5).
- v6: parameter `spA_c` dan `spB_c` ditambahkan (CSV, HL6, HL17, work instructions); koordinat panjang di work instructions memakai load cell di titik asal.
- v5: desain berubah ke spacer PLA terpisah 30 x 30 x 2,5 mm dan pelat polos; HL4, HL5, HL6, HL7, HL9 (blok pembaruan dengan angka baru), HL11, HL13, HL20, dan catatan di Lampiran A diperbarui. Gambar Cowork v4 belum diperbarui.
- v4: tautan ke langkah kerja Fusion dan CSV parameter di folder `fusion/` ditambahkan (HL6, HL17).
- v3: diagram konsep itu diganti gambar Cowork v4: potongan dan exploded di HL4, setup kalibrasi di HL10; daftar HL4 merujuk ke nomor callout gambar v4. Gambar konsep `hl4-layout.svg` tidak dipakai lagi.

### Audit v27: desain housing

1. Layout awal desain housing (catatan proyek v26) mengasumsikan batang kaku tanpa menghitung kekakuan. Pemeriksaan gambar skematik menunjukkan pelat PLA 7 mm melentur puluhan mm pada 70 kgf (HL9). Diperbaiki di v27: status bahan, tebal total, dan klaim "celah = stop overload" di 14.4.
2. Klaim bahwa titik gantung kalibrasi melewati jalur yang sama dengan remasan salah arah (HL10); diganti bidang tumpu.
3. Instruksi koreksi gambar sempat menyatakan penyebaran beban menurunkan tegangan di pangkal; salah (HL9).
4. Proposal Bagian 6 (risiko enclosure) menyebut fallback "cetak ulang dengan PETG atau bracket logam". PETG tidak menyelesaikan kekakuan karena modulusnya sebanding dengan PLA; yang relevan adalah bracket atau pelat logam.
5. Proposal 3.6 belum memuat housing grip terpisah (sudah tercatat di Audit v26).

### Audit v28: hasil gambar v4

1. HL9 menulis baja 8 mm "lolos di atas kertas". Itu hanya benar untuk penampang utuh; dengan dua lubang Ø6,5 mm tegangannya sekitar 227 MPa, hampir sama dengan luluh baja lunak. Dikoreksi di v28.
2. Gambar v4 memenuhi instruksi: aluminium 10 mm, celah 2,5 mm sebagai celah bebas-sentuh, tanpa eyelet, bidang tumpu kalibrasi, penanda ujung jauh sekitar 133 mm, dan penjumlahan tebal total.
3. Tebal total 51,5 mm melebihi kisaran 40-48 mm di draf awal; kisaran itu bukan batas dari spesifikasi, tetapi kenyamanan genggam perlu dicek dengan prototipe kardus sebelum bahan diputuskan.
4. Setup kalibrasi v4 punya keterbatasan (HL10) yang belum punya solusi.


## HL20. Pertanyaan terbuka untuk dosen atau TA dan konsultasi teknik mesin

Keputusan bahan batang menunggu jawaban dosen atau TA, atau konsultasi dengan mahasiswa teknik mesin.

Untuk dosen atau TA:
1. Apakah grip hibrida (pelat logam dan bagian cetak PLA) memenuhi syarat bagian cetak dan enclosure cetak 3D di tahap akhir?
2. Apakah pelat logam dan jasa bor masuk anggaran Rp300.000 dengan bukti beli? Sisa anggaran sekitar Rp46 ribu sebelum ongkir.
3. Apakah bengkel kampus atau bor duduk boleh dipakai mahasiswa, dan apakah ada biayanya?
4. Apakah perubahan housing (grip terpisah, batang logam) perlu dicatat sebagai revisi proposal di luar 3.6?
5. Apakah lab punya anak timbang atau cara aman untuk beban kalibrasi sampai 70 kgf, atau cukup kalibrasi dumbbell sampai 42 kg dengan ekstrapolasi (HL10)?
6. Tenggat gerber: Minggu 7 (panduan mata kuliah) atau Minggu 8 (panduan tugas); lihat konflik di catatan proyek.

Untuk konsultasi teknik mesin:
1. Cek hitungan lentur: kantilever, beban 686 N di 37 mm dari tepi spacer 28 mm, pelat aluminium 10 mm (E ≈ 70 GPa). Apakah celah 2,5 mm cukup, dan apakah tegangan penampang berlubang (≈80-100 MPa; Ø6,6 mm: sekitar 91 MPa) wajar?
2. Apakah ada cara menyalurkan beban ke load cell yang lebih kaku dalam tebal total sekitar 51,5 mm?
3. Grade aluminium strip apa yang umum dijual di sini (6061 atau 5052), dan bagaimana cara mengeceknya?
4. Cara mengebor lubang baut yang akurat dengan alat sederhana (templat bor, mata bor, toleransi diameter).
5. Apakah spacer PLA 2,5 mm di bawah baut aman terhadap relaksasi preload (creep)? Alternatifnya shim aluminium 3 mm.
6. Apakah tekanan tepi dan gaya ungkit di tepi dalam spacer (baut 12 mm dari ujung, 16 mm dari tepi dalam spacer) wajar, dan berapa torsi baut M6 yang sesuai untuk ulir di aluminium load cell dan spacer PLA (asumsi desain saat ini: 3 N·m)?

Bahan yang dibawa: potongan v5 (`potongan_memanjang_v5_logam.svg`) dan hitungan di HL9; sebutkan yang berupa asumsi (beban 686 N, lengan tuas 37 mm, beban terpusat, jarak lubang dari gambar penjual yang belum diukur, anggaran celah 0,5 / 1,0 / 0,3 mm, torsi 3 N·m).

## Lampiran A. Asumsi gambar skematik housing grip

Isi lengkap (asumsi, hitungan kekakuan, spacer dan baut, anggaran celah, nilai keputusan 11 Okt. 2026, klem kabel, arah kabel dan panah stiker, pertentangan instruksi dan penyelesaiannya, riwayat v2 sampai v4) ada di [asumsi.md](asumsi.md). Lampiran ini dulunya menyalin isi dokumen itu; salinannya dihapus supaya tidak ada dua versi yang berbeda.

Nilai kunci (rincian di `asumsi.md`): pelat Al 6061-T6 170 x 30 x 10, spacer PLA 28 x 28 x 2,5, load cell CZL 601 130 x 28 x 21,5, celah 2,5, tebal total 51,5, a = 37, x = 102, bentang bebas 74, zona genggam ≈85; lenturan ujung jauh 0,24 mm pada 70 kgf, sisa celah ≈0,46 mm; baut ISO 4017 M6 x 25 dengan torsi 3 N·m (asumsi); kabel Ø4, lubang Ø5 (4,2 dari ujung, 13,25 di atas A), klem Ø4,1 menyatu dengan Spacer A.

# Asumsi — Gambar Skematik Housing Grip Load Cell (v5, varian logam)

Dikerjakan satu kali, langsung dari v4 ke v5. Sumber yang dipakai:
- `koreksi_v4.md` (utama);
- bagian `koreksi_v3.md` yang tidak digantikan (spacer PLA terpisah, pelat polos, urutan exploded, kesalahan yang pernah terjadi);
- instruksi pengguna di chat;
- gambar penjual Automa-88 "Loadcell CZL-601, Size & Dimensions", disimpan sebagai `czl601_dimensi_automa88.png`.

Berkas v4 tidak ditimpa. Semua gambar **tidak berskala**, dan celah 2,5 mm digambar diperbesar.

**Pembaruan 11 Okt. 2026 (hanya teks dokumen ini):** hasil ukur load cell (130 × 28 × 21,5 mm), ulir lubang pasang (M6, tembus, bibir kisi Ø8 mm), dan diameter kabel (4 mm) sudah dimasukkan. Ringkasan di Bagian 8. **Gambar SVG v5 belum digambar ulang** dan masih memuat label lama (misalnya kabel Ø5, tinggi load cell 22).

**Pembaruan 8 Nov. 2026 (desain v16):** lebar pelat menjadi 40 mm (140 x 40 x 8); angka Bagian 2 diperbarui. Gambar di `gambar/` dan `kalibrasi_setup.svg` digambar ulang dari CSV (berskala); potongan A-A klem sekarang di sumbu kabel (x = −59,7). Daftar gambar dan rincian klem di bawah (12 Okt. 2026, Bagian 4.1) adalah riwayat.

**Pembaruan 7 Nov. 2026 (desain v15):** pelat menjadi 140 x 35 x 8 mm dan celah (tebal spacer) 3,0 mm; Bagian 2 dan 3 diperbarui. Angka v5 di Bagian 1 adalah riwayat.

**Pembaruan 6 Nov. 2026:** Bagian 2 dan 3 dihitung ulang dengan ukuran load cell terukur, `boss_L` 24 dari CSV, dan baut yang dibeli (flange A2-70); rincian di `desain-housing-load-cell.md` HL9.

**Pembaruan 31 Okt. 2026:** pola lubang pasang terukur dan cocok dengan gambar penjual (12 / 106 / 15 mm, Bagian 8.1). Lubang spacer di model menjadi Ø7,6 mm (`hole_d` + `comp_hole`) supaya tercetak sekitar Ø6,6 mm (Bagian 2.4).

**Pembaruan 12 Okt. 2026 (hanya teks dokumen ini):** gambar yang berlaku sekarang bukan lagi v5, melainkan:
- `potongan_memanjang.svg` dan `kalibrasi_setup.svg`;
- `exploded_isometrik.svg` (panah stiker beban menunjuk ke arah B; lihat 4.1);
- `klem_kabel_tampak_depan.svg` dan `klem_kabel_tampak_kiri.svg` (tampak depan dan potongan A-A di x = 4,2 untuk klem kabel);
- `fusion_parameters_housing_grip.csv` (tidak ada CSV v9 sampai v12).

Berkas v5 di tabel di bawah hanya riwayat. Dokumen ini adalah gabungan: isi asumsi lama (v2, v3, dan v4) sudah dipadatkan ke Bagian 9 dan berkas lama itu dihapus (12 Okt. 2026). Instruksi untuk Cowork tidak disertakan di repo. Arah kabel, panah stiker, dan rincian klem di Bagian 4.1 sudah mengikuti gambar-gambar di atas.

| File v5 | Isi |
|---|---|
| `potongan_memanjang_v5_logam.svg` | Potongan memanjang melalui satu baris lubang. Pelat Al 10 polos, Spacer A/B PLA, load cell 130 mm, baut M6 × 25 + washer. Dimensi a = 37, x = 102, spacer 28, bentang bebas 74, lubang 12 dari ujung, 106 antar pasangan |
| `exploded_isometrik_v5_logam.svg` | Urutan dari atas: pelat A → Spacer A → load cell → Spacer B → pelat B. 4 baut pola 2 × 2 (15 melintang, 106 antar pasangan) |
| `kalibrasi_setup_v5.svg` | **Tidak dibuat**, sesuai permintaan (lihat Pertentangan 6) |

## 1. Perubahan dari v4

1. **Load cell diganti** dengan CZL601. Ukuran di gambar penjual 130 × 28 × 22 mm; **terukur 130 × 28 × 21,5 mm (10 Okt. 2026, jangka sorong)**. Ukuran lama 147 × 30 × 22 mm dari load cell 180 kg tidak dipakai lagi. Load cell ditaruh di tengah pelat 170 mm, jadi sisa pelat 20 mm di tiap ujung.
2. **Lubang pasang:** 4 × M6, dua per ujung. Pusat lubang 12 mm dari ujung load cell, 106 mm antar pasangan, dan 15 mm melintang (gambar penjual; terukur 31 Okt. 2026 dan cocok, Bagian 8.1). Lubang pelat dan spacer sejajar dengan lubang load cell. Diameter lubang: **Ø6,6 mm di pelat** dan **Ø7,0 mm di spacer PLA** (keputusan 11 Okt. 2026; diganti 31 Okt. 2026: Ø7,6 di model, tercetak ≈Ø6,6, Bagian 2.4; gambar SVG v5 belum memuat angka ini). **Ulir lubang di load cell terukur M6** (diameter dalam 5,0 dan 5,2 mm; bibir kisi Ø8 mm; tembus lurus), lihat Bagian 8.
3. **Tonjolan dihapus**, dan pelat A/B menjadi **pelat aluminium 10 mm polos** (sudah diputuskan; paduan **6061-T6**, alternatif 5052-H32, keputusan 11 Okt. 2026). Counterbore v4 juga dihapus, karena pelat polos tidak difrais.
4. **Spacer PLA terpisah:**
   - Spacer A dan Spacer B, masing-masing 28 × 28 × 2,5 mm, menutup kedua lubang di ujungnya.
   - Callout 4 = Spacer A, callout 5 = Spacer B. Nomor callout lain tetap. Arsirannya dibedakan dari aluminium dan load cell.
5. **Baut:** M6 × 25 + washer logam, 4 buah, langsung masuk ulir load cell tanpa mur. Kepala dan washer menonjol di luar pelat (lihat 3.3).
6. **Geometri turunan (boss_L = 28):** lengan tuas a = 37 mm, ujung jauh x = 102 mm, bentang bebas 74 mm. Nilai lama 60/133 (v4) dan 43,5/117/30/87 (`koreksi_v3.md`) tidak dipakai.
7. **Zona genggam ≈85 mm**, sebelumnya ≈90 di v4. Zona ini lebih panjang daripada bentang bebas 74 mm, jadi sebagian beban jatuh di atas spacer. Model beban terpusat di tengah tetap dipakai dan konservatif untuk momen.
8. **Area tengah load cell** yang berarsir di gambar penjual digambar sebagai area tengah/penutup sebagai area tengah/penutup (callout 17). Tinggi penutup di atas permukaan diasumsikan 1,0 mm (asumsi desain, 11 Okt. 2026; ukur pada load cell sungguhan).
9. **Lubang binokular** digambar sebagai dua lubang bulat yang disambung celah, mengikuti bentuk di gambar penjual. Ukurannya skematik, tanpa angka.
10. **Kabel:** panjang kabel CZL601 belum diketahui, jadi gambar dan legenda tidak memuat angka panjang kabel. Keputusan 11 Okt. 2026: panjang kabel dipakai ≈28 cm (listing penjual; asumsi desain), lihat 4.1.
11. **Tetap sama:** celah 2,5 mm sebagai celah bebas-sentuh, tebal total 51,5 mm, ridge dan bidang tumpu di sisi luar B, klip kabel menyatu dengan Spacer A, arah beban remasan, jalur gaya tunggal, dan catatan "Stop overload belum dirancang".
12. **Diameter kabel load cell = 4 mm (dilaporkan pengguna, 11 Okt. 2026)**, menggantikan asumsi kerja 5 mm di revisi v5 sebelumnya. Cocok dengan dokumen distributor (4 mm). Yang terdampak (rincian di bagian 4.1):
    - penjepit kabel di batang A: lubang ≈ Ø kabel (4 mm), lebar dan tinggi penjepit menyesuaikan;
    - lengkungan sisa kabel di dekat penjepit: radius tekuk 10 mm (asumsi desain, revisi 11 Okt. malam, lihat 4.1);
    - tempat kabel keluar dari ujung load cell ke penjepit.

    ⚠ Gambar SVG v5 masih berlabel "kabel Ø5 (asumsi kerja, belum diukur)" dan penjepit ≈9 mm. Keduanya belum diperbarui. Angka lain tidak berubah.

## 2. Hitungan kekakuan

**Dihitung ulang 6 Nov. 2026, lalu 7 Nov. 2026 untuk pelat 8 mm,** dengan ukuran load cell terukur (Bagian 8, 8.1) dan geometri CSV v13 (`boss_L` 24): a = 41 mm, x = 106 mm, bentang bebas 82 mm. Metode, diagram V dan M, dan semua tabel ada di [desain-housing-load-cell.md, HL9](desain-housing-load-cell.md#hl9-temuan-kekakuan-batang-dan-usulan-bahan); bagian ini hanya ringkasan supaya tidak ada dua versi angka.

### Model
- Urutan Beer, Johnston, dan DeWolf Bab 5: diagram benda bebas, reaksi, diagram V dan M, σ = M/S, S_min = M_maks/σ_izin.
- F = 686 N (70 kgf). Pelat Al 6061-T6 40 x 8 mm (panjang 140), I = 1 707 mm⁴, S = 427 mm³, S_bersih (2 lubang Ø6,6) = 285,9 mm³, E 70 GPa, luluh 240 MPa.
- Lenturan: kantilever dijepit di tepi dalam spacer; tiga kasus beban (terpusat di tengah, merata di zona genggam 85 mm, resultan bergeser 10 mm). Kasus merata dipakai.
- Keterbatasan: spacer dan baut dianggap kaku; lenturan load cell, tinggi penutup, dan margin rakit masih asumsi; hitungan kasar untuk arah desain, bukan verifikasi.

### 2.1 Hasil (Al 140 x 40 x 8, `boss_L` 24, celah 3,0)

| Besaran | Nilai |
|---|---|
| M_maks di tepi dalam spacer | 28 126 N·mm (terpusat); 28 135 N·mm (merata) |
| σ penampang pejal / berlubang (batas atas) | 65,9 MPa (faktor aman 3,6) / 98,4 MPa (faktor aman 2,4) |
| S_min (faktor aman 2) | 234 mm³; tebal minimum 5,9 mm pejal, 7,3 mm berlubang (lebar 40) |
| Lenturan ujung jauh: terpusat / merata / bergeser 10 mm | 0,446 / **0,558** / 0,665 mm |

Hitungan v15 (pelat 140 x 35 x 8): 75,3 / 121,0 MPa, lenturan 0,638 mm, sisa celah ≈0,56 mm. Hitungan v14 (pelat 170 x 30 x 10, celah 2,5): 56,3 / 100,5 MPa, lenturan 0,304 / 0,381 / 0,454 mm, sisa celah ≈0,32 mm. Hitungan lama (spacer 28, a = 37, x = 102, beban terpusat: 0,241 mm, 50,8 MPa, σ_net Ø6,6 91 MPa) diganti. Catatan lama bahwa beban terpusat "konservatif" hanya benar untuk momen, tidak untuk lenturan ujung jauh.

### 2.2 Apakah celah 3,0 mm terlampaui di bawah beban?
Tidak. Lenturan ujung jauh 0,56 mm (beban merata), jadi sisa celah 2,44 mm sebelum anggaran dan ≈0,64 mm setelah anggaran 2.3. Bila posisi jari meleset 10 mm ke ujung jauh, sisa ≈0,54 mm. Dengan celah lama 2,5 mm, pelat 8 mm hanya menyisakan 0,14 mm, jadi celah dinaikkan.

### 2.3 Anggaran celah (Al 8, celah 3,0, `boss_L` 24, beban merata)

| Komponen | Nilai |
|---|---|
| Lenturan ujung jauh pelat | 0,56 mm |
| Lenturan load cell sendiri pada 70 kgf | 0,5 mm (asumsi desain; belum diukur) |
| Tinggi penutup/area tengah di atas permukaan | 1,0 mm (asumsi desain; **ukur sebelum pelat dibor**, angka terbesar di anggaran) |
| Margin rakit (kerataan pelat, spacer, baut) | 0,3 mm (asumsi desain) |
| **Total terpakai** | **2,36 mm** |
| **Sisa dari celah 3,0 mm** | **≈0,64 mm** |

### 2.4 Catatan Ø lubang
Keputusan 11 Okt. 2026: **lubang pelat Ø6,6 mm** (lubang lewat M6 seri sedang, ISO 273) dan **lubang spacer Ø7,0 mm** (Ø pelat + 0,4). **Diganti 31 Okt. 2026:** lubang spacer di model = `hole_d` + `comp_hole` = 6,6 + 1 = **Ø7,6 mm**, mengikuti aturan housing elektronik. Lubang bulat kecil hasil cetak tim menyusut sekitar 1 mm (laporan modul 3D: M3 3,4 → 2,4), jadi hasil cetaknya sekitar Ø6,6 mm, sama dengan lubang pelat. Ø5,5 tidak mungkin dipakai sebagai lubang lewat baut M6. Dengan Ø6,6, σ penampang berlubang (batas atas) ≈98 MPa untuk pelat 40 x 8, faktor aman ≈2,4 terhadap luluh 6061-T6 ≈240 MPa.

## 3. Pemeriksaan spacer PLA dan baut

Dihitung ulang 6 Nov. 2026 (dan 7 Nov. 2026 untuk spacer 3,0 mm dan pelat 8 mm) untuk bidang kontak 24 x 28 dan baut yang dibeli (flange bolt M6 x 25 stainless A2-70, luluh ≥ 450 MPa); rincian di HL9.7.

### 3.1 Kompresi spacer (E PLA ≈3 GPa, F = 686 N, tebal 3,0 mm)
Luas bersih 24 x 28 − 2 lubang Ø6,6 = 603,6 mm²; tekanan rata-rata 1,14 MPa; kompresi ≈0,001 mm. Dapat diabaikan terhadap celah. (Lama: 28 x 28, 0,0008 mm.)

### 3.2 Tekanan tepi spacer dan gaya ungkit
- Seluruh momen ditahan kontak spacer (perlu preload cukup): S = 28 x 24²/6 = 2 688 mm³, M/S = 10,5 MPa, ditambah 1,14 MPa = **11,6 MPa**.
- Tanpa preload, pelat berungkit di tepi dalam bidang kontak. Baut 12 mm dari ujung dan 12 mm dari tepi dalam (sebelumnya 16 mm dengan spacer 28): gaya kontak di tepi 686 x 53 / 12 = **3 030 N**, tarik baut 2 344 N = **1 172 N per baut**; tekanan lokal (lebar kontak 5 mm, asumsi) ≈21,6 MPa.
- Torsi baut **3 N·m** (asumsi desain): preload F_i = T/(K·d) = 2,5 kN (K 0,2) atau 1,67 kN (K 0,3, umum untuk stainless kering), yaitu 2,1 atau 1,4 kali tarik ungkit. Pelat tidak berungkit, tetapi margin tipis bila K 0,3. Cadangan: 4 N·m (2,2 sampai 3,3 kN).
- Tekanan spacer akibat preload 5,5 sampai 8,3 MPa; dengan tekanan tepi ≈17 sampai 20 MPa, di bawah kisaran tekan PLA ≈50 MPa tetapi rawan creep.
- **Risiko PLA:** creep/relaksasi preload. Kencangkan ulang 3 N·m sebelum tiap sesi kalibrasi dan setelah 10 sesi; beri tanda cat pada kepala baut untuk melihat bila longgar.

### 3.3 Panjang dan kekuatan baut M6 x 25
- **Panjang ulir masuk:** 25 − ring (1,6 mm) − pelat 8 − spacer 3,0 ≈ **12,4 mm** (tanpa ring 14,0 mm), ≈2,1 x d. Lubang berulir tembus lurus (terukur), jadi baut tidak mentok; ujung baut 9,1 mm dari muka seberang load cell (tanpa ring 7,5). Baut terpanjang yang boleh M6 x 30; M6 x 35 menembus muka seberang 0,9 mm.
- **Tegangan baut:** (preload 2,5 kN + tarik ungkit 1,17 kN) / luas tegangan 20,1 mm² ≤ 183 MPa, faktor aman ≥ 2,5 terhadap luluh A2-70 450 MPa.
- **Kepala baut:** flange bolt, tinggi kepala belum diukur (sekitar 6 mm untuk ukuran umum; rencana lama ISO 4017 + washer = 5,6 mm). Tonjolan di luar pelat A dan B, 17 mm dari ujung pelat; tepi flange (Ø14,2 menurut DIN 6921) 3,4 mm di luar zona genggam (27,5–112,5 mm dari ujung pelat). Tebal total = 8 + 3 + 21,5 + 3 + 8 + ridge 5 = 48,5 mm; kepala baut belum termasuk.

### 3.4 Lebar dan panjang spacer
- **Lebar:** bidang kontak 28 mm, sama dengan lebar load cell terukur; spacer luar 34 mm dengan bibir samping (CSV `spacer_W`).
- **Panjang:** bidang kontak `boss_L` 24 mm supaya tidak masuk area strain gauge (panjang blok ujung ≈27–28 mm hanya taksiran dari gambar penjual). Spacer luar 27 mm termasuk bibir ujung (CSV `spacer_L`).

## 4. Catatan lain
- **Kapasitas load cell:** judul `koreksi_v4.md` menyebut CZL601 **80 kg**, sehingga 70 kgf (batas atas S1) = **87,5% dari kapasitas**.
  - Spesifikasi lama (2,0 mV/V, safe overload 150%, panjang kabel, dan seterusnya) milik load cell 180 kg dan **tidak berlaku lagi**. Nilai desain dari datasheet DS-LC1: overload aman 120% (96 kgf) dan merusak 150% (120 kgf).
  - Karena stop overload belum dirancang, remasan di atas ≈80 kg tidak terlindungi. Ini perlu dipertimbangkan bersama batas overload dari datasheet.
- **Massa B:** pelat B Al 140 x 40 x 8 ≈120 g di ujung bebas, jauh di atas tolerance ±36 g di S1. Tare dan pengukuran harus dilakukan pada orientasi yang sama.
- **Grip hibrida:** pelat logam (potong dan bor di bengkel) + spacer dan penjepit PLA. Baut tidak masuk ke PLA, tetapi menembus spacer PLA yang ikut terjepit.

### 4.1 Kabel: diameter 4 mm (dilaporkan pengguna, 11 Okt. 2026)
- **Diameter kabel:** 4 mm, menggantikan asumsi kerja 5 mm dari revisi v5 sebelumnya. Cocok dengan dokumen distributor (4 mm). Alat ukur dan jumlah pengukuran tidak dicatat; ukur ulang dengan jangka sorong bila perlu.
- **Klip kabel (revisi 11 Okt. malam: menyatu dengan Spacer A):**
  - Klip tidak lagi ditempel ke pelat aluminium. Spacer A dicetak satu bagian dengan rak 2,5 mm (≈16 mm sepanjang batang, menjulur 10 mm melewati sisi load cell, ≈9 mm melewati tepi pelat A) dan blok penjepit; semuanya terjepit oleh baut (asumsi desain).
  - Blok: lebar 8,2 mm (sepanjang batang), tebal 8 mm ke arah luar (mulai 1 mm dari sisi load cell), tinggi ≈17,4 mm dari permukaan pelat A. Lubang kabel **Ø4,1 mm** (Ø kabel + 0,1), sumbu 13,25 mm di atas permukaan A dan 4,2 mm dari ujung load cell, segaris dengan lubang Ø5 di load cell. Dinding ≈2,05 mm di kiri dan kanan lubang.
  - **Dinding atas dan bawah lubang (12 Okt. 2026, dari potongan A-A di x = 4,2):** 2,1 mm ke arah B (17,4 − 13,25 − 2,05) dan 11,2 mm ke arah A (13,25 − 2,05, diukur dari muka pelat A, termasuk rak 2,5). Tampak depan dan tampak kiri: `klem_kabel_tampak_depan.svg` dan `klem_kabel_tampak_kiri.svg`.
  - Kabel direkat **lem tembak** di lubang. Epoksi tidak dipakai (tidak tersedia), dan pengikat kabel dibatalkan. Celah kabel Ø4 ke lubang Ø5 load cell = 0,5 mm per sisi.
  - Jarak bebas blok ke pelat B ≈9 mm (26,5 − 17,4 = 9,1). Blok 20–28 mm dari ujung pelat, di luar zona genggam (mulai 42,5 mm). Bagian sisi A saja.
- **Tempat kabel keluar (terukur 11 Okt. 2026, menggantikan asumsi muka ujung):**
  - Kabel keluar **menyamping** dari muka samping panjang load cell (130 × 21,5 mm), di dekat ujung berkabel, di tengah tinggi (10,75 mm dari muka atas dan bawah, jadi 13,25 mm di atas permukaan pelat A).
  - Lubang kabel Ø5 mm; kabel Ø4 mm agak longgar. Tepi lubang ke arah tengah 6,7 mm dari ujung load cell, jadi pusat lubang 4,2 mm dari ujung dan dinding ke muka ujung 1,7 mm. Titik keluar 24,2 mm dari ujung pelat, di luar zona genggam (mulai 42,5 mm).
  - **Kedalaman lubang Ø5 ke arah dalam ≈14 mm (asumsi, belum terukur;** nilai sementara LC_W / 2 di Fusion dan di tampak kiri; ukur dengan batang kawat atau kedalaman jangka sorong).
  - Ø4 mm lebih besar daripada celah 2,5 mm, tetapi kabel keluar di tengah tinggi di antara kedua pelat dan di samping spacer, jadi tidak lewat celah A–LC atau B–LC.
  - Ujung berkabel dijadikan ujung tetap (A).
  - **Panah beban di muka ujung bebas menunjuk ke arah B** (searah tebal 21,5, dengan A di atas), sesuai stiker pada benda asli (dikoreksi pengguna 12 Okt. 2026; digambar di `exploded_isometrik.svg`). Gaya tekan sebenarnya (B menekan ke arah A) berlawanan dengan panah, jadi pembacaan bertanda negatif bila skala tidak dibalik. Protokol kalibrasi sudah mengizinkan kemiringan skala negatif; tandanya diatur di firmware. Arah panah pada benda nyata masih perlu dicek ulang.
- **Lengkungan kabel setelah blok (asumsi desain):**
  - Kabel keluar lurus menyamping dari lubang Ø5 lewat blok, lalu melengkung bebas **ke arah B (ke bawah bila A di atas), menjauhi telapak**, dengan radius **10 mm** (2,5 × diameter; asumsi untuk kabel statis, cek pada kabel sungguhan). Lengkung 90° ke ujung pelat (revisi sebelumnya) dan radius 20 mm dibatalkan.
  - Titik lemah: rak PLA menjulur melewati tepi pelat (kantilever). Tarikan kabel kecil, tetapi bila rak melentur, lebarkan rak sepanjang batang.
- **Arah kabel pada posisi pakai (12 Okt. 2026, protokol V1: duduk, bahu netral, siku 90°, pergelangan 0-30°):** tangan kanan menggenggam, telapak di atas A, jari menekan B ke arah A. Lubang dan kabel di muka depan; lengkung bebas **ke arah B**, menggantung ke bawah menuju housing LCD di tangan kiri, tidak melintas di atas telapak (asumsi; orientasi tangan terhadap batang belum diverifikasi pada benda nyata). Sama dengan model CAD pemilik proyek. Kabel hanya terpasang lurus di lubang blok, jadi arah lengkung hanyalah rute. Pada setup kalibrasi (rakitan dibalik) kabel diarahkan turun ke alas karet, yaitu ke arah A, supaya tidak naik ke ruang di bawah papan. Ini menggantikan arah "ke arah A" di `koreksi_v9.md` C.3 dan `koreksi_v10.md`.
- **Panjang kabel:** dipakai ≈28 cm (listing penjual; dokumen distributor menulis 0,42 m; asumsi desain). Sisa jangkauan dari blok ≈27 cm. Housing elektronik harus berada dalam jangkauan ini. Kabel sambungan di luar lingkup dokumen ini.

## 5. Pertentangan dan cara penyelesaiannya
Aturan: ikuti instruksi yang lebih spesifik.

1. **Ukuran spacer dan geometri.** `koreksi_v3.md`: spacer 30 × 30, a = 43,5, x = 117, bentang 87. `koreksi_v4.md`: 28 × 28, a = 37, x = 102, bentang 74. → **Ikut `koreksi_v4.md`** (menggantikan v3 secara eksplisit, dan memakai ukuran load cell yang berlaku).
2. **Posisi lubang.** `koreksi_v3.md`: "posisi lubang TBD, jangan menulis angka koordinat". `koreksi_v4.md` dan instruksi pengguna: 12 dari ujung, 106 antar pasangan, 15 melintang. → **Angka ditulis.** Sumbernya gambar penjual dan instruksinya lebih spesifik. Diameter diputuskan 11 Okt. 2026: Ø6,6 pelat, Ø7,0 spacer.
3. **Jumlah lubang.** `koreksi_v3.md`: "pola lubang 2 × 2 per ujung" (8 lubang). `koreksi_v4.md` dan pengguna: 4 × M6, dua per ujung. → **4 lubang.**
4. **Luas bersih spacer.** `koreksi_v4.md` E: "28 × 28 setelah empat lubang". Menurut geometri, tiap spacer hanya menutup dua lubang di ujungnya. → **Hitungan utama memakai 2 lubang**, dan nilai 4 lubang ditampilkan sebagai pembanding (3.1).
5. **Tabel acuan `koreksi_v4.md` E:** dua sel berbeda > 2% (lihat 2.1). Satu karena pembulatan, satu karena acuan tidak konsisten (Al 8 / boss 24, δ titik beban 0,16 → 0,176). Angka tidak disamakan.
6. **Setup kalibrasi.** `koreksi_v3.md` F.1 meminta `kalibrasi_setup_v5.svg`, `koreksi_v4.md` D meminta bertanya dulu, dan pengguna meminta jangan dibuat. → **Tidak dibuat.** Setup kalibrasi yang berlaku sekarang adalah protokol dumbbell (`kalibrasi_protokol.svg`), menurut `koreksi_v4.md`.
7. **Zona genggam.** v4 ≈90 mm, sedangkan `koreksi_v3.md` dan `koreksi_v4.md` menyebut ≈85 mm. → **85 mm.**
8. **Kasus Ø5,5.** Diminta sebagai kasus hitungan, tetapi tidak cocok untuk baut M6 (2.4). → Dihitung dan diberi tanda, tidak dihapus.
9. **"Stop overload".** Frasa ini muncul tepat satu kali di tiap SVG v5, sebagai catatan "Stop overload belum dirancang".
10. **Ujung tetap/bebas fisik belum ditentukan** (saat itu), padahal gambar butuh satu ujung tetap. → Ujung tetap digambar di kiri sebagai konvensi gambar. Ujung fisik load cell (termasuk ujung kabel) yang dijadikan ujung tetap: ujung berkabel (keputusan 11 Okt. 2026). Desain mengharuskan kabel keluar di ujung tetap.

## 6. Keputusan terbuka dan nilai yang masih asumsi
Tidak ada lagi butir yang belum ditentukan. Nilai yang **diukur**: lebar 28 mm, tinggi 21,5 mm, ulir M6 tembus dari kedua sisi, minor 5,0–5,2 mm, Ø kabel 4 mm. Nilai dari **standar atau datasheet**: lubang lewat M6 6,6 mm (ISO 273), washer 1,6 mm (ISO 7089), overload 96/120 kgf (DS-LC1). Nilai **asumsi desain** (11–12 Okt. 2026) yang perlu dicek pada benda nyata:
- ~~jarak lubang 12 / 106 / 15 mm~~: terukur 31 Okt. 2026, cocok (Bagian 8.1);
- panjang kabel ≈28 cm, lubang kabel Ø5 mm di muka samping (terukur), **kedalaman lubang Ø5 ≈14 mm (asumsi, belum terukur)**, radius tekuk 10 mm, klem kabel takik 4,6 + zip tie 2,5 × 100 di dinding Spacer A (sejak 1 Nov. 2026; klip blok berlem tembak dari 11 Okt. 2026 dibatalkan);
- arah panah stiker beban pada benda nyata (digambar menunjuk ke arah B, dikoreksi pengguna 12 Okt. 2026);
- anggaran celah: load cell 0,5 mm, penutup 1,0 mm, margin 0,3 mm (sisa ≈0,64 mm dengan pelat 140 x 40 x 8 dan celah 3,0, sejak 8 Nov. 2026);
- torsi 3 N·m, koefisien torsi K 0,2 sampai 0,3 untuk baut stainless, dan prosedur kencang ulang;
- ridge PLA 30 × 3 × 5 mm, dua buah, jarak 22 mm, ditempel; alas karet 6 mm dengan satu slot 29 × 14 mm di bawah kedua kepala baut (untuk baut ISO 4017; flange bolt butuh slot lebih dari 29,2 × 14,2 dan alas lebih tebal dari tonjolan kepala, tetapkan setelah kepala diukur);
- uji baut M6 dengan tangan (tanpa dipaksa) belum dilakukan.

## 7. Pengecekan sebelum selesai
1. **Teks:** kedua SVG v5 dirender di Chromium. Batas semua teks diukur otomatis: 0 tumpang tindih, 0 terpotong. Hasil render juga diperiksa visual.
2. **Bersentuhan:** di potongan, semua bentuk milik A (pelat, Spacer A, baut + washer, penjepit) dan milik B (pelat, Spacer B, baut + washer, ridge, bidang tumpu) dicek tidak bersentuhan pada geometri tanpa beban. Cek di bawah beban ada di bagian 2.2–2.3.
3. **Label dan frasa:** semua gambar bertanda "TIDAK BERSKALA". "Stop overload" muncul satu kali per SVG. Tidak ada eyelet.
4. **Berkas lama:** berkas v4 tidak diubah.
5. **Revisi kabel Ø5 (sebelumnya):** kedua SVG v5 dirender ulang dan dicek otomatis: 0 teks tumpang tindih, 0 terpotong. Hasil render juga diperiksa visual. A dan B tetap tidak bersentuhan pada geometri tanpa beban (penjepit kabel hanya terikat ke A). Tidak ada angka panjang kabel di gambar.
6. **Pembaruan 11 Okt. 2026:** hanya dokumen ini yang diubah. SVG v5 tidak digambar ulang, jadi label Ø5 dan tinggi 22 mm di gambar sudah usang terhadap dokumen ini.
7. **Pembaruan 12 Okt. 2026:** hanya dokumen ini dan berkas instruksi Cowork yang diubah. Dua gambar klem v1 dan exploded v12 dirender di Chromium: 0 teks tumpang tindih, 0 terpotong. Semua angka klem cocok dengan `fusion_parameters_housing_grip.csv` dan 4.1. Exploded v12 dibandingkan baris demi baris dengan v11; hanya judul, panah stiker, legenda 19, dan catatan stiker yang berbeda. Tiga gambar v11 (potongan, setup, exploded) tidak berubah dari salinan yang sudah diperiksa.

## 8. Pembaruan hasil ukur (11 Okt. 2026)

| Item | Sebelumnya | Sekarang | Sumber |
|---|---|---|---|
| Ukuran load cell | 130 × 28 × 22 mm (gambar penjual, belum diukur) | **130 × 28 × 21,5 mm** | Pengukuran 10 Okt. 2026 (Catatan Proyek §15, C1) |
| Lubang pasang: jenis | Ulir M6 (gambar penjual) | **Berulir, tembus lurus** | Dilaporkan pengguna |
| Lubang pasang: diameter dalam (minor) | belum diukur | **5,0 dan 5,2 mm** (dua pembacaan jangka sorong) → ulir **M6** (minor M6 ≈ 4,9–5,2 mm). M5 ≈ 4,2 dan M8 ≈ 6,7 tidak cocok | Dilaporkan pengguna; uji baut M6 dengan tangan belum dilakukan |
| Lubang pasang: bibir kisi (chamfer) | — | **Ø8 mm** (hanya mulut lubang, bukan ukuran baut) | Dilaporkan pengguna |
| Diameter kabel | 5 mm (asumsi kerja; distributor 4 mm) | **4 mm** | Dilaporkan pengguna |
| Lubang kabel di load cell | Muka ujung (asumsi) | **Muka samping panjang, tengah tinggi, Ø5 mm, pusat 4,2 mm dari ujung** (tepi ke arah tengah 6,7 mm; kabel Ø4 mm longgar) | Sketsa dan pengukuran pengguna, 11 Okt. 2026 |
| Panah beban | Belum diketahui (C5) | **Di muka ujung bebas, menunjuk ke arah B** (searah tebal 21,5). Semula tercatat "ke bawah"; arah dikoreksi pengguna 12 Okt. 2026, lihat 4.1 | Sketsa pengguna, 11 Okt. 2026; koreksi 12 Okt. 2026 |
| Baut M6 × 25 untuk grup | Tercantum di daftar kebutuhan komponen kelompok | Cocok dengan ulir M6 | — |

Dampak pada dokumen ini: 1 (butir 1, 2, 12), 3.3, 3.4, 4.1, dan 6. Jarak antar lubang dan jarak dari ujung (106 / 15 / 12 mm) diukur kemudian, lihat 8.1. Ulir M6 dengan tembus lurus berarti baut M6 × 25 tidak mentok di dasar lubang.

### 8.1 Pola lubang pasang (terukur 31 Okt. 2026)

Cara ukur: jangka sorong digital. Celah dalam g (rahang luar dijepitkan ke logam di antara dua lubang) dan bentang luar S (rahang dalam dibuka ke tepi jauh kedua lubang), keduanya ke tepi yang sama. Jarak pusat = (S + g) / 2, jadi diameter lubang tidak ikut masuk hitungan.

| Ukuran | Pembacaan | Hasil | Gambar penjual |
|---|---|---|---|
| Pasangan melintang | g 9,5–10; S 20–20,5 | **15 ± 0,25** | 15 |
| Pasangan memanjang | g 100,5–101; S 110,5–111 | **105,75 ± 0,25** | 106 |
| Tepi lubang ke muka ujung | 9,5 di keempat lubang | pusat ≈ **12,1** (tepi + ≈2,6) | 12 |
| Tepi lubang ke muka samping | 3,5 di keempat lubang | pola di tengah lebar | di tengah |

Cek: (S − g) / 2 ≈ 5 mm di kedua pasangan, yaitu tepi ulir (minor M6), jadi semua pembacaan menyentuh tepi yang sama. Panjang: 12,1 + 105,75 + 12,1 ≈ 130 (cocok dengan panjang terukur). Lebar: 3,5 + 20,25 + 3,5 ≈ 27,3, kurang ≈0,7 dari 28; diduga rahang duduk di tepi load cell yang membulat. Karena keempat lubang sama, pola dianggap di tengah lebar, dan bengkel mengukur dari garis tengah, bukan dari tepi. Selisih 105,75 terhadap 106 masih di dalam kelonggaran lubang pelat Ø6,6 (0,3 per sisi), jadi CSV tetap 106. Ring Ø12 berjarak pusat 15 menyisakan celah 3 mm.

### 8.2 Posisi kabel (terukur 1 Nov. 2026)

Jangka sorong menjepit dari muka ujung load cell ke sisi kabel yang menghadap tengah load cell. Kabel didorong ke arah ujung: 6,7; ke arah tengah: 8,0; menggantung bebas: 6,7–7. Jadi tepi jauh lubang di 8,0 dan tepi dekat di 6,7 − 4 = 2,7: lubang ≈ Ø5,3 (Ø5 terukur ditambah kabel karet yang tertekan), **pusat 5,35 dari muka ujung**. Catatan 11 Okt. (pusat 4,2 dengan anggapan 6,7 = tepi lubang) diganti angka ini. Arah tinggi: pusat ≈ 10,2–10,5 dari muka bawah, dekat `LC_H` / 2. Klem kabel memakai `cable_x` = 5,3 (pusat lubang), sehingga kabel punya ruang ±0,65 di dalam lubang load cell dan tidak ditekuk di pangkalnya. Pangkal kabel: tidak ada karet yang menonjol; kabel keluar langsung dari lubang dengan chamfer di mulutnya (seperti lubang baut, tetapi lebih kecil), jadi dinding klem tidak perlu memberi ruang tambahan. Klem kabel (sejak 1 Nov. 2026): takik terbuka ke arah pelat B + zip tie 2,5 x 100 lewat terowongan di bantalan dinding klem Spacer A.

## 9. Riwayat versi sebelumnya (dipadatkan dari asumsi v2, v3, dan v4)
Berkas lama itu dihapus pada 12 Okt. 2026. Angka di tabel ini milik rancangan lama dan **tidak berlaku**; yang disimpan hanya keputusan dan alasannya.

| Versi | Rancangan | Keputusan dan temuan yang masih berguna |
|---|---|---|
| v2 (5 Okt.) | Batang PLA 7 mm, load cell 180 kg (147 × 30 × 22), A di bawah, celah ≈1,5 mm, tebal total ≈44 mm | Tiap batang bekerja sebagai kantilever ≈120 mm dan melentur puluhan mm (≈11 mm pada 200 N, ≈21 mm pada 400 N), jadi B menyentuh load cell dan PLA patah. Celah harus diukur dari titik tertinggi penutup putih, bukan permukaan logam. Posisi lubang, ulir, tembus/buta, arah beban, dan ujung kabel saat itu belum diketahui |
| v3 (5 Okt.) | Orientasi dibalik: A (telapak) di atas, B (jari) di bawah, ujung tetap di kiri; varian PLA (konsep awal, dengan peringatan kekakuan) dan varian logam (Al 8 mm / baja 6 mm) | Zona genggam ≈90 mm dan lengan tuas ≈60 mm (asumsi). PLA 7 mm: δ 19,2 mm dan σ 168 MPa pada L = 60, jauh di atas kekuatan PLA cetak (±30–50 MPa), patah sebelum 70 kgf. Al 8 mm: δ 0,55 mm, σ 129 MPa. Baja 6 mm: δ 0,46 mm, σ 229 MPa (mendekati luluh baja lunak ≈235 MPa). **Celah di ujung jauh lebih kritis daripada di titik beban** (pelat terus melentur melewati titik beban): Al 8 mm 1,56 mm di ujung load cell, melampaui celah 1,5 mm. Pengecekan geometri tanpa beban saja tidak cukup; wajib cek defleksi. Varian logam butuh akses bengkel, berat Al 8 mm ≈110 g per batang, baut tidak lagi masuk PLA. Ridge dan eyelet PLA tempel: kekuatan eyelet untuk kalibrasi gantung perlu dicek |
| v4 (6 Okt.) | Al 10 mm, celah 2,5 mm, tebal total ≈52 mm, load cell 147 mm | Eyelet dihapus, diganti bidang tumpu kalibrasi di antara dua ridge. Kata "stop overload" hanya dipakai sebagai catatan "belum dirancang" (celah hanya celah bebas-sentuh). Al 10 mm: δ titik beban 0,28 mm, δ ujung jauh 0,80 mm, σ 82 MPa, σ_net 130–145 MPa; Al 8 mm σ_net 203–227 MPa tidak lolos; baja 8 mm σ_net 227 MPa ≈97% luluh. Tebal ≈52 mm melampaui anggaran awal 40–48 mm (cek kenyamanan dengan prototipe kardus). Massa B ≈138 g jauh di atas tolerance S1, jadi tare dan pengukuran harus pada orientasi yang sama. Kalibrasi dengan rakitan dibalik (A di bawah, B di atas, beban menekan B ke A) karena beban yang digantung menarik B menjauhi A, kebalikan remasan |

Keterbatasan setup kalibrasi lama yang masih relevan: bidang tumpu di antara dua ridge hanya ≈22 mm, jadi alas beban harus muat atau perlu adaptor; saat A ditumpu meja, lenturan A saat digenggam tidak teruji, hanya celah sisi B yang teruji pada beban tinggi.

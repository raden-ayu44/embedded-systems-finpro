# Daftar isian datasheet (rak "Komponen & Datasheet")

Cara pakai: isi semua baris yang bertanda **ISI**. Baris yang sudah terisi berasal dari dokumen yang sudah kita baca di chat; tetap cek ulang ke dokumen aslinya sebelum disitasi. Satu komponen = satu blok. Kalau datasheet-nya tidak ada (hanya pesan penjual), tulis status "bukan dokumen resmi" dan jangan disitasi sebagai sumber akademik.

## A. Isian dasar untuk SETIAP datasheet

| Isian | Keterangan |
|---|---|
| Kode | Mis. DS-HX1 (rak + komponen + nomor) |
| Komponen | Nama dan nomor tipe persis seperti di barang |
| Pabrikan | Pembuat asli (bukan toko) |
| Judul dokumen | Persis seperti di halaman judul |
| Nomor / revisi / tanggal | Dari halaman judul atau footer |
| Sumber (URL atau nama berkas) | Tempat dokumen diperoleh |
| Status | Resmi pabrikan / tidak resmi (toko, reseller) / pesan penjual |
| Parameter yang dipakai | Nama, nilai, satuan, kondisi uji, halaman atau tabel |
| Dipakai di bagian | Keputusan atau perhitungan yang bergantung padanya |
| Diverifikasi dengan | Datasheet saja / diukur sendiri (alat dan hasil) |

## B. Daftar komponen

### DS-LC: Load cell (Eagle Weigh CZL 601, 80 kg)

| Isian | Nilai |
|---|---|
| Kode | DS-LC1 |
| Pabrikan | Eagle Weigh (**ISI**: cek nama pabrikan persis di PDF) |
| Dokumen | CZL-601.pdf (resmi), CZL-601-nonofficial.pdf dan gambar reseller (tidak resmi) |
| Nomor / revisi / tanggal | **ISI** |
| Kapasitas | 80 kg (**ISI**: konfirmasi ke penjual bahwa yang dikirim 80 kg) |
| Rated output | 2,0 ± 0,2 mV/V (pesan penjual dan datasheet) |
| Non-linearity / creep | 0,02% FS / 0,0016% FS (**ISI**: halaman, dan waktu uji creep) |
| Kelas / proteksi | C3 / IP67 |
| Overload aman / destruktif | 150% FS / 200% FS |
| Eksitasi | Rekomendasi 10-15 V; **ISI**: apakah 5 V diperbolehkan dan akurasinya (tanya penjual) |
| Resistansi input / output | 401 ± 10 ohm / 350 ± 5 ohm (**ISI**: ukur dengan multimeter) |
| Kabel | 4 kawat: merah E+, hitam E-, hijau S+, putih S-; panjang 28 cm; **ISI**: kawat kelima (GND/shield?) |
| Geometri | 130 x 28 x 22 mm; lubang 4 x M6, 12 mm dari tiap ujung, jarak antar pasangan 106 mm, 15 mm melintang (**ISI**: ukur dengan jangka sorong) |
| Lubang tembus atau buta, kedalaman ulir | **ISI** (tanya penjual atau ukur) |
| Posisi keluar kabel | **ISI** |
| Status | Gambar reseller bukan dokumen resmi; nilai resmi hanya dari PDF pabrikan |
| Dipakai di bagian | Kapasitas dan noise (Bagian 8 dan 9), geometri housing, kalibrasi |
| Diverifikasi dengan | **ISI** (multimeter, jangka sorong, uji noise 1000 sampel) |

### DS-HX: Modul HX711

| Isian | Nilai |
|---|---|
| Kode | DS-HX1 |
| Pabrikan chip | Avia Semiconductor (**ISI**: cek di datasheet) |
| Dokumen | HX711 datasheet (resmi); **ISI**: nomor revisi dan tanggal |
| Skala penuh | +-0,5 x AVDD / gain; +-19,5 mV pada 5 V, gain 128 |
| Noise input | 50 nV rms pada 10 SPS, 90 nV rms pada 80 SPS |
| Settling | 400 ms (10 SPS), 50 ms (80 SPS) |
| Common-mode input | AGND + 1,2 V sampai AVDD - 1,3 V |
| Catu daya | 2,6-5,5 V |
| Drift | Offset +-6 nV/derajat C, gain +-5 ppm/derajat C, PSRR 100 dB |
| Papan modul yang dipakai | **ISI**: merek atau toko, foto papan |
| Pin RATE (10 atau 80 SPS) pada papan | **ISI**: dihubungkan ke apa, bisa diubah? |
| Tegangan AVDD yang sebenarnya di papan | **ISI**: ukur dengan multimeter (3,3 V atau 5 V) |
| Dipakai di bagian | Perhitungan noise (0,4 g / 0,7 g), kalibrasi, firmware |
| Diverifikasi dengan | Uji noise E1 pada 10 SPS, 706 sampel (15 Okt. 2026): σ 38,2 counts. 80 SPS: **ISI** |

### DS-ESP: ESP32

| Isian | Nilai |
|---|---|
| Kode | DS-ESP1 |
| Modul / papan | **ISI**: nama persis (mis. ESP32 DevKit, tipe modul) |
| Dokumen | **ISI**: datasheet Espressif (versi dan tanggal) dan skema papan |
| Tegangan logika | **ISI** (3,3 V) |
| Pin yang dipakai (HX711, LCD, tombol, LED) | **ISI** (rujuk [desain-pcb-hgd.md](../pcb/desain-pcb-hgd.md)) |
| Arus total yang diperlukan | **ISI** |
| Dipakai di bagian | Platform (Bagian 8), anggaran daya (belum dihitung ulang; Bagian 16) |

### DS-LCD: LCD 1602 + backpack I2C

| Isian | Nilai |
|---|---|
| Kode | DS-LCD1 |
| Pengontrol dan chip I2C | **ISI** (mis. HD44780 dan PCF8574 atau setara) |
| Alamat I2C | 0x27 (pindai, 15 Okt. 2026) |
| Tegangan dan arus | Catu 5 V (VIN); di 3,3 V kontras gagal. SDA/SCL diam 4,93 V → level shifter BSS138 (DS-LLS1). Arus: **ISI** |
| Dimensi PCB dan jendela tampilan | PCB 80 x 36; layar 64,5 x 14,0; tinggi total 16,9 (tab LCD I2C di [tabel-pengukuran-hgd.xlsx](../pengukuran/tabel-pengukuran-hgd.xlsx)) |
| Dokumen | **ISI** |

### DS-LED, DS-RES, DS-BTN: komponen kecil

| Komponen | Isian |
|---|---|
| LED 5 mm | **ISI**: warna, tegangan maju, arus maksimum, dokumen |
| Resistor pembatas | 220 ohm, 1/4 W; (3,3 - 2,0) / 220 ≈ 6 mA (Vf LED **ISI**) |
| Tombol tactile | **ISI**: tipe, arus kontak, dimensi, dokumen |

### DS-PWR: catu daya

| Isian | Nilai |
|---|---|
| Sumber (USB, baterai, adaptor) | USB saja; tanpa baterai (15 Okt. 2026) |
| Regulator (jika ada) | **ISI**: tipe, noise keluaran |
| Kapasitor decoupling untuk HX711 | 100 nF keramik + 10 µF elektrolit di +3V3 dekat J1; 10 µF + 100 nF di +5V dekat J2 (LCD) |

### DS-MAT: bahan

| Isian | Nilai |
|---|---|
| Aluminium pelat | **ISI**: paduan (6061-T6?), modulus E, kekuatan luluh, sumber (kita memakai E = 69 GPa) |
| Filamen PLA | **ISI**: merek, modulus, suhu transisi, catatan creep |
| Baut M6 | **ISI**: kelas kekuatan, panjang (M6 x 20?) |

## C. Hal yang perlu ditanyakan ke penjual load cell

1. Lubang M6 tembus atau buta, dan kedalaman ulirnya?
2. Eksitasi 5 V diperbolehkan? Berapa akurasinya pada 5 V?
3. Apakah kabel kelima adalah GND atau shield?
4. Harga dan stok versi 80 kg?
5. Posisi keluar kabel dan panjang blok pada kedua ujung?

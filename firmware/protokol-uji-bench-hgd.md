---
title: Protokol Uji Bench - Hand Grip Dynamometer

---

# Protokol Uji Bench - Hand Grip Dynamometer (v2)
**Proyek akhir - embedded system**

> Langkah uji modul di breadboard dengan sketsa [`uji_bench_hgd`](sketches/uji_bench_hgd/uji_bench_hgd.ino): U4 (I2C dan LCD), U7 (HX711), U8 (tombol dan LED), E1 (noise diam), uji arah tekan, dan pengulangan tare. Tabel uji dan konstanta ada di Bagian 3 dan 11 di [alur-program-hgd.md](alur-program-hgd.md); perangkat keras di [desain-pcb-hgd.md](../pcb/desain-pcb-hgd.md); induk: [catatan-proyek-hgd.md](../catatan-proyek-hgd.md).

---

## 1. Alat dan sambungan

| Bagian | Sambungan | Catatan |
|---|---|---|
| ESP32 DevKit V1 | USB ke laptop; Serial Monitor 115200 baud | Satu-satunya catu; tanpa baterai |
| HX711 | VCC ke 3V3, GND, DT ke GPIO18, SCK ke GPIO19 | **Bukan 5 V** (level DT). Jumper DT/SCK harus rapat: jumper longgar menghasilkan pembacaan bergeser bit dan galat `ADC_SATURASI` |
| Load cell | Eksitasi hitam/putih, sinyal merah/hijau ke terminal HX711 | Dijepit di meja, tanpa beban, kabel tidak bergerak |
| LCD + backpack | VCC ke VIN (5 V), SDA/SCL **lewat level shifter** ke GPIO21/22 | Lihat peringatan di bawah |
| Tombol | GPIO25 ke GND (kaki diagonal) | Pull-up internal |
| LED | GPIO27 → R1 220 ohm → anoda; katoda ke GND | - |

> **Risiko kerusakan: SDA/SCL backpack di VIN terukur 4,93 V**, di atas batas mutlak GPIO ESP32 (≈ 3,6 V). SDA/SCL LCD hanya ke sisi HV level shifter; GPIO21/22 hanya ke sisi LV.

Wiring level shifter (sesuai skema `hgd-v1`, J3/J4):

| Pin shifter | Ke |
|---|---|
| HV | VIN (5 V) |
| HV1 / HV2 | SDA / SCL backpack LCD |
| LV | 3V3 |
| LV1 / LV2 | GPIO21 / GPIO22 |
| GND (sisi HV dan LV) | GND |
| HV3, HV4, LV3, LV4 | Kosong |

Cek sebelum uji: dengan catu USB dan LCD diam, LV1/LV2 ≈ 3,3 V dan HV1/HV2 ≈ 4,9 V. Bila LV1/LV2 terbaca ≈ 4,9 V, cabut USB: sisi HV/LV tertukar.

Perintah serial: `t` tare, `k` rata-rata 10 raw, `p` status, `e` E1 60 s, `n` balik tanda, `l` LED, `s` pindai I2C, `h` bantuan. Tombol: TAP = LED nyala/mati, TAHAN 1,5 s = tare ulang.

Sketsa mencetak satu baris `raw=... kg=... win_avg=... spread=... STABLE` tiap 250 ms (4 baris per detik). Nilai kg memakai skala awal 53.687 counts/kg (belum dikalibrasi, ±10 %).

---

## 2. Urutan uji

| Langkah | Kegiatan | Catat | Lulus bila |
|---|---|---|---|
| 1 | Periksa sambungan dengan catu terputus (Bagian 1) | - | HX711 di 3V3; SDA/SCL tidak langsung ke backpack 5 V |
| 2 | Sambungkan USB, buka Serial Monitor. Baca baris boot | Alamat I2C; status HX711 | **U4:** `found 0x27`. **U7:** `HX711: DT went low, module responds` |
| 3 | **Pemanasan ≥ 10 menit**: biarkan menyala, load cell diam, Serial Monitor tetap merekam | Rata-rata raw tiap 75 s (300 baris) | Rata-rata blok berturut-turut dalam ±25 counts (±0,5 g). Hasil 16 Okt. 2026: stabil setelah 4-5 menit |
| 4 | Tare (`t`) | Offset dan sebaran 10 sampel | Sebaran ≤ 120 counts (≈ 2 g) |
| 5 | **E1** (`e`): 60 s, jangan sentuh meja atau kabel | n, mean raw, σ, peak-to-peak, saran `THRESHOLD_ONSET_KG` | n ≈ 700; tidak ada baris `ERR`; σ ≤ 50 counts (≈ 1 g); saran = 1,00 kg |
| 6 | **Uji arah tekan**: tekan load cell searah genggaman 3 kali; tahan yang terakhir ≥ 5 s; lepas dan diam 60 s | Puncak kg tiap tekan; nilai 60 s setelah lepas | Tanda **positif** saat ditekan (bila negatif: `n`, catat); kembali ke nol dalam ±0,01 kg setelah 60 s |
| 7 | **U8**: 10 TAP, lalu 10 TAHAN (≥ 1,5 s) | Jumlah baris `TAP #` dan `HOLD #` | Tepat 10 TAP dan 10 HOLD; tidak ada TAP tambahan saat tombol dilepas |
| 8 | Tare ulang 3 kali (`t`), selang ≥ 30 s | Tiga offset | Selisih offset ≤ 100 counts (≈ 2 g) |
| 9 | Simpan seluruh isi Serial Monitor sebagai teks (nama berkas memuat tanggal) | - | Berkas lengkap dari baris boot sampai akhir |

Urutan ini sengaja: E1 setelah pemanasan (syarat E1 di Bagian 3.3 di [alur-program-hgd.md](alur-program-hgd.md)), uji tekan setelah E1 supaya beban tidak mengganggu data noise.

---

## 3. Membaca log

- **Baris rusak** (awal baris berisi karakter acak, ekor `... STABLE` masih terbaca): gangguan tautan serial, bukan pembacaan HX711. Abaikan; jangan dihitung sebagai data. Firmware utama memakai checksum dan ACK (Bagian 4.8 di [alur-program-hgd.md](alur-program-hgd.md)).
- **`ERR: ADC_SATURASI` berulang** dengan raw ≈ 8.368.000: masukan HX711 mengambang atau jumper DT/SCK longgar. Hentikan, perbaiki sambungan, ulangi dari langkah 4.
- **Tanda `STABLE`** memakai `THRESHOLD_STABIL_KG` = 0,5 kg (nilai awal E2). Pada uji tekan, baris dengan selisih 5 sampel sampai 0,25 kg masih ditandai STABLE; ini bukan galat, tetapi masukan untuk E2.
- **Turun sesaat sebelum tekan** (mis. −0,09 kg): sentuhan pada meja atau kabel, bukan galat sensor.
- **Pemanasan:** hitung rata-rata raw per 300 baris (75 s) dari awal log sampai baris `[E1] noise test for 60 s`.

---

## 4. Hasil

| Tanggal | Kondisi | E1 (n; σ; p-p) | Pemanasan | Arah tekan | U8 | Tare ulang | Catatan |
|---|---|---|---|---|---|---|---|
| 15 Okt. 2026 (percobaan 1) | Dingin | Tidak valid | - | - | - | - | Jumper DT/SCK longgar: `ADC_SATURASI`, σ palsu 1,2 juta counts |
| 15 Okt. 2026 (ulang) | Dingin, tanpa pemanasan | 706; 38,2 counts (0,0007 kg); 206 counts (0,004 kg) | - | - | 3 TAP, 1 HOLD benar | - | Drift ≈ 60 counts dalam ≈ 2 menit |
| **16 Okt. 2026** | **Pemanasan ≈ 11 menit** | **705; 35,0 counts (0,0007 kg); 206 counts (0,004 kg)** | Drift ≈ 80 counts (≈ 1,5 g), stabil setelah 4-5 menit (blok 75 s dalam ±25 counts) | Positif; puncak ≈ 2,5-2,9, ≈ 2,5-2,9, ≈ 3,1 kg (tahan ≈ 9 s); setelah lepas +0,012 kg, turun ke +0,004 kg dalam ≈ 12 s | 2 TAP, 2 HOLD benar | Offset −39.030 sampai −38.978 (selisih 84 counts); sebaran tare 66-104 counts | **Hasil E1 resmi.** ≈ 10 baris serial rusak, diabaikan |

Kumulatif U8: 5 TAP dan 3 HOLD benar; uji 10/10 (langkah 7) belum.

Data lengkap (semua baris E1 15 dan 16 Okt. 2026, pemanasan, uji tekan, log mentah) ada di [e1-noise-hgd.xlsx](../pengukuran/e1-noise-hgd.xlsx), dengan lembar kosong "E1 rakitan akhir" untuk dibandingkan setelah housing dirakit.

---

## 5. Perubahan

| Versi | Tanggal | Perubahan |
|---|---|---|
| v2 | 17 Okt. 2026 | Level shifter terpasang: tabel wiring shifter dan cek tegangan sisi LV; aturan cabut SDA/SCL dihapus |
| v1 | 16 Okt. 2026 | Protokol pertama, dari uji bench 15 dan 16 Okt. 2026 |

---

*Catatan kerja proyek akhir mata kuliah embedded system. Bukan dokumen yang dinilai.*

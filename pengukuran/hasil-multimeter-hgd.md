---
title: Hasil Pengukuran Multimeter - Hand Grip Dynamometer

---

# Hasil Pengukuran Multimeter - Hand Grip Dynamometer (v1)
**Proyek akhir - embedded system**

Semua pembacaan multimeter proyek di satu tempat, urut tanggal: tegangan, resistansi, arus, kontinuitas, dan kapasitansi, dari komponen yang baru tiba (10 Okt. 2026) sampai purwarupa breadboard lengkap (25 Okt. 2026). Tiap bagian menyebut kondisi, rentang alat, hasil, batas yang dipakai, dan dokumen asalnya. Dokumen asal tetap memuat pembacaannya; berkas ini rangkumannya.

## 1. Alat

- **Multimeter digital DT9205A.** Rentang, akurasi, dan peringatan alat ada di DS-INS1 [datasheet-library.md](../datasheet/datasheet-library.md). Yang paling sering dipakai: 20 V DC (± (0,5 % + 2 digit), resolusi 10 mV), 200 mV DC (resolusi 0,1 mV), 2 kΩ (± (0,8 % + 2 digit)), 200 mA dan 2 mA lewat jack **mA**.
- **Kapasitansi diukur lewat soket Cx**, bukan lewat probe (lewat probe alat menampilkan `1`). Kapasitor dikosongkan dulu.
- `1` di layar = di luar rentang (atau terputus pada mode Ω).

## 2. Ringkasan tegangan rail

| Rail | 10 Okt. 2026: DevKit saja | 17 Okt. 2026: shifter saja | 24 Okt. 2026: breadboard lengkap, sisi ESP32 | 24 Okt. 2026: breadboard lengkap, pin modul |
|---|---|---|---|---|
| 3V3 | 3,30 V | 3,26 V (LV1/LV2) | 3,25 V (C1, C2, GPIO25) | 3,17 V (VCC HX711, LV1/LV2) |
| 5 V (VIN) | 5,03 V | 4,98 V (HV1/HV2) | 4,95 V (C3, C4) | 4,91-4,94 V (HV1/HV2, VCC backpack) |

Tegangan turun sedikit seiring beban bertambah, tetap di dalam rentang regulator 3,3 V dan USB 5 V ± 5 %.

**Penelusuran selisih 3V3 (25 Okt. 2026).** Pembacaan di **lubang rail yang kosong** sempat memberi 3,15 V lalu 3,08 V (setelah DevKit ditekan ke breadboard), padahal logam pin 3V3 DevKit 3,26-3,28 V. Itu **salah cara ukur**, bukan masalah rangkaian: lubang kosong bisa berada pada klip yang longgar atau di seberang putusan rail, sehingga yang terukur adalah lubangnya. Diulang di **lubang tepat di sebelah kabel yang membawa arus**, dengan kabel perpanjangan di probe (kabel dipasang saat USB dicabut, lalu USB dicolok, lalu dibaca), 20 V DC:

| Titik | Hasil |
|---|---|
| Logam pin 3V3 DevKit (bagian atas header) | 3,26-3,28 V |
| GPIO25 (pull-up, tanpa arus) | 3,25 V |
| Rail 3V3 atas: lubang kabel dari pin 3V3 | 3,25-3,26 V (paling stabil 3,26) |
| Rail 3V3 atas: R2 | 3,25 V |
| Rail 3V3 atas: C2 | 3,25-3,26 V |
| Rail 3V3 bawah: lubang kabel dari pin 3V3 dan kabel ke VCC HX711 | 3,22 V (sekali 3,25) |
| Rail 3V3 bawah: LV shifter | 3,22 V (sekali 3,18) |
| Rail 3V3 bawah: C1 | 3,22 V (sekali 3,25, sekali 3,18) |

- **Rail atas sama dengan pin** (D2, 6,7 mA): kontak pin 3V3 DevKit ke breadboard baik.
- **Rail bawah ≈ 0,04 V lebih rendah** (HX711 + LV, 10,4 mA): jatuh tegangan di jumper 2 dan kontaknya (≈ 4 Ω). Tidak berbahaya; HX711 bekerja pada 2,6-5,5 V. Pembacaan sekali-sekali 3,18/3,25 V adalah kontak probe yang berkedip.
- Dua pembacaan 200 mV sebelumnya (kolom pin 3V3 ↔ rail bawah 7,2 mV; rail bawah ↔ VCC HX711 4,2 mV) diambil sebelum cara ini, dan titik rail-nya mungkin lubang kosong; tidak dipakai untuk kesimpulan.
- **Pelajaran cara ukur:** ukur tegangan rail di lubang yang bersebelahan dengan kabel atau kaki komponen yang membawa arus, bukan di lubang kosong.

## 3. 10 Okt. 2026: papan DevKit dan load cell

**Tegangan DevKit** (20 V DC; DevKit dicatu USB laptop, tanpa modul). Asal: "Hasil ukur multimeter" di [datasheet-library.md](../datasheet/datasheet-library.md).

| Titik | Hasil | Batas (hasil ± ketidakpastian alat) | Kesimpulan |
|---|---|---|---|
| 3V3 ↔ GND | **3,30 V**, stabil | 3,26-3,34 V | sesuai regulator 3,3 V |
| VIN ↔ GND | **5,02-5,04 V** (paling stabil 5,03 V) | 4,98-5,08 V | di dalam USB 5 V ± 5 % |

**Resistansi load cell** (2 kΩ; load cell belum disolder, tanpa catu, tidak ditekan; diukur dua kali, konsisten). Asal: DS-LC1 di [datasheet-library.md](../datasheet/datasheet-library.md) dan tab Load cell baris C9 [tabel-pengukuran-hgd.xlsx](tabel-pengukuran-hgd.xlsx).

| Pasangan | Kabel | Hasil | Datasheet | Kesimpulan |
|---|---|---|---|---|
| **E+ ↔ E−** (eksitasi, masukan) | hitam-putih | **393 Ω** | 401 ± 10 Ω | lolos |
| **A+ ↔ A−** (sinyal, keluaran) | hijau-merah | **347 Ω** | 350 ± 5 Ω | lolos |
| Silang | merah-hitam, hitam-hijau | 306 Ω (merah-hitam sempat 400 Ω) | 260-310 Ω (hitungan jembatan) | konsisten |
| Silang | hijau-putih, merah-putih | 260 Ω | 260-310 Ω | konsisten |

- **Temuan:** warna kabel berbeda dari datasheet (datasheet: merah/hitam eksitasi). Pasangan eksitasi ditemukan dari resistansi terbesar.
- **Penyolderan ke HX711 yang bekerja:** hitam **E+**, putih **E−**, hijau **A+**, merah **A−**. Urutan A+/A− terbukti benar: gaya tekan terbaca positif.

## 4. 15 Okt. 2026: backpack LCD (uji U5)

Asal: DS-I2C1 di [datasheet-library.md](../datasheet/datasheet-library.md), Bagian 6 [desain-pcb-hgd.md](../pcb/desain-pcb-hgd.md).

| Titik | Hasil | Kesimpulan |
|---|---|---|
| SDA dan SCL saat diam, backpack dicatu 5 V (20 V DC) | **4,93 V** | pull-up backpack ke VCC 5 V; di atas batas GPIO ESP32 (≈ 3,6 V) → **level shifter wajib** |
| Kontras (pengamatan, bukan pembacaan) | tidak terbaca pada 3,3 V walau trimpot mentok; terbaca pada 5 V | LCD butuh 5 V |

## 5. 17 Okt. 2026: pull-up I2C dan level shifter

**Resistansi** (Ω; tanpa catu, modul dilepas). Asal: DS-I2C1 dan DS-LLS1 di [datasheet-library.md](../datasheet/datasheet-library.md).

| Titik | Hasil |
|---|---|
| Backpack SDA ↔ VCC | **4,67 kΩ** (pull-up 4,7 kΩ ke VCC) |
| Shifter LV1/LV2 ↔ LV | **9,86 kΩ** (SMD `103`) |
| Shifter HV1/HV2 ↔ HV | **9,90 kΩ** (SMD `103`) |
| Kontinuitas pin bertetangga setelah header disolder | jembatan solder LV1-LV2 dan HV1-HV2 **ditemukan dan diperbaiki**; setelahnya tidak ada yang berbunyi |

Hitungan dari angka ini (bukan pembacaan): segmen LV 9,86 kΩ; segmen HV 9,90 ‖ 4,67 = 3,17 kΩ (≈ 1,6 mA pada 4,93 V). Keduanya di dalam batas 1-11,8 kΩ, jadi PCB tanpa pull-up tambahan.

**Tegangan shifter** (20 V DC; hanya HV, LV, GND tersambung, tanpa LCD). Asal: DS-LLS1, [protokol-uji-bench-hgd.md](../firmware/protokol-uji-bench-hgd.md).

| Titik | Hasil |
|---|---|
| LV1 = LV2 | **3,26 V** |
| HV1 = HV2 | **4,98 V** |

## 6. 24 Okt. 2026: arus tiap rail (uji S3)

Multimeter seri menggantikan kabel catu tiap beban, jack mA. Diam = `SIAP`; puncak = genggaman stabil dengan D1 menyala. Langkah lengkap: [protokol-arus-rail-hgd.md](protokol-arus-rail-hgd.md); hasil dan perbandingan S3: Bagian 5.1 [desain-pcb-hgd.md](../pcb/desain-pcb-hgd.md).

| Pembacaan | Beban | Rentang | Diam (mA) | Puncak (mA) |
|---|---|---|---|---|
| A1 | rail 3V3 atas: D2 + R2 | 200 mA | **6,7** | 6,7 |
| A2 | rail 3V3 bawah: HX711 + load cell + LV shifter | 200 mA | **10,4** | 10,4 |
| B1 | VIN: LCD + backpack | 200 mA | **26,4** | 27,0 (berkedip 26,0-27,0) |
| B2 | VIN: HV shifter | 2 mA | **< 0,01** | 0,1-0,2 |
| **Beban luar** | | | **43,5** | **44,3** |

- Total dari USB ≈ 100-150 mA dengan **estimasi** papan DevKit (tidak terukur); S3 (rata-rata ≤ 150, puncak ≤ 600 mA) terpenuhi.
- Catatan cara: lengan goyah membuat B1 berkedip 25,4-26,2 mA (stabil 26,4); B1 pada rentang 2 mA menampilkan `1`/`-1` dan LCD kosong (meter menjatuhkan tegangan); probe tertukar hanya membalik tanda.

**Kontinuitas saat pengukuran arus** (USB dicabut). Asal: Bagian 2 dan 7 protokol arus rail.

| Pemeriksaan | Hasil |
|---|---|
| Ujung jumper 1 yang dicabut ↔ rail atas | `1` (terputus) |
| Ujung jumper 2 yang dicabut ↔ rail bawah | > 1000 Ω (hanya lewat komponen) |
| Kedua ujung bebas jumper 3V3 ↔ satu sama lain | bunyi, 0 Ω (pin 3V3 yang sama) |
| R2 ↔ rail atas / rail bawah | bunyi / tidak |
| VCC HX711 dan LV shifter ↔ rail bawah / rail atas | bunyi (0 Ω) / tidak |
| VCC backpack ↔ VIN / rail 3V3 | bunyi (≈ 0 Ω) / tidak |
| 3V3 ↔ VIN | 500-1500 Ω, berubah-ubah (regulator dan kapasitor DevKit, bukan hubung singkat) |

## 7. 24 Okt. 2026 malam: kapasitor dipindah ke posisi skema

C3 dan C4 dipindah dari +3V3 ke +5V (VIN) sesuai skema. Asal: Bagian 2.1 [protokol-arus-rail-hgd.md](protokol-arus-rail-hgd.md).

| Pemeriksaan | Hasil |
|---|---|
| Kontinuitas, USB dicabut (7 pemeriksaan) | semua lolos: 3V3 ↔ VIN 500-1500 Ω; VIN ↔ GND dan 3V3 ↔ GND tidak bunyi; C3/C4 ↔ VIN bunyi; C3/C4 ↔ rail 3V3 lama tidak bunyi |
| C1, C2 (+3V3), 20 V DC | **3,25 V** |
| C3, C4 (+5V), 20 V DC | **4,95 V** (tanda +, jadi C3 tidak terbalik) |

## 8. 24 Okt. 2026 malam: tegangan di pin modul (breadboard lengkap, keadaan `SIAP`)

20 V DC, probe hitam di GND, kecuali yang ditandai "di antara".

| Titik | Hasil | Kesimpulan |
|---|---|---|
| **E+** HX711 (U6) | **2,98 V** | regulator modul (E+ = AVDD), ≈ 0,2 V di bawah VCC modul |
| **E−** HX711 (U6) | **0,02 V** | sehat (papan cacat ≈ 0,6 V) |
| VCC HX711 | 3,17 V | di dalam 2,6-5,5 V |
| LV1 / LV2 (sisi GPIO21/22), LCD tersambung | 3,17 V | **pin I2C ESP32 melihat 3,2 V, bukan 5 V**: shifter melindungi |
| HV1 / HV2 | 4,91 / 4,93 V | |
| VCC backpack LCD | 4,93-4,94 V | sama dengan 15 Okt. 2026 |
| GPIO25 (tombol dilepas) | 3,25 V | pull-up internal bekerja |
| Di antara kaki R2 | 1,42 V | 1,42 V / 208 Ω = **6,8 mA**; terukur seri 6,7 mA (A1) |
| Di antara kaki D2 | 1,78 V | Vf LED daya pada 6,8 mA; R2 + D2 = 3,20 V = rail atas |
| A+ ↔ A− tanpa beban (200 mV) | 0 | sesuai: offset nol ≈ 39 000 counts ≈ 0,05 mV, di bawah resolusi 0,1 mV |

## 9. 24 Okt. 2026 malam: komponen dilepas dari breadboard

USB dicabut, komponen dicabut supaya rangkaian lain tidak ikut terukur.

| Komponen | Rentang | Hasil | Nominal | Kesimpulan |
|---|---|---|---|---|
| R1 | 2 kΩ | **213 Ω** | 220 Ω ± 5 % (209-231) | lolos |
| R2 | 2 kΩ | **208 Ω** | 220 Ω ± 5 % | lolos dengan ketidakpastian alat (± 4 Ω) |
| C1, C4 (keramik `104`) | 200 nF (soket Cx) | **≈ 107 nF** | 100 nF | lolos |
| C2, C3 (elektrolit 10 µF 16 V) | 20 µF (soket Cx) | **≈ 10 µF** (naik perlahan saat dibaca) | 10 µF (± 20 % umum) | lolos |
| D1, D2 (mode dioda) | | tidak stabil (D1 1299; D2 800 sampai > 1000) | ≈ 1600-1800 | **tidak dipakai**; Vf diukur di rangkaian (Bagian 8 dan 10) |

Pembacaan kapasitor pertama memakai rentang yang salah (20 nF untuk keramik, 200 µF untuk elektrolit) dan menunjukkan angka yang sama dengan koma bergeser satu posisi; diulang pada rentang yang benar dan cocok dengan cetakan.

## 10. 25 Okt. 2026: pembacaan dua tangan (dengan pembantu)

20 V DC kecuali yang ditandai.

| Titik | Keadaan | Hasil | Kesimpulan |
|---|---|---|---|
| GPIO25 | tombol ditahan | **0 V** | tombol menarik pin ke GND |
| GPIO27 | D1 menyala (genggaman stabil) | **3,07-3,08 V** | keluaran HIGH dengan beban LED |
| Di antara kaki R1 | D1 menyala | **1,03-1,04 V** | 1,035 V / 213 Ω = **4,9 mA** (perkiraan dokumen lama ≈ 6 mA) |
| Di antara kaki D1 | D1 menyala | **1,92 V** | Vf LED 5 mm pada 4,9 mA |
| A+ ↔ A− (200 mV) | genggaman ≈ 4 kg | **0,2-0,3 mV** | perkiraan 0,075 mV/kg × 4 = 0,30 mV |
| A+ ↔ A− (200 mV) | puncak 6,04 kg | **0,3 mV** (berkedip 0,2) | perkiraan 0,45 mV; di dalam ketidakpastian ± 0,2 mV, dan meter (2-3 pembaruan/s) melewatkan puncak singkat |

- R1 + D1 = 2,96 V terhadap GPIO27 3,07 V: selisih dari tiga genggaman terpisah dan ketidakpastian alat.
- Sinyal jembatan hanya ≈ 0,075 mV per kg (2,0 mV/V × 2,98 V / 80 kg). Multimeter melihat langkah 0,1 mV; satu count HX711 (gain 128, 24 bit) ≈ 1,4 nV, sekitar 70 000 kali lebih halus. Multimeter cukup untuk membuktikan jembatan bekerja dan arahnya benar, **tidak cukup untuk kalibrasi**.
- Nilai kg memakai skala nominal 53 687 counts/kg (belum terkalibrasi).

## 11. Belum diukur

- Arus papan DevKit sendiri (ESP32, CP2102, regulator): hanya estimasi (Bagian 5.1 desain PCB); butuh pemutusan jalur 5 V USB.
- Vf D1 dan D2 dengan mode dioda (tidak stabil pada alat ini; nilai di rangkaian dipakai).

## 12. Riwayat versi

| Versi | Tanggal | Perubahan |
|---|---|---|
| v1 | 25 Okt. 2026 | Rangkuman semua pembacaan multimeter 10-25 Okt. 2026, termasuk U6 (E+/E−), tegangan pin modul, komponen lepas, pembacaan dua tangan, dan penelusuran selisih tegangan 3V3 (pembacaan lubang kosong salah; rail bawah ≈ 0,04 V di bawah pin) yang baru |

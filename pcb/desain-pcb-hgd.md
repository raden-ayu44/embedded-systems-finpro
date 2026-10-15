---
title: Desain PCB dan Skema - Hand Grip Dynamometer

---

# Desain PCB dan Skema - Hand Grip Dynamometer (v1)
**Proyek akhir - embedded system**

> Skema dan PCB (skema termasuk): komponen, alokasi pin, rantai sinyal, load cell, catu daya, wiring antarmodul, draf PCB v1 (non-formal, dibawa ke kelas Rabu 14 Okt. 2026), netlist ringkas, pemeriksaan load cell, dan sisa pekerjaan. Berkas netlist dan daftar komponen: `pcb_netlist.xlsx`; berkas KiCad: [kicad/](kicad/README.md). Firmware ada di [alur-program-hgd.md](../firmware/alur-program-hgd.md); induk: [catatan-proyek-hgd.md](../catatan-proyek-hgd.md).

---

## 1. Komponen
| Komponen | Tipe | Peran | Status |
|----------|------|-------|--------|
| Mikrokontroler | ESP32 DevKit V1 (chip ESP32-D0WD-V3 rev v3.1, flash 4 MB, tanpa PSRAM) | Firmware, state machine | ✔ ada; loopback pin lolos 8 Okt. 2026 |
| Load cell | CZL 601 80 kg, straight-bar, jembatan penuh, 2,0 ±0,2 mV/V | Sensor gaya | ✔ diterima; 130 x 28 x 21,5 mm; resistansi lolos (10 Okt. 2026) |
| Penguat + ADC | Modul HX711 (papan XFW-HX711), kanal A, gain 128, 10 SPS | Digitalisasi | ▣ dibeli; belum di tangan; header jantan disolder di lab |
| Tampilan | LCD 16x2 HS1602A + backpack I2C (PCF8574) | Teks per state | ✔ ada (dengan backpack, pin jantan) |
| Indikator | LED 5 mm + R1 100 ohm | Sinyal "sedang direkam" | ▣ dari komponen tim |
| Input | Tactile push button, 1 buah | START, NEXT, RESTART | ▣ dari komponen tim |
| PCB | Draf v1, take-home | Dudukan ESP32, tombol, LED, header | ◐ penempatan dan netlist ada |
| Housing grip | 2 pelat Al 170 x 30 x 10 mm + spacer PLA + M6 x 25 | Mekanik | ◐ desain; sisa pengukuran load cell |

---

## 2. Alokasi pin ✔ (loopback 8 Okt. 2026)
| Sinyal | Pin | Antarmuka | Catatan |
|--------|-----|-----------|---------|
| Tombol | GPIO 25 | Input, interrupt FALLING, pull-up internal | Satu tombol untuk semua fungsi |
| LED | GPIO 27 | Output digital | Via R1 100 ohm (≈ 13 mA pada Vf 2,0 V) |
| HX711 DT | GPIO 18 | Input (bit-bang) | Aman di WROOM dan WROVER |
| HX711 SCK | GPIO 19 | Output (bit-bang) | Idem |
| LCD SDA | GPIO 21 | I2C | `Wire.begin(21, 22)` ditulis eksplisit |
| LCD SCL | GPIO 22 | I2C | Alamat 0x27 atau 0x3F; dipindai saat pertama |

> Pin yang **tidak dipakai** dan dihindari: GPIO 16/17 (PSRAM pada WROVER), strapping (0, 2, 5, 12, 15), input-only (34-39) untuk keluaran. `VIN` tidak dipakai.

---

## 3. Rantai sinyal
```
Gaya genggam
   │  pelat A (telapak, tetap) dan pelat B (jari, bergerak) menjepit load cell
   ▼
Load cell CZL 601  ── 4 kabel: eksitasi (hitam, putih), sinyal (merah, hijau)
   │  tegangan diferensial: 0,125 mV/kg pada 5 V; ≈ 0,083 mV/kg pada 3,3 V
   ▼
HX711  ── gain 128, ADC 24 bit, 10 SPS, keluaran serial DT/SCK
   ▼
ESP32  ── (raw − offset) / skala = kg ── state machine
   ├──► LCD (I2C)
   ├──► LED
   ├──◄ Tombol
   └──► USB-Serial ──► logger.py ──► CSV   (ACK kembali)
```

---

## 4. Load cell ✔ (diperiksa 10 Okt. 2026)
| Pemeriksaan | Hasil |
|-------------|-------|
| Dimensi | 130 x 28 x 21,5 mm (lebar 28 mm menyelesaikan beda 28 vs 30 mm) |
| Kabel | 4 kabel: merah, hitam, putih, hijau |
| Resistansi (multimeter, rentang 2k) | hitam-putih 393 ohm; merah-hijau 347 ohm; pasangan silang 260-306 ohm. Dua kali pengukuran, sama |
| Perbandingan datasheet | masukan 401 ±10 ohm (terukur 393 ✔), keluaran 350 ±5 ohm (terukur 347 ✔) |
| **Pemetaan warna** | **Eksitasi = hitam dan putih; sinyal = merah dan hijau**. Berbeda dari datasheet (merah/hitam eksitasi). Penyambungan memakai pemetaan terukur ini, bukan warna datasheet |
| Polaritas E+/E−, S+/S− | Ditentukan di uji bertegangan pertama; tertukar hanya membalik tanda |
| Penyimpanan | Kotak kedap udara, bungkus gelembung (sisi gelembung ke sel), silika gel di luar bungkus, laci bawah lemari; jauh dari kulkas; kabel diberi label (selotip kertas), ujung telanjang ditutup isolasi PVC atau selongsong susut |

---

## 5. Catu daya
| Rail | Tegangan | Sumber | Beban |
|------|----------|--------|-------|
| Logika | 3,3 V (✔ terukur 3,26-3,34 V) | Regulator papan ESP32 dari USB (VIN ✔ 4,98-5,08 V) | ESP32, HX711, LED, tombol, LCD (lihat Bagian 6) |

- Papan dicatu dari USB; tidak ada catu luar yang disambung ke `VIN` bersamaan dengan USB.
- Estimasi arus: tipikal ≈ 117 mA, puncak ≈ 540 mA (didominasi lonjakan WiFi TX, yang tidak aktif pada jalur logging inti); margin terhadap regulator 1 A. Diverifikasi di uji S3.
- Eksitasi load cell diturunkan dari catu HX711 (E+ = AVDD). Karena pengukuran bersifat **rasiometrik**, jumlah counts per kg praktis sama pada 5 V dan 3,3 V; yang berubah adalah noise relatif (sinyal 0,083 mV/kg, bukan 0,125 mV/kg). E+ dan E− diukur saat modul tiba.
- Kapasitor decoupling (100 nF keramik + 10 µF elektrolit, + ke 3V3) dipasang paralel 3V3-GND dekat header HX711.

---

## 6. Keputusan catu LCD
Catu LCD ditetapkan lewat **aturan keputusan** setelah backpack diukur:

| Langkah | Pengukuran | Keputusan |
|---------|-----------|-----------|
| 1 | Dari backpack: alamat I2C (pindai), tegangan SDA dan SCL saat diam, ke mana pull-up terhubung | - |
| 2a | Pull-up ke VCC dan backpack dicatu 5 V | SDA/SCL naik ke 5 V, di atas batas masukan ESP32 (≈ 3,6 V). Pakai **level shifter 2 saluran**; LCD dan backpack dari `VIN` (5 V) |
| 2b | Pull-up ke VCC dan backpack dicatu 3,3 V | Aman untuk ESP32; periksa kontras. Jika terbaca jelas, LCD tetap di 3V3 |
| 2c | LCD tidak terbaca pada 3,3 V | Pindah ke jalur 2a |

> Datasheet modul LCD menyebut catu 5,0 V (4,9-5,1 V), sedangkan keputusan awal tim memakai 3,3 V. Aturan di atas menyelesaikannya dengan pengukuran, bukan dugaan. **Backpack tidak dicatu 5 V sebelum langkah 1 selesai.**

---

## 7. Wiring antarmodul
| Dari | Ke | Sinyal | Kabel |
|------|----|--------|-------|
| PCB J3.1 | HX711 GND | Ground | Jumper F-M |
| PCB J3.2 | HX711 VCC | 3V3 | Jumper F-M |
| PCB J3.3 | HX711 DT | GPIO 18 | Jumper F-M |
| PCB J3.4 | HX711 SCK | GPIO 19 | Jumper F-M |
| PCB J4.1 | Backpack GND | Ground | Jumper F-M |
| PCB J4.2 | Backpack VCC | 3V3 (atau 5 V, Bagian 6) | Jumper F-M |
| PCB J4.3 | Backpack SDA | GPIO 21 | Jumper F-M |
| PCB J4.4 | Backpack SCL | GPIO 22 | Jumper F-M |
| Load cell | Terminal HX711 (E+, E−, A−, A+) | Eksitasi dan sinyal | Kabel 4 inti load cell, dipetakan sesuai Bagian 4; cara sambung ditetapkan setelah modul tiba dan polaritas teruji |

- **Urutan J3** ditetapkan **GND, VCC, DT, SCK** pada revisi PCB, sama dengan J4 (GND, VCC, SDA, SCL), supaya colokan yang tertukar tidak merusak. Netlist v1 saat ini masih GND, DT, SCK, VCC dan diperbarui.
- **Jumper F-M:** ujung betina (F) ke header jantan modul/backpack; ujung jantan (M) disolder ke PCB. Bagian yang aus adalah yang murah (kabel), bukan header modul.
- Header jantan modul HX711 disolder di lab sebelum dipasang.

---

## 8. PCB v1 (draf take-home, non-formal) ◐
Dibawa ke kelas **Rabu 14 Okt. 2026**; belum ada pengumpulan atau penilaian.

| Hal | Rancangan |
|-----|-----------|
| ESP32 | Dua soket betina 1x15 (J1 kiri, J2 kanan), pitch 2,54 mm, jarak baris 25,4 mm (✔ ukur 25,3 mm); papan bisa dicabut |
| Dimensi papan ✔ | 51,5 x 28,2 mm; 15 pin per baris; pin Ø0,5 mm; konektor USB tidak menonjol; modul WROOM 18,00 x 25,50 mm |
| Komponen | SW1 (tombol), D1 + R1 (LED), C1 100 nF, C2 10 µF, TP1 (titik uji 3V3/GND), J3, J4, empat lubang M3 (3,2 mm) |
| Lapisan | Dua lapis; GND pour sisi bawah |
| Jalur (usulan awal) | Sinyal 0,3 mm; 3V3 0,5 mm; jarak minimum 0,25 mm; disesuaikan dengan aturan kelas dan pabrik |
| Antena | Area antena modul (6,19 mm) bebas tembaga; antena menjorok melewati tepi PCB |
| Netlist | `BTN`, `LED_DRV`, `LED_A`, `I2C_SCL`, `I2C_SDA`, `HX_SCK`, `HX_DT`, `+3V3`, `GND` |
| Berkas | `pcb_netlist.xlsx`, [kicad/](kicad/README.md) |

**Langkah PCB berikutnya:** ukur jarak ujung papan ke pin pertama dan terakhir (asumsi simetris 7,97 mm; geser soket bila beda > 0,5 mm) → ukur tombol dan tetapkan footprint → periksa Vf LED dan nilai R1 → pemberian jalur → DRC → gerber → pesan kolektif.

**Aturan desain PCB:**
1. Bagian yang cepat aus berada di bagian murah: header jantan di PCB, soket untuk ESP32.
2. GND pour di sisi bawah, jalur sinyal pendek, kapasitor dekat pin catu HX711.
3. Antena modul bebas tembaga dan jauh dari kabel sinyal HX711.
4. Elektrolit berpolaritas (+ ke 3V3); keramik tidak.
5. Draf ini tanpa proteksi polaritas, sekering, atau ESD; cukup untuk perangkat 3,3 V berdaya USB.

> **Apakah antena WiFi mengganggu kabel HX711?** Jalur logging inti memakai USB-Serial dan WiFi tidak diaktifkan. Kabel HX711 tetap dijauhkan dari area antena sebagai kebiasaan baik.

---

## 9. PCB V1 DAN PEMERIKSAAN KOMPONEN (10 OKT. 2026)

*Ringkasan pekerjaan 10 Okt. 2026. Berkas: `pcb_netlist.xlsx` (komponen dengan nama umum, nama part resmi, simbol dan footprint KiCad; netlist; asumsi dan sisa). Gambar dan angka detail ada di berkas itu; bagian ini hanya keputusan dan statusnya.*

### 9.1 Ruang lingkup

- Draf PCB v1 dibawa ke kelas Rabu 14 Okt. 2026. Sifatnya take-home dan non-formal: belum ada pengumpulan atau penilaian.
- Persyaratan tugas PCB (alat desain, ukuran maksimum, jumlah lapisan, aturan jalur) belum diketahui karena materi kelas belum tersedia di sistem kelas. Semua angka jalur di bawah adalah usulan.
- Tidak termasuk: regulator tegangan (DS-REG1 ditunda sampai materi kelas), proteksi polaritas, sekering, ESD.

### 9.2 Keputusan desain

| Hal | Keputusan | Alasan |
|---|---|---|
| ESP32 | Dua soket betina 1x15 pitch 2,54 mm (J1 kiri, J2 kanan), jarak antar baris 25,4 mm (ukur 25,3 mm). Papan tidak disolder langsung | Papan bisa dicabut; sesuai persetujuan lab. Selisih 0,1 mm masih di dalam kelonggaran pin 0,5 mm di lubang 1,0 mm |
| LCD dan HX711 | Tidak di PCB. Tersambung lewat kabel jumper betina-jantan (F-M): ujung F ke header jantan backpack LCD dan modul HX711, ujung M disolder ke PCB pada J3 (HX711) dan J4 (LCD) | Bagian yang aus adalah yang murah (kabel), bukan header modul atau PCB |
| Urutan pin | J3: GND, DT, SCK, VCC. J4: GND, VCC, SDA, SCL (urutan umum backpack, cek saat tiba). Usulan (belum diputuskan): ubah J3 menjadi GND, VCC, DT, SCK supaya urutannya sama dengan J4 dan colokan yang tertukar tidak merusak | Urutan pin modul XFW-HX711 belum dicek pada barang yang tiba |
| Catu | Papan dicatu dari USB. HX711 dari 3V3; LCD direncanakan dari VIN (5 V) per DS-LCD1 (diperbarui v39) | **LCD 5 V atau 3,3 V belum pasti** (Bagian 7 di [catatan-proyek-hgd.md](../catatan-proyek-hgd.md)). Backpack tidak boleh dicatu 5 V sebelum SDA dan SCL diukur |
| Tombol | SW1 tactile, GPIO25, pull-up internal, interrupt. Footprint 6 x 6 mm sementara; hanya dua kaki diagonal dipakai | Ganti sesuai DS-BTN1 setelah diukur |
| LED | D1 5 mm di GPIO27, R1 100 ohm | Sekitar 13 mA pada Vf 2,0 V ((3,3 - 2,0) / 100); anggaran daya memakai 15 mA. Cek Vf LED yang dipakai |
| Kapasitor (opsional) | C1 100 nF keramik dan C2 10 uF elektrolit (6,3 V atau lebih), paralel antara 3V3 dan GND dekat J3 | Menahan riak catu HX711. Keramik tidak berpolaritas; elektrolit berpolaritas (+ ke 3V3). Ada di komponen lab |
| Titik uji | TP1 header jantan 1x2 (3V3 dan GND) | Untuk multimeter |
| Lainnya | Empat lubang M3 (3,2 mm); GND pour sisi bawah; area antena modul (6,19 mm) bebas tembaga | Pemasangan ke housing; antena tidak boleh tertutup tembaga |

Penempatan memakai pinout DevKit V1 30 pin (kiri, antena di atas: EN, 36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13, GND, VIN; kanan: 23, 22, TX0, RX0, 21, 19, 18, 5, 17, 16, 4, 2, 15, GND, 3V3).

### 9.3 Netlist (ringkas)

| Net | Fungsi | Pin |
|---|---|---|
| BTN | Tombol, GPIO25 | J1.8, SW1.4 |
| LED_DRV | GPIO27 ke resistor | J1.10, R1.1 |
| LED_A | Resistor ke anoda LED | R1.2, D1.A |
| I2C_SCL | LCD SCL, GPIO22 | J2.2, J4.4 |
| I2C_SDA | LCD SDA, GPIO21 | J2.5, J4.3 |
| HX_SCK | HX711 SCK, GPIO19 | J2.6, J3.3 |
| HX_DT | HX711 DT, GPIO18 | J2.7, J3.2 |
| +3V3 | Rail logika dari regulator papan | J2.15, J3.4, J4.2, C1.1, C2.+, TP1.1 |
| GND | Ground bersama | J1.14, J2.14, SW1.1, D1.K, J3.1, J4.1, C1.2, C2.-, TP1.2 |
| (tidak dipakai) | VIN | J1.15 |

### 9.4 Pemeriksaan load cell (CZL 601 80 kg)

| Pemeriksaan | Hasil | Arti |
|---|---|---|
| Dimensi | 130 x 28 x 21,5 mm | Cocok dengan Eagle Weigh (130 x 28 x 22); lebar 28 mm, bukan 30 mm; tinggi 0,5 mm kurang dari nilai gambar |
| Kabel | 4 kabel: merah, hitam, putih, hijau | Sama jumlahnya dengan datasheet; warnanya dipetakan ulang |
| Resistansi (multimeter DT9205A, rentang 2k, hasil tampil dalam kohm: 0,306 = 306 ohm) | Hitam-putih 393 ohm; merah-hijau 347 ohm; empat pasangan silang 260-306 ohm. Diulang dua kali, nilai sama | Pasangan masukan (eksitasi): datasheet 401 +-10 ohm, terukur 393, di dalam rentang. Pasangan keluaran (sinyal): datasheet 350 +-5 ohm, terukur 347, di dalam rentang. Pasangan silang memberi angka antara keduanya, sesuai jembatan penuh |
| Pemetaan warna | **Eksitasi = hitam dan putih; sinyal = merah dan hijau** | Bertentangan dengan datasheet (merah/hitam eksitasi, hijau/putih sinyal) dan listing penjual. Jangan menyambung berdasarkan warna datasheet |
| Polaritas (E+/E-, S+/S-) | Belum diketahui | Penukaran hanya membalik tanda pembacaan; tentukan di uji bertegangan pertama |
| Uji bertegangan | Sebagian (15 Okt. 2026, HX711 pada 3V3, breadboard) | Sel merespons tekanan; noise diam ≈ 0,9 g (kasar). Tanda belum pasti karena sel belum dijepit ([hasil](../firmware/sketches/hgd_bench_test/hasil-uji-2026-10-15.md)) |

Penyimpanan: kotak kedap udara dengan gelembung plastik (sisi gelembung menghadap sel), paket silika gel di luar bungkus gelembung, di laci bawah lemari; jauhkan dari kulkas (kondensasi dan getaran); jangan ada beban di atasnya dan jangan dibawa lewat kabel. Ujung kabel telanjang ditutup (isolasi PVC atau selongsong susut; selotip kertas hanya untuk label), dan tiap kabel diberi label peran dan hasil ukur. Biarkan kotak mencapai suhu ruang 10-15 menit sebelum dibuka.

### 9.5 Kode uji

- [esp32_uji_awal.ino](../firmware/sketches/esp32_uji_awal/esp32_uji_awal.ino): uji loopback pin proyek (DS-ESP1).
- [hgd_bench_test.ino](../firmware/sketches/hgd_bench_test/hgd_bench_test.ino): uji bench seluruh rangkaian di breadboard (I2C/LCD, HX711 dan load cell, tombol, LED, uji noise E1). Dijalankan 15 Okt. 2026: U4, U7, U8 lolos; hasil di [hasil-uji-2026-10-15.md](../firmware/sketches/hgd_bench_test/hasil-uji-2026-10-15.md).

### 9.6 Sisa pekerjaan

1. Putuskan catu LCD (3,3 V atau 5 V) setelah backpack diukur; ubah J4 bila perlu.
2. Putuskan urutan pin J3 (GND, VCC, DT, SCK atau tetap).
3. Ukur jarak ujung papan ke pin pertama dan terakhir DevKit; geser soket bila beda lebih dari 0,5 mm.
4. Ukur tombol (DS-BTN1) dan pastikan footprint serta nilai R1 (Vf LED).
5. Pemberian jalur dan DRC setelah persyaratan tugas diketahui (usulan: dua lapis, sinyal 0,3 mm, 3V3 0,5 mm, jarak minimum 0,25 mm).
6. Solder header jantan modul HX711 di lab; isi daftar cek kedatangan DS-HX1, DS-LCD1, DS-I2C1.
7. Uji bertegangan load cell dan tentukan polaritas; hitung ulang tebal total housing dengan tinggi 21,5 mm.
8. Skema KiCad `hgd-v1` perlu diperbaiki sebelum dipakai: simbol modul diganti dua header betina 1x15, hapus USB yang tidak tersambung, periksa IO25/IO27, tambah kapasitor, hubungkan 3V3 ke VCC modul, beri anotasi, tambah PWR_FLAG.

---

*Catatan kerja proyek akhir mata kuliah embedded system. Bukan dokumen yang dinilai.*

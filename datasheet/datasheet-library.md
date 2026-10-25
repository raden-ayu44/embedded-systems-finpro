# DS Library - rak "Komponen & Datasheet"

Rak ini menampung sumber tingkat komponen. Rak teori dan literatur (buku dan paper teoretis) ada di Bagian 11 `catatan-proyek-hgd.md`. Satu komponen = satu entri `DS-xxx`.

**Konvensi sumber untuk setiap entri**

| Kode | Sumber | Peran | Bisa disitasi sebagai sumber akademik? |
|---|---|---|---|
| S1 | Spesifikasi vendor (halaman produk) | Nilai spesifikasi barang yang kita beli. **Kolom spesifikasi selalu diisi dari S1.** | Tidak (dokumen penjual) |
| S2 | Datasheet pabrikan chip/komponen | Nilai yang diharapkan; dipakai untuk **memeriksa silang** S1 | Ya |
| S3 | Manual atau dokumen pihak ketiga | Pengamatan: keunggulan dan kelemahan yang ditemukan pada penerapan nyata | Dengan catatan (bukan pabrikan) |

**Label hasil pemeriksaan silang (S1 terhadap S2):** Konsisten / Lebih konservatif (S1 lebih ketat dari S2) / Tidak dinyatakan (S1 diam) / Konflik (S1 berbeda dengan S2 atau dokumen komponen lain).

## Indeks entri (daftar datasheet yang harus diambil, urut prioritas)

Status: **Terisi** = entri sudah ada di file ini; **Punya** = dokumennya sudah ada, entri belum ditulis; **Ambil** = dokumen belum ada; **Opsional** = hanya bila disitasi atau dipakai.

### Prioritas 1: membawa keputusan desain atau noise

| No | Kode | Dokumen | Yang diambil / catatan | Status |
|---|---|---|---|---|
| 1 | DS-HX1 | Datasheet chip HX711 (Avia) | Sudah ada. Ambil juga **skema modul HX711 yang benar-benar dibeli**: papan klon yang menentukan catu, filter, dan pengaturan rate yang sebenarnya didapat. | Terisi; skema papan: Ambil |
| 2 | DS-LC1 | Datasheet Eagle Weigh CZL 601 | Sudah ada halaman Eagle Weigh dan PDF GJ Impex (lihat entri). Ambil gambar resmi pabrikan dengan **kedalaman lubang dan posisi keluar kabel** bila diterbitkan. | Terisi; gambar resmi: Ambil |
| 3 | DS-ESP1 | Datasheet modul ESP32-WROOM-32D (Espressif) | Modul yang dipilih tim: ESP32-WROOM-32D. | Terisi |
| 4 | DS-ESP2 | Papan ESP32 DevKit V1 | Papan yang dipilih tim. Posisi pin, fungsi pin, regulator, dan chip USB sudah ada (dari S3); skema V1 yang dirujuk halaman S3 bisa diambil bila perlu. | Terisi; papan sudah ada. Terverifikasi sebagian (enam pin proyek, 3V3 dan VIN); sisa: cetakan modul, penanda regulator, chip USB |
| 5 | DS-REG1 | Datasheet regulator tegangan pada papan itu | Sering AMS1117-3.3 atau setara. Penting untuk noise catu dan batas arus. | Ambil |

### Prioritas 2: elektronik lain di BOM

| No | Kode | Dokumen | Yang diambil / catatan | Status |
|---|---|---|---|---|
| 6 | DS-LCD1 | Datasheet modul LCD HS1602A (pengontrol SPLC780, kompatibel HD44780) | Modul pada foto vendor. | Terisi, menunggu verifikasi saat barang tiba |
| 7 | DS-I2C1 | Datasheet chip backpack I2C (PCF8574/PCF8574A) | Untuk alamat dan catu. Panduan Adafruit yang tersedia adalah backpack **MCP23008**, bukan PCF8574 (lihat entri). | Terisi, menunggu verifikasi chip pada backpack |
| 8 | DS-LED1 | Datasheet LED 5 mm | Tegangan dan arus maju LED yang benar-benar dibeli. | Ambil |
| 9 | DS-BTN1 | Datasheet tombol tactile | Arus kontak, waktu bounce, dimensi. | Ambil |
| 10 | DS-RES1 | Datasheet resistor pembatas LED | Toleransi dan daya; hanya jika ingin disitasi. | Opsional |
| 11 | DS-CAP1 | Kapasitor decoupling untuk catu HX711 | Tipe dan tegangan kerja; hanya jika ditambahkan. | Opsional |
| 12 | DS-CON1 | Pin header/konektor untuk PCB | Hanya jika dimensi housing bergantung padanya. | Opsional |
| 12a | DS-LLS1 | Modul level shifter dua arah 4 kanal (BSS138) | Ditambahkan 15 Okt. 2026 untuk SDA/SCL LCD 5 V. Datasheet MOSFET BSS138 dan gambar dimensi modul yang dibeli. | Terisi sebagian; datasheet BSS138: Ambil |

### Prioritas 3: bahan dan mekanik

| No | Kode | Dokumen | Yang diambil / catatan | Status |
|---|---|---|---|---|
| 13 | DS-MAT1 | Datasheet pelat aluminium dari pemasok | Paduan dan temper, modulus E, kuat luluh. Perhitungan defleksi dan tegangan kita memakai E = 70 GPa. | Ambil |
| 14 | DS-MAT2 | Technical data sheet filamen PLA | Modulus, suhu transisi gelas, catatan creep. | Ambil |
| 15 | DS-MAT3 | Standar baut M6 | ISO 898-1 (kelas kekuatan) + standar dimensi ISO/DIN untuk tipe baut yang dipakai. | Ambil |
| 16 | DS-MAT4 | Bahan alas karet/spacer | Hanya jika rig kalibrasi menyitasinya. | Opsional |

### Prioritas 4: instrumen dan standar

| No | Kode | Dokumen | Yang diambil / catatan | Status |
|---|---|---|---|---|
| 17 | DS-STD1 | OIML R60 | - | Punya, entri belum ditulis |
| 18 | DS-STD2 | VPG 11864 (dokumen teknis) | - | Punya, entri belum ditulis |
| 19 | DS-INS1 | Spesifikasi multimeter DT9205A | Akurasi dan resolusi; dipakai saat memeriksa resistansi jembatan 350/400 ohm, tegangan catu, dan arus total. | Terisi |
| 20 | DS-INS2 | Spesifikasi jangka sorong | Resolusi dan akurasi; dipakai untuk dimensi load cell. | Ambil |
| 21 | DS-INS3 | Spesifikasi timbangan badan | Resolusi dan akurasi; instrumen massa acuan. **Paling berpengaruh pada ketidakpastian kalibrasi.** | Ambil |
| 22 | DS-INS4 | Toleransi label dumbbell | Bila pabrik menyebutnya. Opsional karena kita menimbangnya sendiri. | Opsional |

### Prioritas 5: manufaktur

| No | Kode | Dokumen | Yang diambil / catatan | Status |
|---|---|---|---|---|
| 23 | DS-MFG1 | Kemampuan pabrik PCB dan persyaratan Gerber | Lebar jalur minimum, ukuran lubang, format berkas. Di sinilah konflik tenggat di catatan proyek muncul. | Ambil |
| 24 | DS-MFG2 | Pengaturan printer 3D dan slicer di lab | Hanya jika parameter cetak disitasi. | Opsional |

**Tidak diperlukan:** datasheet CZL 882 dan sel generik 180 kg, karena keduanya sudah diganti. Simpan CZL 882 hanya jika laporan menjelaskan alasan penolakannya.

Perubahan kode dari indeks lama: DS-LED1/DS-RES1/DS-BTN1 dipisah (DS-LED1, DS-BTN1, DS-RES1), DS-PWR1 diganti DS-REG1 dan DS-CAP1, DS-MAT1 dipecah menjadi DS-MAT1 sampai DS-MAT4, DS-STD1 kini hanya OIML R60 (VPG 11864 menjadi DS-STD2).

---

## DS-HX1: Modul HX711

### Identitas dan bukti

| Item | Isi |
|---|---|
| Modul | Papan hijau bertanda **XFW-HX711**, 24 x 16 mm (dari foto listing; ukur ulang dengan jangka sorong) |
| Chip | HX711 buatan **Avia Semiconductor** |
| S1 | Listing vendor CNC Store (Bandung) [1]: daftar spesifikasi di bawah dan foto papan. Tautan listing: https://shopee.co.id/MODULE-HX711-HX-711-AMPLIFIER-LOAD-CELL-ADC-CONVERTER-SENSOR-BERAT-i.62956347.3613589309 (tanggal akses: **ISI**) |
| S2 | Avia Semiconductor, *HX711 24-Bit Analog-to-Digital Converter (ADC) for Weigh Scales* (datasheet resmi, chip) [2]. Tautan: https://cdn.sparkfun.com/datasheets/Sensors/ForceFlex/hx711_english.pdf (salinan di SparkFun; pabrikan Avia) |
| S3 | *HX711: 24-bit Delta Sigma ADC interface for weight scale*, PSoC Creator Component datasheet, v0.0.b, Rev. *B, 30 Maret 2020 (pihak ketiga, bukan pabrikan; berisi manual pemakaian dan catatan pengamatan) [3]. Tautan: https://community.infineon.com/gfawx74859/attachments/gfawx74859/CodeExamples/546/7/HX711_v0_0_B.pdf (tanggal akses: **ISI**). Dokumen ini sendiri merujuk S2 pada alamat SparkFun yang sama. |
| Bukti bahwa S3 membahas papan yang sama | Gambar 1 di S3 menunjukkan papan hijau bertanda XFW-HX711 dengan label pin yang sama dengan foto listing S1 (E+, E-, A-, A+, B-, B+ dan GND, DT, SCK, VCC) dan penanda 80Hz/10Hz di sisi atas. Foto S1 menunjukkan tanda "10Hz" di posisi yang sama. |
| Catatan | Avia adalah pabrikan **chip**, bukan papan modul. Papan XFW-HX711 adalah desain pihak ketiga. Bagian papan (transistor regulator, filter RC input, resistor pemilih rate) tidak tercakup dalam S2. |

### Tabel A: spesifikasi vendor (S1), pemeriksaan silang ke S2, dan catatan S3

| No | Parameter | S1: vendor | S2: pemeriksaan silang ke Avia | Hasil | S3: catatan manual (pengamatan) |
|---|---|---|---|---|---|
| 1 | Kabel merah | Excitation+ atau VCC | Datasheet chip tidak mengatur warna kabel. Terminal papan bernama E+ (excitation positif jembatan). | Tidak dinyatakan (S2) | Banyak papan murah memiliki cacat PCB: terminal E- tidak tersambung ke ground dan mengambang di sekitar 0,6 V, sehingga E+ tidak teregulasi (sekitar 4,9-5 V). Kalau E- disambung ke GND, E+ stabil di sekitar 4,3 V (Lampiran 3). **Kelemahan.** |
| 2 | Kabel hitam | Excitation- atau GND | Idem; terminal E-. | Tidak dinyatakan (S2) | Idem (cacat E- mengambang). Perbaikan: kabel pendek dari E- ke pin GND (Gambar 14). |
| 3 | Kabel putih | Amplifier+, Signal+ atau Output+ | Chip memiliki masukan INA+ untuk kanal A. | Tidak dinyatakan (S2) | - |
| 4 | Kabel hijau | A-, S- atau O- | Chip memiliki masukan INA-. | Tidak dinyatakan (S2) | - |
| 5 | Tegangan operasi | 2,7 V - 5 V | AVDD dan DVDD 2,6 - 5,5 V; VSUP (regulator) 2,7 - 5,5 V. | **Lebih konservatif** | Skala penuh kanal A adalah +-20 mV (gain 128) pada AVDD 5 V. Pada papan tanpa cacat, E+ sekitar 4,3 V. |
| 6 | Arus operasi | < 1,5 mA | Operasi normal < 1,5 mA (analog 1400 uA, digital 100 uA tipikal); power down < 1 uA. | **Konsisten** | Kode komponen S3 dapat menghentikan ADC dan menaruhnya pada mode arus rendah. |
| 7 | Laju data | 10 SPS atau 80 SPS (dapat dipilih) | 10 SPS (RATE = 0) atau 80 SPS (RATE = DVDD) dengan osilator internal. | **Konsisten** | Pada papan, pemilihan dilakukan dengan memindahkan resistor 0 ohm (Gambar 4), **bukan lewat firmware**. Bawaan 10 Hz. **Kelemahan:** 80 SPS butuh modifikasi perangkat keras. |
| 8 | Penolakan 50 dan 60 Hz | Serentak (simultaneous) | Tercantum pada daftar fitur: "Simultaneous 50 and 60Hz supply rejection". Tanpa angka dB khusus untuk 50/60 Hz (yang tercantum: PSRR 100 dB pada gain 128, RATE = 0). | **Konsisten** (hanya sebagai klaim fitur) | Tidak dibahas. |
| 9 | Dual-channel 24 bit | Dual-channel, 24 bit | Dua masukan diferensial yang dapat dipilih; ADC 24 bit. Kanal A gain 128 atau 64; kanal B gain tetap 32 (+-80 mV pada 5 V). | **Konsisten** | Kode 24 bit adalah lebar kode, bukan resolusi efektif. Dengan noise 50 nV rms dan LSB sekitar 2,3 nV (hitungan kita, gain 128, AVDD 5 V), noise sekitar 21 LSB rms. Gain tidak dapat diubah saat berjalan pada komponen S3. |

### Tabel B: parameter yang kita pakai tetapi tidak disebut vendor (sumber: S2)

| Parameter | S1: vendor | S2: Avia | S3: catatan manual | Dipakai di |
|---|---|---|---|---|
| Skala penuh diferensial | Tidak dinyatakan | +-0,5 x (AVDD / gain): +-19,5 mV (5 V), +-12,9 mV (3,3 V), gain 128 (hitungan kita) | S3 membulatkan ke +-20 mV pada 5 V | Skala penuh dan resolusi |
| Rentang tegangan common-mode | Tidak dinyatakan | AGND + 1,2 V sampai AVDD - 1,3 V | - | Mengapa jembatan tidak boleh dieksitasi langsung 10 V |
| Noise masukan (gain 128) | Tidak dinyatakan | 50 nV rms (10 SPS), 90 nV rms (80 SPS) | Contoh S3: sel 1 kg, filter median 19 titik, akurasi jangka pendek sekitar 27 mg (sel dan papan mereka, bukan sel kita) | Jawaban noise ke asisten |
| Waktu settling | Tidak dinyatakan | 400 ms (10 SPS), 50 ms (80 SPS) | Empat pembacaan pertama setelah mulai (400 ms pada 10 Hz) berisi galat dan harus dibuang | Firmware (SIAP) |
| Drift suhu | Tidak dinyatakan | Offset +-6 nV/derajat C, gain +-5 ppm/derajat C | - | Pemanasan, tare |
| PSRR dan CMRR | Tidak dinyatakan | 100 dB (gain 128, RATE = 0) | - | Catu daya |
| Rentang suhu | Tidak dinyatakan | -40 sampai +85 derajat C | - | Batas pemakaian |
| Level logika DT dan SCK | Tidak dinyatakan | DVDD sebaiknya sama dengan catu MCU | - | Lihat Peringatan 2 |

### Catatan merit dan demerit dari S3

| Jenis | Isi |
|---|---|
| Merit | Menjelaskan komposisi papan tipikal: transistor regulasi dan jaringan RC sebagai low-pass di masukan. |
| Merit | Mencantumkan alur baca 2 kawat (24 clock data, 1-3 clock gain), tare dengan mengurangi offset, dan contoh filter median 19 titik (penundaan sekitar 1 s). |
| Merit | Contoh nyata: sensitivitas sekitar 920 counts/g pada sel 1 kg, akurasi jangka pendek sekitar 27 mg setelah filter. |
| Demerit | Cacat PCB E- mengambang pada banyak papan murah; dokumen tidak menyebut papan mana yang terkena. |
| Demerit | Kode dan API ditulis untuk PSoC, bukan ESP32; hanya konsepnya yang bisa dipakai. |
| Demerit | Dokumen pihak ketiga (2020), bukan peer-review, bukan dari pabrikan. |

### Skema referensi Avia (S2, Fig. 4 dan Fig. 5): pembanding untuk papan kita

S2 memuat "Reference PCB Board (Single Layer)": skema (Fig. 4) dan tata letak (Fig. 5). Ini desain referensi **pabrikan chip**, bukan skema papan XFW-HX711 kita. Papan klon kemungkinan mengikutinya (S3 menyebut transistor regulator dan jaringan RC di masukan), tetapi itu belum terbukti. Dipakai sebagai nilai "seharusnya" saat papan tiba.

| Elemen di Fig. 4 | Isi | Yang dibandingkan pada papan kita |
|---|---|---|
| Catu | VSUP dan DVDD sama-sama ke "MCU VDD (2,7-5,5 V)", dengan C1 10 uF | Pada klon: apakah VCC memberi DVDD? (mengonfirmasi Peringatan 2) |
| Regulator analog | Q1 (PNP; S8550 pada Fig. 1), R1/R2 pada VFB, AVDD diberi label "2,6-5,2 V", C2 10 uF di AVDD | Ukur AVDD dan E+. Catatan: tabel spesifikasi S2 menyebut AVDD 2,6-5,5 V, label Fig. 4 menyebut 5,2 V. Sumber sama, angka berbeda |
| Referensi VBG | C3 0,1 uF | - |
| Masukan A | R3 dan R4 100 ohm seri, C4 0,1 uF di antara INNA dan INPA | Papan klon biasanya punya filter serupa (S3); jangan tambah kapasitor |
| E- | Terminal E- tersambung ke **GND** | **Pembanding cacat E-:** papan sehat membaca E- sekitar 0 V |
| RATE, XI, INB+, INB- | Ke GND: 10 SPS, osilator internal, kanal B tidak dipakai | Cek pad 10Hz/80Hz |
| Header | E+, E-, S-, S+ (4 pin) dan catu (VCH, GND) | - |
| Nilai R1 dan R2 | Tidak tertulis di Fig. 4; rumus S2: AVDD = VBG x (R1+R2) / R1 (Fig. 1) | Hitung dari nilai terukur bila perlu |

### Peringatan untuk desain kita

1. **Warna kabel signal bertentangan dengan datasheet load cell.** S1 menyebut putih = Signal+ dan hijau = Signal-. Datasheet Eagle Weigh CZL 601 menyebut hijau = S+ dan putih = S-. Merah (E+) dan hitam (E-) sama. Kalau tertukar, pembacaan hanya berbalik tanda (tanpa kerusakan); sambungkan sesuai datasheet CZL 601 dan periksa tanda pembacaan saat uji pertama. Kalau tanda negatif, tukar kabel sinyal atau pakai kemiringan negatif saat kalibrasi.
2. **Level logika.** Avia menyarankan DVDD memakai catu yang sama dengan MCU. Skema referensi S2 (Fig. 4) menyambungkan VSUP dan DVDD ke MCU VDD yang sama. Pada papan tipikal, VCC memberi DVDD, sehingga jika modul dicatu 5 V, pin DT mengeluarkan logika 5 V ke GPIO ESP32 (3,3 V). Pilihan: (a) catu modul 3,3 V (perkiraan noise dan skala penuh di bawah), atau (b) catu 5 V dengan pembagi tegangan atau level shifter pada DT. **Periksa pada papan Anda** apakah VCC memberi DVDD.
3. **Cacat E-.** Skema referensi S2 (Fig. 4) menyambungkan E- ke GND; papan yang E--nya mengambang menyimpang dari referensi. Ukur E- terhadap GND dan E+ terhadap GND saat papan tiba (lihat daftar cek).

**Perkiraan noise load cell 80 kg (eksitasi 2 mV/V, gain 128):**

| Eksitasi E+ | Skala penuh jembatan | Noise 10 SPS | Noise 80 SPS |
|---|---|---|---|
| 5,0 V | 10,0 mV | sekitar 0,40 g | sekitar 0,72 g |
| 4,3 V (papan tanpa cacat pada 5 V) | 8,6 mV | sekitar 0,47 g | sekitar 0,84 g |
| 3,3 V (catu 3,3 V) | 6,6 mV | sekitar 0,61 g | sekitar 1,09 g |

Ini hitungan dari noise Avia, **bukan** pengukuran; E+ yang sebenarnya pada catu 3,3 V bisa lebih rendah akibat drop regulator. Ukur E+ dan ganti angka ini dengan hasil uji noise 1000 sampel.

### Daftar cek saat modul tiba

| No | Pemeriksaan | Hasil |
|---|---|---|
| 1 | Foto papan: penanda XFW-HX711 dan pad 10Hz/80Hz | |
| 2 | Ukur PCB dengan jangka sorong (24 x 16 mm?) | 24 x 15,6 mm; header 4 pin di tengah sisi pendek (15 Okt. 2026) |
| 3 | Catu modul pada tegangan pilihan; ukur E- terhadap GND (seharusnya sekitar 0 V; cacat bila sekitar 0,6 V) | **0,02 V** pada catu 3,3 V: tidak cacat (24 Okt. 2026) |
| 4 | Ukur E+ terhadap GND dengan load cell tersambung (sekitar 4,3 V pada catu 5 V bila baik) | **2,98 V** pada VCC modul 3,17 V (24 Okt. 2026) |
| 5 | Ukur AVDD dan tegangan pada pin VCC | VCC **3,17 V**; AVDD = E+ = 2,98 V (24 Okt. 2026; semua pembacaan di [hasil-multimeter-hgd.md](../pengukuran/hasil-multimeter-hgd.md)) |
| 6 | Status pad rate (10 SPS bawaan?) | Sekitar 11,8 sampel/s terukur (706 sampel dalam 60 s): mode 10 SPS (15 Okt. 2026) |
| 7 | Tegangan logika pada DT saat bekerja | |
| 8 | Polaritas pembacaan dengan kabel sesuai datasheet CZL 601 | |
| 9 | Uji noise 1000 sampel, 10 dan 80 SPS | 10 SPS saja, 706 sampel (E1, 15 Okt. 2026): σ 38,2 counts = 0,0007 kg, peak-to-peak 206 counts. 80 SPS belum |

### Daftar pustaka DS-HX1 (IEEE)

Nomor [1]-[3] mengikuti urutan kemunculan di tabel Identitas. Seperti kode lokal lain di catatan ini, nomor ini baru berlaku di dokumen ini; saat disalin ke laporan, nomornya harus disesuaikan dengan daftar pustaka laporan.

[1] CNC Store Bandung, "Module HX711 HX-711 amplifier load cell ADC converter sensor berat," Shopee Indonesia, product listing. [Online]. Available: https://shopee.co.id/MODULE-HX711-HX-711-AMPLIFIER-LOAD-CELL-ADC-CONVERTER-SENSOR-BERAT-i.62956347.3613589309 (diakses 8 Okt. 2026).

[2] Avia Semiconductor, "HX711: 24-bit analog-to-digital converter (ADC) for weigh scales," datasheet, n.d. [Online]. Available: https://cdn.sparkfun.com/datasheets/Sensors/ForceFlex/hx711_english.pdf (diakses 8 Okt. 2026).

[3] "HX711: 24-bit delta sigma ADC interface for weight scale," PSoC Creator Component Datasheet, v0.0.b, Rev. *B, Infineon Developer Community, Code Examples, Mar. 30, 2020. [Online]. Available: https://community.infineon.com/gfawx74859/attachments/gfawx74859/CodeExamples/546/7/HX711_v0_0_B.pdf (diakses 8 Okt. 2026).

**Catatan sitasi (hapus setelah dicek):**
- [1]: nama toko "CNC STORE BANDUNG" sudah dicocokkan dengan tangkapan layar halaman toko pada tautan yang sama (8 Okt. 2026). Judul diambil dari bagian URL; samakan dengan judul di halaman produk. Ganti tanggal akses bila dibuka di tanggal lain. Parameter pelacak (`?xptdk=...`) pada tautan asli sengaja dibuang.
- [2]: dokumen tidak mencantumkan tanggal atau revisi pada halamannya ("n.d."); tautan adalah salinan yang di-hosting SparkFun, bukan situs Avia.
- [3]: penulis tidak tertulis di halaman dokumen (metadata PDF menyebut nama "Irina", tetapi itu bukan data yang dicetak, jadi tidak dipakai sebagai penulis). Tautan adalah lampiran komunitas, bukan dokumen resmi pabrikan.

---

## DS-LC1: Load cell Eagle Weigh CZL 601, 80 kg

### Identitas dan bukti

| Item | Isi |
|---|---|
| Komponen | Load cell single-point aluminium CZL 601, kapasitas 80 kg, jembatan 4 kawat (kawat kelima belum jelas). |
| S1 | Listing vendor Automa-88 (Kab. Bekasi), Shopee [4]. Tautan: https://shopee.co.id/Loadcell-CZL-601-3-80KG-Digital-Scale-Sensor-i.43040199.24877149110 (tanggal akses: 8 Okt. 2026). Merek di listing "-", tanpa garansi. |
| S2 | Halaman produk resmi Eagle Weigh, seri CZL 601 [5]. Halaman web, bukan PDF, dan tipis: hanya sensitivitas, error gabungan, rentang suhu, IP, bahan, dan daftar kapasitas. Tidak memuat dimensi, kabel, resistansi, eksitasi, creep, atau overload. |
| S3 | **Satu PDF pendamping: GJ Impex, "CZL-601 Load Cell" (2023)** [6]. Dipilih karena paling dekat dengan S2: daftar kapasitas sama persis (3, 6, 10, 20, 40, 80 kg, dokumen lain berhenti di 120 kg atau 60 kg tanpa 80), kompensasi suhu sama (-10 sampai +40 derajat C), dan menyebut model CZL-601 dengan gambar ukuran, wiring, serta parameter listrik lengkap. Catatan: tabelnya ditulis untuk model 40 kg dan GJ Impex adalah penyalur, bukan pabrikan; nilainya dipakai sebagai pelengkap, bukan sebagai jaminan untuk barang kita. |

### Tabel A: spesifikasi vendor (S1), pemeriksaan silang ke Eagle Weigh (S2), dan GJ Impex (S3)

| No | Parameter | S1: vendor | S2: Eagle Weigh | Hasil | S3: GJ Impex |
|---|---|---|---|---|---|
| 1 | Kapasitas | 3/6/10/20/40/80 kg (kita ambil 80) | 3/6/10/20/40/80 kg | **Konsisten** | Daftar sama; tabel untuk 40 kg |
| 2 | Sensitivitas | 2,0 +-0,2 mV/V | 2,0 +-0,15 mV/V | **Konflik** (vendor lebih longgar) | 2,0 +-0,1 mV/V |
| 3 | Non-linearitas / error | 0,02% FS | Error gabungan <= +-0,03 %RO | **Tidak dapat dibandingkan** (besaran berbeda) | NL <0,03% FSO; C2/C3 = 0,017%/0,02% FSO. Error gabungan +-0,03 %RO pada 80 kg = sekitar +-24 g (hitungan kita) |
| 4 | Creep | 0,0016% FS | Tidak dinyatakan | **Konflik** dengan S3 | <0,025% FSO per 30 menit. Dugaan: angka vendor salah label. Pakai 0,025%: sekitar 20 g per 30 menit pada 80 kg (hitungan kita) |
| 5 | Kelas | C3 | Tidak dinyatakan | **Tidak dinyatakan** (S2) | C2/C3; konsisten |
| 6 | Proteksi | IP67 | IP67 | **Konsisten** | IP65 (lebih rendah) |
| 7 | Overload aman / destruktif | 150% / 200% | Tidak dinyatakan | **Konflik** dengan S3 | 120% / 150%. **Desain dengan 120%/150% (96/120 kgf).** |
| 8 | Eksitasi | 10-15 V | Tidak dinyatakan | **Konflik** dengan S3 | 9-12 V. Eksitasi kita (3,3-5 V) di bawah kedua rentang |
| 9 | Resistansi masukan | 401 +-10 ohm | Tidak dinyatakan | **Tidak dinyatakan** (S2) | 400 +-10 ohm; konsisten |
| 10 | Resistansi keluaran | 350 +-5 ohm | Tidak dinyatakan | **Tidak dinyatakan** (S2) | 350 +-5 ohm; konsisten |
| 11 | Warna kabel | Merah Exc+, hitam Exc-, hijau Out+, putih Out- | Tidak dinyatakan | **Tidak dinyatakan** (S2) | Merah In+, hitam In-, hijau Out+, putih Out- pada diagram; konsisten. Bertentangan dengan listing modul HX711 (DS-HX1, Peringatan 1) |
| 12 | Kabel kelima | Diagram listing berlabel GND | Tidak dinyatakan | **Tidak dinyatakan** (S2) | Tidak ada (4 kawat). Dugaan: drain perisai; uji kontinuitas |
| 13 | Kabel | Sekitar 28 cm | Tidak dinyatakan | **Tidak dinyatakan** (S2) | 4 mm, 0,42 m (catatan lain di dokumen yang sama: 3000 mm). Ukur |
| 14 | Dimensi | 130 x 28 x 22 mm | Tidak dinyatakan | **Tidak dinyatakan** (S2); **Konflik** lebar | 130 x **30** x 22 mm. Ukur lebar dengan jangka sorong |
| 15 | Lubang pasang | 4 x M6; 106 mm antar pasangan, 12 mm dari ujung, 15 mm melintang | Tidak dinyatakan | **Tidak dinyatakan** (S2) | Pada gambar ukuran; tembus/buta, kedalaman ulir, dan ujung tetap/bebas tidak tertulis |
| 16 | Suhu | Tidak dinyatakan | Kompensasi -10 sampai +40; operasi -35 sampai +80 derajat C | **Tidak dinyatakan** (S1) | Kompensasi -10 sampai +40; pengaruh suhu pada nol dan span <0,03% FSO/10 derajat C |

### Tabel B: parameter yang kita pakai tetapi tidak disebut vendor

| Parameter | S1 | S2 | S3: GJ Impex | Dipakai di |
|---|---|---|---|---|
| Zero balance | Tidak dinyatakan | Tidak dinyatakan | <+-1% FSO (sampai sekitar 0,8 kg pada 80 kg; dihilangkan tare) | Tare, rentang ADC |
| Resistansi isolasi | Tidak dinyatakan | Tidak dinyatakan | >5000 MOhm | Uji awal |
| Bahan | Aluminium | Aluminium anodisasi | Paduan aluminium, anodisasi tanpa warna | Housing |
| Platform | Tidak dinyatakan | Tidak dinyatakan | 250 x 350 mm | Uji off-centre |

### Peringatan untuk desain kita

1. **Lebar 28 vs 30 mm:** terselesaikan oleh pengukuran sendiri: 28 mm (10 Okt. 2026), cocok dengan Eagle Weigh; angka 30 mm pada dokumen GJ Impex tidak berlaku untuk barang kita. Tinggi terukur 21,5 mm.
2. **Eksitasi 3,3-5 V di bawah semua rentang yang tercantum** (9-12 atau 10-15 V): keluaran absolut lebih kecil (lihat noise di DS-HX1). Tanyakan ke vendor; verifikasi dengan uji linearitas sendiri.
3. **Overload konservatif 120%/150%.** Gaya genggam tertinggi pada data Gotthelf sekitar 59 kgf (usia 20-24) sampai 78 kgf (usia 30-34), masih di bawah 96 kgf (120%).
4. **Creep:** jangan pakai 0,0016%; pakai <0,025%.
5. **Error gabungan +-0,03 %RO sekitar +-24 g**; ini batas bawah ketidakpastian kita (pemasangan dan kalibrasi menambah).
6. **Warna kabel:** pengukuran resistansi 10 Okt. 2026 menunjukkan eksitasi = hitam dan putih, sinyal = merah dan hijau (bukan seperti datasheet). Jangan menyambung berdasarkan warna datasheet. Polaritas dicek lewat tanda pembacaan di uji pertama; bila terbalik, tukar dua kabel sinyal.
7. **Kabel kelima:** hubungkan perisai di sisi elektronik saja, setelah uji kontinuitas.
8. **Arah pembebanan** (ujung tetap/bebas) belum diketahui; tentukan sebelum cetak final.

### Daftar cek saat load cell tiba

| No | Pemeriksaan | Hasil |
|---|---|---|
| 1 | Label barang: kapasitas, mV/V, nomor seri, tanda panah beban | |
| 2 | Jangka sorong: panjang, lebar (28 atau 30?), tinggi | **130 x 28 x 21,5 mm** (diukur sendiri, 10 Okt. 2026). Lebar 28 mm cocok dengan Eagle Weigh; tinggi 21,5 mm lebih kecil 0,5 mm dari 22 mm pada Eagle (toleransi atau resolusi alat belum dicatat) |
| 3 | Posisi lubang: 106 / 12 / 15 mm | |
| 4 | Lubang M6 tembus atau buta; kedalaman ulir | |
| 5 | Ujung tetap dan ujung bebas | |
| 6 | Resistansi: E+/E- (sekitar 400), S+/S- (sekitar 350) | **Diukur 10 Okt. 2026** (DT9205A, rentang 2 kΩ, pembacaan 0,001 kΩ): merah-hitam 306 Ω (sempat berkedip 400 Ω); hijau-putih 260 Ω; merah-hijau 347 Ω; merah-putih 260 Ω; hitam-hijau 306 Ω; hitam-putih 393 Ω. Hasil: **pasangan eksitasi = hitam-putih (393 Ω; datasheet 401 ± 10, lolos)**, **pasangan sinyal = merah-hijau (347 Ω; datasheet 350 ± 5, lolos)**. Empat pasangan silang 260 sampai 306 Ω cocok dengan perkiraan hitungan jembatan (260 sampai 310 Ω). Ketidakpastian alat sekitar ±5 Ω. **Pemetaan warna pada barang ini berbeda dari datasheet** (datasheet: merah/hitam eksitasi, hijau/putih sinyal). Urutan baris pembacaan mengikuti catatan pengguna; polaritas + dan - belum ditentukan (hanya membalik tanda) |
| 7 | Kawat kelima: kontinuitas ke badan dan perisai | |
| 6a | Penyolderan ke HX711 yang bekerja | hitam E+, putih E−, hijau A+, merah A− (gaya tekan terbaca positif; 24 Okt. 2026) |
| 8 | Panjang dan diameter kabel | |
| 9 | Offset nol tanpa beban | |
| 10 | Tanda pembacaan dengan hijau = S+, putih = S- | |

### Daftar pustaka DS-LC1 (IEEE)

Nomor melanjutkan DS-HX1 ([1]-[3]); berlaku di dokumen ini saja.

[4] Automa-88, "Loadcell CZL 601 3-80KG digital scale sensor," Shopee Indonesia, product listing. [Online]. Available: https://shopee.co.id/Loadcell-CZL-601-3-80KG-Digital-Scale-Sensor-i.43040199.24877149110 (diakses 8 Okt. 2026).

[5] Eagle Weigh, "CZL 601 single point load cell," product page. [Online]. Available: https://www.eagleweigh.com/products/loadcells/czl-601/ (diakses 8 Okt. 2026).

[6] GJ Impex, "CZL-601 load cell," datasheet, 2023. [Online]. Available: https://5.imimg.com/data5/SELLER/Doc/2023/2/TZ/XA/HZ/8729711/czl601-aluminium-load-cell.pdf (diakses 8 Okt. 2026).

**Catatan sitasi (hapus setelah dicek):**
- [4]: judul diambil dari bagian URL; samakan dengan judul di halaman produk. Parameter `extraParams` pada tautan asli sengaja dibuang. [5]: halaman web tanpa tanggal.
- [6]: PDF di-hosting di imimg.com (CDN IndiaMART), milik penjual; penerbit GJ Impex dan tahun 2023 dari isi berkas dan jalur URL (2023/2). Tidak ada nomor dokumen.
- Angka "sekitar" dan +-24 g adalah hitungan kita, bukan angka sumber.

---

## DS-ESP1: Modul ESP32-WROOM-32D

### Identitas dan bukti

| Item | Isi |
|---|---|
| Modul | ESP32-WROOM-32D (antena PCB), 4 MB flash, chip ESP32-D0WD, 18 x 25,5 x 3,1 mm, 38 pin. **Pilihan tim.** |
| S1 | Keputusan tim: modul disediakan lab dan dipasang pada DevKit V1 (lihat DS-ESP2). Tidak ada listing vendor. |
| S2 | Espressif Systems, *ESP32-WROOM-32D & ESP32-WROOM-32U Datasheet*, v2.8 [7]. Datasheet pabrikan. |
| Status siklus hidup | Halaman datasheet bertanda **"Not Recommended For New Designs (NRND)"**. Modul masih bisa dipakai dan terdaftar sebagai varian pada papan DevKitC V4 [8]; tidak ada dampak pada desain kita, tetapi laporan sebaiknya menyebutnya. |
| S2 (chip) | Espressif Systems, *ESP32 Series Datasheet*, v5.3, Juli 2026 [10]. Datasheet chip (D0WD termasuk dalam daftar, status NRND), dipakai untuk **angka konsumsi arus** yang tidak ada di datasheet modul (Bagian 5.4 modul hanya merujuk ke datasheet chip ini). |
| Catatan | Angka arus di bawah adalah arus **chip** (bukan papan DevKit). Regulator dan chip USB pada papan menambah arus (lihat DS-ESP2, DS-REG1). |

### Tabel A: parameter yang dipakai proyek (sumber: S2)

| Parameter | Nilai S2 | Dipakai di | Catatan untuk proyek |
|---|---|---|---|
| Tegangan catu modul | 3,0 - 3,6 V (tipikal 3,3 V) | Catu | Maks mutlak 3,6 V (Tabel 13) |
| Arus catu yang harus disediakan | >= 0,5 A | Catu | Syarat pada catu eksternal; regulator papan harus sanggup (DS-REG1) |
| Tegangan masukan tinggi (VIH) | min 0,75 x VDD = sekitar 2,5 V; **maks VDD + 0,3 = sekitar 3,6 V** | Level logika HX711 | **Mengonfirmasi Peringatan 2 di DS-HX1:** pin DT 5 V di atas maksimum GPIO. Modul HX711 harus dicatu 3,3 V atau DT diberi level shifter |
| Tegangan masukan rendah (VIL) | maks 0,25 x VDD = sekitar 0,8 V | Tombol, I2C | - |
| Arus keluaran per pin | sumber 40 mA, sink 28 mA (drive maksimum; sumber turun ke sekitar 29 mA bila banyak pin aktif) | LED | LED + resistor tetap harus membatasi arus jauh di bawah ini |
| Arus keluaran kumulatif maks | 1.100 mA (Tabel 13) | - | Tidak relevan di proyek ini |
| Pull-up/pull-down internal | sekitar 45 kohm | Tombol GPIO 25 | Pull-up internal cukup untuk tombol |
| ADC | 12 bit, perbedaan antar chip +-6% bawaan, galat total sampai +-60 mV setelah kalibrasi (atenuasi 3) | - | **Tidak dipakai untuk load cell**; pengukuran gaya memakai HX711 eksternal |
| Temperatur ambien | -40 sampai 85 derajat C | - | Aman untuk pemakaian dalam ruangan |
| Flash | 4 MB, 100.000 siklus tulis, retensi 20 tahun | Logging | Tidak ada PSRAM, jadi GPIO 16/17 bebas |
| GPIO 6-11 | Terhubung ke flash di modul; tidak disarankan dipakai | Alokasi pin | Tidak dipakai di proyek |
| GPIO 34, 35, 36, 39 | Hanya masukan (tipe I) | Alokasi pin | Tidak dipakai di proyek |
| Pin strapping | GPIO0 (pull-up), GPIO2 (pull-down), MTDI = GPIO12 (pull-down), MTDO = GPIO15 (pull-up), GPIO5 (pull-up) | Alokasi pin | GPIO0 dan GPIO2 menentukan mode boot (Tabel 6); jangan dipakai untuk tombol atau LED yang menahan level saat reset. Tidak dipakai di proyek |

### Tabel B: konsumsi arus ESP32 (S2 chip [10])

Modem-sleep = radio mati, CPU hidup (keadaan normal alat bila Wi-Fi tidak dipakai). Datasheet: frekuensi CPU berubah otomatis menurut beban dan periferal yang dipakai.

| Mode | Kondisi | Arus | Dipakai di | Bisa diperiksa sendiri? |
|---|---|---|---|---|
| Modem-sleep 240 MHz | Chip dual-core | 30 - 68 mA | Anggaran daya, skenario atas | **Ya**, multimeter seri atau USB inline meter dengan firmware proyek |
| Modem-sleep 160 MHz | Dual-core | 27 - 44 mA | Anggaran daya | Ya |
| Modem-sleep 80 MHz | Dual-core | 20 - 31 mA | Anggaran daya, skenario bawah | Ya |
| Wi-Fi TX 802.11b, +19,5 dBm | Duty 50%, 3,3 V, 25 derajat C | 240 mA (tipikal) | Puncak untuk stretch goal Wi-Fi | Hanya rata-rata; puncak butuh alat cepat |
| Wi-Fi TX 802.11g, +16 dBm | Idem | 190 mA | Idem | Idem |
| Wi-Fi TX 802.11n MCS7, +14 dBm | Idem | 180 mA | Idem | Idem |
| Wi-Fi RX | Idem | 95 - 100 mA | Idem | Idem |
| BT/BLE TX 0 dBm; RX | Idem | 130 mA; 95 - 100 mA | Tidak dipakai | - |
| Light-sleep, deep-sleep, hibernation, power off | - | 0,8 mA; 5 uA - 150 uA; 1 uA | Tidak dipakai (alat selalu hidup) | Tidak (arus papan menutupi) |

Syarat uji Tabel 5-4: catu 3,3 V, suhu 25 derajat C, diukur di port RF. Tabel modem-sleep tidak menyebut tegangan dan suhu.

### Alokasi pin proyek diperiksa terhadap S2

| Fungsi | GPIO | Fungsi lain pada modul (S2, Tabel 3) | Pin khusus? | Hasil |
|---|---|---|---|---|
| HX711 DT | 18 | VSPICLK, HS1_DATA7 | Bukan strapping, bukan flash, bukan input-only | Lolos |
| HX711 SCK | 19 | VSPIQ, U0CTS | Idem | Lolos |
| LCD SDA | 21 | VSPIHD | Idem; default I2C Arduino | Lolos |
| LCD SCL | 22 | VSPIWP, U0RTS | Idem; default I2C Arduino | Lolos |
| Tombol | 25 | DAC_1, ADC2_CH8 | Idem | Lolos |
| LED | 27 | ADC2_CH7, TOUCH7 | Idem | Lolos |

Enam pin ini lolos aturan "tidak ada pin dobel, pin input-only dan strapping tidak disalahgunakan".

### Peringatan untuk desain kita

1. **Level logika HX711 (lihat DS-HX1).** VIH maks sekitar 3,6 V.
2. **Catatan proyek menulis ESP32-WROOM-32 dan kekhawatiran WROVER.** Varian pilihan tim adalah WROOM-32D (tanpa PSRAM), jadi kekhawatiran GPIO 16/17 tidak berlaku; BOM perlu diperbarui.
3. **Anggaran daya:** pakai 31 mA (80 MHz) dan 68 mA (240 MHz) sebagai skenario dengan Wi-Fi mati; 240 mA sebagai puncak Wi-Fi. Ini arus chip; tambahkan arus papan, HX711, jembatan, LCD, dan LED. Memakai 80 MHz memotong anggaran sekitar separuh, tetapi cukup tidaknya untuk bit-bang HX711 dan LCD perlu dites di papan.
4. **NRND:** sebutkan di laporan sebagai catatan.

### Hasil uji papan sendiri (8 Okt. 2026, tanpa multimeter)

Bukan sumber S1/S2/S3; ini pengukuran sendiri. Alat: papan DevKit V1 + kabel USB; sketsa `esp32_uji_awal.ino` (di `firmware/sketches/esp32_uji_awal/`) (Arduino IDE 2.3.10, core esp32 3.3.11, ESP-IDF v5.5.5), Serial Monitor 115200 baud, catu hanya dari USB.

| Pemeriksaan | Hasil | Dibandingkan dengan |
|---|---|---|
| Model chip | ESP32-D0WD-V3, revisi v3.1 (301), 2 core, kristal 40 MHz | Cocok dengan D0WD pada daftar S2 chip [10]. Chip yang sama juga dipakai modul WROOM-32E, jadi uji ini mengonfirmasi chip, bukan huruf akhiran modul (D atau E): cetakan pelindung modul tetap perlu dilihat |
| Flash | 4 MB eksternal @ 80 MHz | Cocok dengan Tabel A (4 MB) |
| PSRAM | 0 byte | Cocok (WROOM-32D tanpa PSRAM) |
| Alasan reset | POWERON, tidak ada BROWNOUT | Catu USB cukup pada kondisi uji (belum ada beban HX711/LCD) |
| Loopback GPIO 18-19, 21-22, 25-27, dua arah, LOW dan HIGH | 6 dari 6 LULUS | Keenam pin proyek dapat mengeluarkan dan membaca level logika. **Hanya uji logika digital:** tegangan nyata, arus pin, dan nilai pull-up internal belum terukur |
| Pindai I2C (SDA 21, SCL 22) | Tidak ada perangkat | Wajar, backpack belum terpasang; pindai memakan sekitar 5,5 detik karena jumper 21-22 menghubungkan SDA dan SCL. Ulangi tanpa jumper dengan backpack terpasang |
| Lebar pulsa SCK tertinggi, interupsi aktif | 240 MHz: 6,4 - 7,0 us; 160 MHz: 8,9 - 9,4 us; 80 MHz: 18,5 - 21,3 us (Wi-Fi mati dan aktif; Wi-Fi aktif hanya mencoba SSID palsu, tanpa lalu lintas data) | Batas HX711: SCK tinggi maks 50 us untuk pembacaan data, power-down jika lebih dari 60 us [2]. Semua di bawah 50 us; margin terkecil sekitar 2,4 kali (80 MHz, Wi-Fi aktif). Interupsi dimatikan: maks 3,9 - 6,7 us |

Kesimpulan: CPU 240 MHz (bawaan) atau 160 MHz aman untuk bit-bang HX711; 80 MHz masih di bawah batas. Pelindung `noInterrupts()` di sekitar 24 clock tidak wajib berdasarkan data ini, hanya pengaman tambahan.

### Hasil ukur multimeter (10 Okt. 2026)

Pengukuran sendiri dengan multimeter digital (DT9205A), rentang 20 V DC (DS-INS1). Kondisi: papan DevKit V1 dicatu dari USB laptop, tanpa HX711, LCD, atau load cell terpasang. Pembacaan terlihat stabil setelah beberapa detik.

| Titik | Terbaca | Rentang yang masuk akal (hasil + ketidakpastian alat) | Dibandingkan dengan |
|---|---|---|---|
| Pin 3V3 terhadap GND | 3,30 V (stabil) | 3,26 sampai 3,34 V (± 0,037 V) | Sesuai nominal regulator 3,3 V. Catu logika ESP32 dan HX711 berada pada nilai yang diharapkan |
| Pin VIN terhadap GND | 5,02 - 5,04 V; paling stabil 5,03 V | 4,98 sampai 5,08 V (± 0,045 V) | Di dalam rentang catu USB (5 V ± 5% menurut spesifikasi USB 2.0). Tegangan jalur 5 V dari laptop hampir tanpa penurunan |

Belum diukur: 3V3 dan 5 V dengan beban (HX711, LCD, LED), arus total.

### Daftar cek saat modul tiba

| No | Pemeriksaan | Hasil |
|---|---|---|
| 1 | Cetakan pada pelindung modul: tertulis ESP32-WROOM-32D? | Cetakan terbaca "ESP-32" dan "XXSR69" saja; chip terkonfirmasi D0WD-V3 lewat perangkat lunak (lihat hasil uji di atas). Huruf akhiran modul belum pasti |
| 1a | Tegangan 3V3 tanpa beban (DT9205A, 20 V DC) | **3,30 V**, 10 Okt. 2026 (lihat hasil ukur multimeter di atas) |
| 1b | Tegangan VIN tanpa beban | **5,03 V** (terbaca 5,02 - 5,04 V), 10 Okt. 2026 |
| 2 | Tegangan 3V3 saat beban (HX711 + LCD + LED). Alat DT9205A, rentang 20 V DC, ketidakpastian sekitar ±0,04 V pada 3,3 V (DS-INS1) | **3,25 V** di sisi ESP32, **3,17 V** di pin modul (24 Okt. 2026, [hasil-multimeter-hgd.md](../pengukuran/hasil-multimeter-hgd.md) Bagian 2) |
| 2a | Arus total dari USB pada 80 MHz dan 240 MHz dengan firmware proyek, Wi-Fi mati; bandingkan dengan Tabel B. Alat DT9205A, rentang 200 mA DC lewat jack mA, dipasang seri di jalur 5 V (langkah dan batas di DS-INS1; sekring alat 200 mA); ketidakpastian sekitar ±(1,4% + 0,2 mA), dan layar hanya diperbarui tiap 2-3 detik sehingga puncak sesaat tidak terbaca | *belum diukur* |
| 3 | Enam pin proyek terbaca pada tes GPIO sederhana | **Lulus**, 8 Okt. 2026 (loopback 6 dari 6 arah, lihat hasil uji di atas) |

---

## DS-ESP2: Papan ESP32 DevKit V1

### Identitas dan bukti

| Item | Isi |
|---|---|
| Papan | ESP32 DevKit V1 (desain DOIT, kelas 30 pin), memuat modul ESP32-WROOM-32D (DS-ESP1). **Pilihan tim.** |
| S1 | Keputusan tim: papan disediakan lab. Tidak ada listing vendor. |
| S2 | Espressif Systems, *esp-dev-kits documentation (ESP32)*, bab ESP32-DevKitC V4 dan V2 [8]. **Papan berbeda dari V1** (DevKitC V4 38 pin, buatan Espressif; V2 hanya halaman ringkas). Dipakai hanya untuk **fungsi pin**, yang berasal dari chip dan modul yang sama. |
| S3 | ESPboards, halaman *DOIT ESP32 DevKit V1* [9]. Pihak ketiga (bukan pabrikan DOIT), tetapi halaman mencantumkan rujukannya sendiri (datasheet papan, datasheet ESP32 Espressif, berkas KiCad, skema PDF, dokumen boot mode Espressif). Dipakai untuk **posisi pin, regulator, dan chip USB papan V1**. Halaman menyebut modul ESP32-WROOM-32 (bukan 32D) dan menyatakan papan di pasaran umumnya klon dengan tata letak dan pinout yang sama. |
| Catatan | S2 tidak mencakup V1. Skema V1 tidak kita pegang sendiri; S3 hanya mencantumkan tautannya (skema PDF, KiCad). Rincian regulator dan chip USB di bawah berasal dari S3 dan harus dicocokkan dengan papan yang sebenarnya (papan klon bisa berbeda). |

### Tabel A: papan V1 (S3) dibandingkan DevKitC V4 (S2)

| Item | V1 (S3, gambar) | DevKitC V4 (S2) | Hasil | Dampak |
|---|---|---|---|---|
| Jumlah pin header | 30 (25 GPIO + GND, 5V/VIN, 3V3) | 38 (19 per sisi) | **Konflik** | Pakai penomoran V1 untuk posisi |
| Pin catu | VIN/5V (masukan 5 V dari USB atau sumber luar), 3V3 (keluaran teregulasi 3,3 V), GND | 5V (J2), 3V3, 2 x GND | Konflik pada label (VIN vs 5V) | Pada V1 pin catu masuk berlabel VIN; ukur saat papan tiba |
| Pin D0-D3, CMD, CLK (GPIO 6-11, flash) | Tidak dikeluarkan | Dikeluarkan, dengan catatan "hindari" | Konflik (V1 lebih aman) | Tidak ada risiko salah pakai |
| GPIO0 | Tidak dikeluarkan (tombol BOOT di papan) | Dikeluarkan (J3 no. 14) | Konflik | GPIO0 tidak tersedia di header V1 |
| Fungsi pin GPIO (ADC, touch, DAC, strapping) | Sesuai tabel pin S3 | Sesuai Tabel 3 modul dan Tabel header V4 | **Konsisten** | Fungsi berasal dari chip yang sama |
| Catu | Pin VIN dan 3V3 tersedia; aturan satu sumber tidak dinyatakan | Tiga cara, tidak boleh dua sekaligus: Micro-USB, 5V, 3V3 | Tidak dinyatakan untuk V1 | Perlakukan sama: **satu sumber saja** |
| Catatan C15 (boot ke mode download, mengganggu clock GPIO0) | - | Khusus V4 awal | Tidak berlaku/tidak diketahui untuk V1 | Jika papan masuk mode download sendiri, periksa C15 |
| USB-UART | CP2102 pada desain asli; klon sering memakai CH340 (S3) | Satu chip USB-UART (3 Mbps) | Beda papan; chip pada papan kita belum diketahui | Cek penanda chip (driver komputer) |
| Regulator | AMS1117-3.3 (S3). Teks "About" di halaman S3 menyebut "high-efficiency LDO", yang tidak cocok dengan AMS1117 | Tidak dibahas pada bab yang kita pakai | **Konflik di dalam S3** | Penting untuk noise catu dan batas arus; selesaikan lewat DS-REG1 dan penanda pada papan |
| Rangkaian auto-reset | Tidak dinyatakan | Tombol EN dan BOOT dijelaskan | Tidak dinyatakan untuk V1 | - |

### Posisi pin proyek pada papan V1 (S3, tabel pin)

| Fungsi | GPIO | Sisi | Label cetak (V1) |
|---|---|---|---|
| HX711 DT | 18 | Kanan | D18 |
| HX711 SCK | 19 | Kanan | D19 |
| LCD SDA | 21 | Kanan | D21 |
| LCD SCL | 22 | Kanan | D22 |
| Tombol | 25 | Kiri | D25 |
| LED | 27 | Kiri | D27 |
| Catu modul HX711 | 3V3 (kanan bawah) | Kanan | 3V3 |
| Catu LCD (backpack) | VIN (5 V dari USB atau sumber luar) atau 3V3; periksa kebutuhan backpack (DS-LCD1/DS-I2C1) | Kiri | VIN |

Semua enam GPIO proyek dikeluarkan di header V1.

### Peringatan untuk desain kita

1. Gunakan **satu** sumber catu pada papan: USB atau VIN atau 3V3, tidak dua sekaligus.
2. GPIO0 tidak ada di header V1. Tombol proyek tetap di GPIO25.
3. Jangan mengacu pada tabel header V4 untuk posisi pin; pakai tabel pin V1 (S3).
4. Regulator (AMS1117-3.3 menurut S3, tetapi S3 bertentangan dengan dirinya sendiri) dan chip USB (CP2102 atau CH340) harus dicocokkan dengan papan yang sebenarnya; selesaikan lewat DS-REG1. Skema V1 bisa diambil dari tautan yang tercantum di halaman S3.
5. S3 menyebut modul WROOM-32, bukan 32D; periksa cetakan modul pada papan kita.

### Daftar cek saat papan tiba

| No | Pemeriksaan | Hasil |
|---|---|---|
| 1 | Foto papan: label pin, tombol EN/BOOT, jenis soket USB | |
| 2 | Cetakan modul (WROOM-32D?) | |
| 3 | Penanda regulator (AMS1117-3.3?) dan chip USB-UART (CP2102 atau CH340?) | |
| 4 | Tegangan pin 3V3 dan VIN dengan USB tersambung | **3,30 V dan 5,03 V**, 10 Okt. 2026 (DT9205A, tanpa beban) |
| 5 | Posisi 18, 19, 21, 22, 25, 27 cocok dengan tabel pin S3 | Enam pin lulus loopback 8 Okt. 2026; kecocokan letak dengan label di papan belum dicatat |
| 6 | Jumlah pin header (30?) | **15 per baris, 30 total** (diukur sendiri, 10 Okt. 2026) |

### Dimensi papan (ukur sendiri, 10 Okt. 2026)

Papan DevKit V1 tidak punya gambar dimensi pabrikan di berkas kita, jadi dimensi ini hasil ukur sendiri (jangka sorong atau penggaris; alat dan resolusinya belum dicatat). Acuan pabrikan hanya untuk modul WROOM-32D (lihat tab ESP32 di [tabel pengukuran](../pengukuran/tabel-pengukuran-hgd.xlsx)).

| Item | Hasil | Catatan |
|---|---|---|
| Panjang x lebar papan | 51,5 x 28,2 mm | Panjang **sudah termasuk** area antena Wi-Fi (keterangan pengguna). Area antena tidak boleh tertutup tembaga atau komponen pada PCB pembawa |
| Jarak antar baris header, pusat ke pusat | 25,3 mm | Dihitung: 25,8 mm (tepi luar ke tepi luar kaki) - 0,5 mm (diameter kaki). Hampir 10 x 2,54 = 25,4 mm |
| Pitch pin | 2,5 mm (pembulatan pengukuran) | Dianggap 2,54 mm; cocok dengan panjang barisan |
| Jumlah pin per baris | 15 | - |
| Panjang barisan (pusat pin pertama ke terakhir) | 35,5 mm | (15 - 1) x 2,54 = 35,56 mm, cocok |
| Diameter kaki header | 0,5 mm | - |
| Konektor USB (micro-USB) | 2,7 x 5,7 x 7,7 mm (urutan: tinggi x panjang x lebar, **dugaan kita**) | Keterangan pengguna 10 Okt. 2026. Urutan dugaan dari ukuran micro-USB umum (lebar sekitar 7,5 mm, tinggi sekitar 2,5 mm). Konektor **tidak menonjol** melewati tepi papan (keterangan pengguna); angka 5,6 mm yang tertulis sebelumnya kemungkinan panjang ini |
| Tebal PCB papan | 1,0 mm | Diukur sendiri 10 Okt. 2026 |
| Tebal papan + modul ESP32 | 4,2 mm | Diukur sendiri. Modul = 4,2 - 1,0 = 3,2 mm; datasheet Espressif 3,10 ± 0,15 mm (maks 3,25), jadi **cocok dalam toleransi** |
| Tebal papan + pin header | 9,2 mm | Diukur sendiri. Dihitung: pin menonjol di bawah papan sekitar 9,2 - 1,0 = 8,2 mm (dugaan kita; bergantung apakah 9,2 diukur dari sisi atas PCB sampai ujung pin) |
| Tebal papan + plastik header | 3,4 mm | Diukur sendiri. Plastik header di bawah papan = 3,4 - 1,0 = 2,4 mm (hitungan kita; mendekati 2,5 mm pada header 2,54 mm umum) |
| Ujung pin di bawah plastik header | 5,8 mm | Hitungan kita: 9,2 - 3,4. Ini kedalaman yang masuk ke soket PCB pembawa |
| Tinggi di atas soket (bagian papan) | 6,6 mm | Hitungan kita: 2,4 + 1,0 + 3,2 = 6,6 mm dari bibir atas soket sampai bagian atas modul; cocok dengan 12,4 - 5,8 |
| Tinggi total, dari ujung pin header di bawah sampai komponen tertinggi | 12,4 mm | Sudah termasuk pin header (keterangan pengguna). Cek silang: 9,2 + 3,2 = 12,4 mm, cocok |

Sisa tebal kiri-kanan antara tepi papan dan baris pin: (28,2 - 25,3) / 2 = 1,45 mm per sisi (hitungan kita).

### Daftar pustaka DS-ESP1 dan DS-ESP2 (IEEE)

Nomor melanjutkan DS-LC1 ([4]-[6]).

[7] Espressif Systems, "ESP32-WROOM-32D & ESP32-WROOM-32U datasheet," v2.8. [Online]. Available: https://espressif.com/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf (diakses 8 Okt. 2026).

[8] Espressif Systems, "ESP32 esp-dev-kits documentation, Release master," ch. 1, "ESP32-DevKitC," Oct. 7, 2026. [Online]. Available: https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp-dev-kits-en-master-esp32.pdf (diakses 8 Okt. 2026).

[9] ESPboards, "DOIT ESP32 DevKit V1," espboards.dev, board page. [Online]. Available: https://www.espboards.dev/esp32/esp32doit-devkit-v1/ (diakses 8 Okt. 2026).

[10] Espressif Systems, "ESP32 series datasheet," v5.3, Jul. 2026. [Online]. Available: https://documentation.espressif.com/esp32_datasheet_en.pdf (diakses 8 Okt. 2026).

**Catatan sitasi (hapus setelah dicek):**
- [7]: URL diambil dari catatan di halaman 1 dokumen (tautan ke versi terbaru). Versi v2.8 dan tanda NRND dari berkas yang tersedia.
- [8]: berkas yang tersedia (esp-dev-kits-en-master-esp32-pages.pdf) adalah kumpulan halaman dari PDF lengkap pada URL di atas; hanya bab DevKitC (V4 dan V2) yang dipakai. Nama berkas dan judul "Release master" cocok; PDF lengkap terlalu besar untuk dibuka di sini, jadi tanggal 7 Okt. 2026 diambil dari halaman sampul berkas Anda.
- [9]: halaman tidak mencantumkan tanggal terbit; hanya catatan hak cipta 2026. Penulis halaman tidak tertulis (metadata berisi nilai tempat "title"), jadi situs dipakai sebagai penerbit.
- [10]: angka dari Tabel 4-2 (Bagian 4.3.1, hlm. 30) dan Tabel 5-4 (Bagian 5.4, hlm. 52) berkas yang tersedia. Tautan lama espressif.com dialihkan ke alamat ini; tanggal Juli 2026 dari riwayat revisi. Judul DS-ESP1 dan DS-ESP2 berbagi daftar pustaka ini.
- Hasil "Lolos" dan perbandingan V1 dengan V4 adalah pemeriksaan kita terhadap dokumen, bukan angka dari sumber.

---

## DS-LCD1: Modul LCD HS1602A

### Identitas dan bukti

| Item | Isi |
|---|---|
| Modul | LCD karakter 16 x 2, HS1602A, buatan Shenzhen Hansheng Industrial Co., Ltd.; pengontrol SPLC780 (kompatibel HD44780). Penandaan HS1602A dari foto vendor. |
| S1 | Listing vendor CNC Store Bandung (toko yang sama dengan modul HX711) [11]: tabel spesifikasi di bawah dan foto yang menunjukkan HS1602A. Tautan: https://shopee.co.id/LCD-Display-Character-16x2-1602-5V-Modul-Hijau-Biru-Abu-Backlight-Pilihan-I2C-atau-Non-I2C-untuk-Arduino-DIY-i.62956347.4011320491 (tanggal akses: 8 Okt. 2026). Varian: DSP-0011 hijau, DSP-0113 abu-abu, DSP-0002 biru (non-I2C); DSP-0014 biru + I2C, DSP-0052 hijau + I2C. **ISI** varian yang dibeli. |
| S2 | Shenzhen Hansheng Industrial Co., Ltd., *HS1602A Datasheet*, Ver. 20110521 [12]. Salinan di halaman bagian JLCPCB C5329588 (pabrikan: Hansheng). |
| Catatan | Datasheet 2011, ditulis ringkas dan dengan beberapa ketidakkonsistenan (lihat Tabel C). |

### Tabel A: spesifikasi vendor (S1) dan pemeriksaan silang ke datasheet HS1602A (S2)

| No | Parameter | S1: vendor | S2: HS1602A | Hasil | Catatan |
|---|---|---|---|---|---|
| 1 | Tipe layar | LCD 16 x 2 | 16 x 2 titik karakter | **Konsisten** | Ringkasan S2 salah menulis "1 line" |
| 2 | Tegangan input | 5 V DC (power supply atau USB) | 5,0 V (4,9-5,1 V) | **Konsisten** | Catu dari VIN/USB papan |
| 3 | Ukuran karakter | 5 mm x 8 mm per karakter | Matriks 5 x 8 titik; jarak titik 0,6 x 0,54 mm; ukuran karakter dalam mm tidak tertulis | **Konflik** | Angka vendor kemungkinan menyalin "5 x 8 titik" sebagai mm. Dari jarak titik, lebar karakter sekitar 3 mm (perkiraan kita) |
| 4 | Warna lampu latar | Hijau, biru, abu-abu (pilihan) | Tidak dinyatakan | **Tidak dinyatakan** (S2) | Arus LED 18 mA di S2 untuk lampu latar bila ada |
| 5 | Antarmuka | I2C atau non-I2C (16 pin) | Paralel 16 pin 2,54 mm | **Konsisten** (non-I2C); I2C tidak tercakup S2 | Varian I2C = backpack (DS-I2C1) |
| 6 | Fungsi | Teks, angka, simbol | CGROM 192 karakter 5x8 | **Konsisten** | - |
| 7 | Kompatibilitas Arduino, Raspberry Pi, ESP32 | Ya | Tidak dinyatakan; VIH 2,2 V sehingga keluaran 3,3 V cukup | **Tidak dinyatakan** (S2) | LCD tetap perlu catu 5 V |
| 8 | Dimensi modul | 80 x 36 x **15** mm | 80 x 36 x 11,0 mm (maks 12,0 atau 9,0) | **Konflik** (tebal) | Selisih mungkin dari backpack atau pin (dugaan). Pakai 15 mm sampai diukur |
| 9 | Ukuran layar | 64,5 x **16** mm | Visible area 64,5 x 13,9 mm (gambar 13,8) | **Konflik** (tinggi) | Lebar sama. Jendela housing minimal 64,5 x 16 mm sampai diukur |

### Tabel B: parameter yang dipakai proyek tetapi tidak disebut vendor (sumber: S2 [12])

| Parameter | Nilai S2 | Dipakai di | Catatan |
|---|---|---|---|
| Catu VCC | **5,0 V** (rentang kerja 4,9-5,1 V; kondisi uji 5,0 +-0,5 V) | Catu | **modul praktikum V**, bukan 3,3 V. Catu dari pin VIN/USB, bukan 3V3 |
| Arus kerja | 1 mA | Anggaran daya | Tanpa lampu latar |
| Arus lampu latar (LED) | 18 mA | Anggaran daya | Pin 15/16 (BLA/BLK) tertulis "NC (LEDA/LEDK)": tidak jelas apakah modul punya lampu latar. Cek fisik; anggarkan 18 mA bila ada |
| VIH / VIL | 2,2 V sampai VDD / -0,3 sampai 0,6 V | Level logika | Keluaran PCF8574 pada 3,3 V (di atas 2,2 V) memenuhi VIH; keluaran ESP32 langsung juga memenuhi |
| VOH / VOL | min 2,4 V / maks 0,4 V | - | Hanya relevan bila R/W dibaca; backpack biasanya tidak membaca |
| Kontras (V0) | Tipikal 0,2 V; perlu trimpot 10 kohm ke VSS | Backpack | Biasanya trimpot ada di backpack; atur saat pertama |
| Antarmuka | 8-bit atau 4-bit paralel; 16 pin 2,54 mm | Backpack | Backpack memakai 4-bit |
| Waktu siklus E | 500 ns (4,5-5,5 V); 1000 ns (2,7-5,5 V) | Kecepatan I2C | Jauh lebih cepat dari I2C 100 kHz; bukan penghambat |
| Waktu instruksi | 42 us; Clear dan Return Home 1,64 ms (fosc 270 kHz) | Firmware | Tunggu setelah Clear |
| Alamat DDRAM | Baris 1: 0x00-0x0F; baris 2: 0x40-0x4F | Firmware | - |
| Dimensi modul | 80 x 36,0 x (maks 12,0 atau 9,0) mm | Housing | Tebal tergantung varian |
| Area tampil (visible) | 64,5 x 13,9 mm (gambar: 13,8) | Jendela housing | Rancang jendela dengan margin; bandingkan dengan tabel D1-D6 di catatan proyek |
| Lubang pasang | 4 x diameter 2,5 mm, jarak 75,0 x 31,0 mm | Housing | - |
| Suhu | Uji pada -20 sampai +75 derajat C (tabel waktu) | - | Rentang operasi dan simpan modul tidak dinyatakan |
| Inisialisasi | Disarankan reset perangkat lunak (Function Set) sebelum dipakai | Firmware | Pustaka LiquidCrystal I2C menanganinya |

### Tabel C: ketidakkonsistenan dalam S2

| Hal | Isi | Dampak |
|---|---|---|
| Jumlah baris | Ringkasan: "1 line 16 character"; tabel dimensi dan DDRAM: 16 x 2 | Dianggap salah ketik; nama 1602 dan DDRAM dua baris menguatkan 16 x 2 |
| Area aktif | Gambar 55,45 mm, tabel 55,7 mm | Selisih kecil; ukur |
| Tebal | "MAX 12,0 (or 9,0)" | Ukur dengan jangka sorong |
| Lampu latar | Pin NC, tetapi arus LED 18 mA tertera | Cek fisik |

### Peringatan untuk desain kita

1. **modul praktikum V.** Catu dari 5 V (VIN papan atau USB), dan itu menentukan rangkaian backpack (lihat DS-I2C1, Peringatan 1).
2. **Anggaran daya LCD:** sekitar 1 mA ditambah 18 mA lampu latar bila ada, dari rel 5 V.
3. **Kontras:** atur trimpot saat pertama; tulisan tidak tampak atau semua kotak terisi bukan berarti rusak.

### Daftar cek saat LCD tiba

| No | Pemeriksaan | Hasil |
|---|---|---|
| 1 | Penandaan pada PCB: HS1602A? | |
| 2 | Lampu latar ada atau tidak (pin 15/16 terhubung ke LED) | |
| 3 | Dimensi PCB dan area tampil dengan jangka sorong; tebal modul | PCB 80 x 36; bingkai hitam 71,5 x 24; layar 64,5 x 14,0; tinggi total dengan backpack 16,9 mm (15 Okt. 2026; tab LCD I2C di tabel pengukuran) |
| 4 | Arus dengan dan tanpa lampu latar (multimeter seri) | |

---

## DS-I2C1: Backpack I2C (PCF8574/PCF8574A)

### Identitas dan bukti

| Item | Isi |
|---|---|
| Komponen | Backpack I2C untuk LCD 1602 pada listing vendor; chip yang tertera pada backpack **belum diperiksa**. Dugaan: PCF8574 atau PCF8574A (alamat 0x27 atau 0x3F pada catatan kita). |
| S1 | Listing vendor CNC Store Bandung [11]: varian "+ I2C" (DSP-0014 dan DSP-0052). Spesifikasi listing hanya menyebut "Interface: I2C"; tidak ada chip, alamat, atau level logika. **ISI** penandaan chip dan jumper alamat dari foto atau saat barang tiba. |
| S2 | NXP Semiconductors, *PCF8574; PCF8574A: Remote 8-bit I/O expander for I2C-bus with interrupt*, Rev. 5, 27 Mei 2013 [13]. Datasheet chip (resmi). |
| S3 | lady ada (Adafruit), *I2C/SPI LCD Backpack* [14], diperbarui 18 Sep. 2026. **Bukan backpack yang sama:** memakai MCP23008 (I2C) dan 74HC595 (SPI), alamat bawaan 0x20, rangkaian boost 3-5 V dan pergeseran level di SDA/SCL. Dipakai hanya sebagai contoh desain dan untuk pernyataan umum. |
| Catatan | Backpack murah biasanya memakai PCF8574 dengan resistor pull-up langsung ke VCC. Itu **belum terbukti** untuk barang kita; periksa. |

### Tabel A: PCF8574 (sumber: S2 [13])

| Parameter | Nilai S2 | Dipakai di | Catatan |
|---|---|---|---|
| Catu VDD | 2,5 - 6 V (maks mutlak 7 V) | Catu | Cocok dengan 5 V dan 3,3 V |
| Arus operasi | 40 uA tipikal, maks 100 uA (VDD = 6 V, 100 kHz); standby 2,5 uA | Anggaran daya | Dapat diabaikan |
| Frekuensi I2C | **maks 100 kHz** (Standard-mode) | Firmware | Pakai 100 kHz (bawaan Wire); jangan 400 kHz |
| VIH / VIL | 0,7 x VDD / 0,3 x VDD | Level logika | Pada VDD = 5 V: VIH = 3,5 V; pada 3,3 V: 2,31 V (hitungan kita) |
| Tegangan masukan maks | VDD + 0,5 V | Level logika | I/O tidak toleran-tegangan-lebih |
| Keluaran port (P0-P7) | IOL min 10 mA (tipikal 25 mA) pada VOL = 1 V; IOH 30-300 uA (sumber arus lemah, 100 uA) | LCD | Cukup untuk menggerakkan masukan LCD |
| Arus total paket | 80 mA | - | Aman |
| Alamat | PCF8574: 0x20-0x27; PCF8574A: 0x38-0x3F (A2, A1, A0 tanpa pull-up internal) | Firmware | **0x27** dan **0x3F** adalah alamat paling atas masing-masing seri (A2 = A1 = A0 = 1): konsisten dengan catatan kita (hitungan kita) |
| Waktu naik/turun SDA, SCL | maks 1 us / 0,3 us | Pull-up | R <= tr / (0,8473 x C): dengan C = 100 pF, R <= sekitar 11,8 kohm (hitungan kita) |
| Suhu | -40 sampai +85 derajat C | - | Aman |

### Peringatan untuk desain kita

1. **Level logika I2C (masalah utama; diselesaikan 15 Okt. 2026 dengan level shifter BSS138 4 kanal, DS-LLS1, setelah SDA/SCL terukur 4,93 V).** LCD butuh 5 V (DS-LCD1), jadi backpack dicatu 5 V. Maka: (a) bila pull-up SDA/SCL pada backpack ke 5 V, garis berada di 5 V dan melebihi batas masukan ESP32 (sekitar 3,6 V, DS-ESP1); (b) bila pull-up ke 3,3 V, VIH PCF8574 pada 5 V adalah 3,5 V, sedikit di atas level 3,3 V, jadi tidak dijamin. **Ukur tegangan SDA/SCL saat diam** pada backpack yang tiba. Pilihan: pergeseran level (mis. modul 2-saluran), atau catu chip PCF8574 3,3 V terpisah dari catu LCD 5 V (keluaran 3,3 V tetap melewati VIH LCD 2,2 V), atau terima risiko setelah uji. S3 (Adafruit) mengatasinya dengan pergeseran level di papannya; backpack murah biasanya tidak.
2. **Alamat:** pindai bus I2C saat pertama (0x27 atau 0x3F); tulis eksplisit di kode.
3. **Kecepatan:** 100 kHz.
4. **S3 bukan backpack kita.** Jangan memakai alamat 0x20, MCP23008, atau pustaka Adafruit.

### Daftar cek saat backpack tiba

| No | Pemeriksaan | Hasil |
|---|---|---|
| 1 | Penandaan chip (PCF8574T? PCF8574AT? atau lainnya) | |
| 2 | Jumper A0-A2 dan alamat hasil pindai I2C | 0x27 (U4, 15 Okt. 2026) |
| 3 | Nilai dan tujuan resistor pull-up SDA/SCL (ke VCC?) | **4,67 kΩ** SDA ke VCC (multimeter Ω, catu mati, backpack lepas; 17 Okt. 2026): pull-up 4,7 kΩ ke VCC |
| 4 | Tegangan SDA dan SCL saat diam, dengan backpack dicatu 5 V | **4,93 V** (U5): pull-up ke VCC. Melebihi batas GPIO ESP32 → level shifter (DS-LLS1) |
| 5 | Jumper lampu latar dan trimpot kontras | Trimpot diputar sekitar 135° searah jarum jam. Pada catu 3,3 V tulisan tetap tidak kontras walau trimpot mentok; pada 5 V terbaca |
| 6 | Arus total LCD + backpack | |

### Daftar pustaka DS-LCD1 dan DS-I2C1 (IEEE)

Nomor melanjutkan DS-ESP ([10]).

[11] CNC Store Bandung, "LCD Display Character 16x2 1602 5V Modul Hijau Biru Abu Backlight Pilihan I2C atau Non I2C untuk Arduino DIY," Shopee Indonesia, product listing. [Online]. Available: https://shopee.co.id/LCD-Display-Character-16x2-1602-5V-Modul-Hijau-Biru-Abu-Backlight-Pilihan-I2C-atau-Non-I2C-untuk-Arduino-DIY-i.62956347.4011320491 (diakses 8 Okt. 2026).

[12] Shenzhen Hansheng Industrial Co., Ltd., "HS1602A datasheet," Ver. 20110521. [Online]. Available: https://jlcpcb.com/partdetail/Hs-HS1602A/C5329588 (diakses 8 Okt. 2026).

[13] NXP Semiconductors, "PCF8574; PCF8574A: Remote 8-bit I/O expander for I2C-bus with interrupt," Rev. 5, product data sheet, May 27, 2013. [Online]. Available: https://www.nxp.com/docs/en/data-sheet/PCF8574_PCF8574A.pdf (diakses 8 Okt. 2026).

[14] lady ada, "I2C/SPI LCD Backpack," Adafruit Learning System, Adafruit Industries, last updated Sep. 18, 2026. [Online]. Available: https://cdn-learn.adafruit.com/downloads/pdf/i2c-spi-lcd-backpack.pdf (diakses 8 Okt. 2026).

**Catatan sitasi (hapus setelah dicek):**
- [11]: judul diambil dari bagian URL; samakan dengan judul di halaman. Parameter `xptdk` pada tautan asli sengaja dibuang.
- [12]: tautan adalah halaman bagian JLCPCB yang menghosting datasheet pabrikan; pabrikan Hansheng dari halaman judul. Tidak ada nomor revisi selain tanggal "Ver: 20110521" (halaman judul juga memuat "VER 1.0/1.1").
- [13]: "PCF8574_PCF8574A" Rev. 5, 27 Mei 2013, dari berkas yang tersedia.
- [14]: penulis tertulis "lady ada"; panduan ini untuk MCP23008, bukan PCF8574. Disitasi sebagai contoh, bukan sumber spesifikasi backpack kita.
- Angka yang ditandai "hitungan kita" adalah perhitungan dari dokumen, bukan angka sumber.

---

## DS-LLS1: Modul level shifter dua arah 4 kanal (BSS138)

### Identitas dan bukti

| Item | Isi |
|---|---|
| Komponen | Modul penggeser level dua arah 4 kanal dengan MOSFET BSS138, dibeli dari penjual daring (listing tidak dicatat di sini). Sudah tiba; ukuran untuk PCB terukur dengan jangka sorong. |
| S1 | Listing penjual (tidak disimpan). |
| S3 | SparkFun, "Logic Level Converter - Bi-Directional" [17]: papan 16,05 x 13,33 mm. ProtoSupplies, "4-Channel Logic Level Converter Module" [18]: 15 x 13 mm, dua header 6 pin. Bukan pabrikan modul kita; dipakai untuk ukuran dan pinout umum. |

### Pinout dan pemakaian

| Sisi | Pin (urutan modul) | Dipakai |
|---|---|---|
| HV | HV1, HV2, HV, GND, HV3, HV4 | HV = +5V; HV1 = SDA LCD; HV2 = SCL LCD (J4) |
| LV | LV1, LV2, LV, GND, LV3, LV4 | LV = +3V3; LV1 = GPIO21; LV2 = GPIO22 (J3) |

### Peringatan untuk desain kita

1. **Jarak antar baris header** tidak dinyatakan di sumber mana pun; terukur pada modul yang tiba: 10,16 mm (0,4 in), dipakai untuk J3-J4.
2. **Modul terbalik 180° di soket** memberi 5 V ke sisi LV dan ke GPIO ESP32. Tandai pin 1 dan sisi HV/LV di silkscreen.
3. Kanal BSS138 dua arah dan bekerja dengan pull-up di kedua sisi, jadi cocok untuk I2C 100 kHz.
4. Alternatif yang tidak dipakai: 2N7000 THT (datasheet Microchip; V_GS(th) 0,8-3 V, R_DS(on) hanya dijamin pada V_GS 4,5 V).
5. **Hitungan pull-up I2C (17 Okt. 2026, terukur).** Shifter membagi bus menjadi dua segmen. Segmen LV (GPIO21/22, 3,3 V): 9,86 kΩ dari shifter, sekitar 8,1 kΩ bila pull-up internal ESP32 (sekitar 45 kΩ) aktif. Segmen HV (backpack, 5 V): 9,90 kΩ ‖ 4,67 kΩ = **3,17 kΩ**; arus tarik-turun 4,93 V / 3,17 kΩ ≈ 1,6 mA, di bawah batas 3 mA I2C mode standar. Kedua segmen di antara batas bawah sekitar 1 kΩ (arahan Modul 5) dan batas atas sekitar 11,8 kΩ (waktu naik, DS-I2C1). Semua pull-up yang ada milik modul; nilai gabungan tiap segmen sudah di dalam batas, jadi **PCB tidak perlu pull-up tambahan** (hitungan kita).

### Daftar cek saat modul tiba

| No | Pemeriksaan | Hasil |
|---|---|---|
| 1 | Ukur papan (panjang x lebar) | |
| 2 | Jarak antar baris header (pusat ke pusat) | 10,16 mm (jangka sorong) |
| 3 | Urutan pin tiap sisi dan penanda HV/LV di silkscreen | LV1, LV2, LV, GND, LV3, LV4 dan HV1, HV2, HV, GND, HV3, HV4 (silkscreen modul), sama dengan J3/J4 |
| 4 | Dengan HV = 5 V dan LV = 3,3 V: tegangan LV1/LV2 saat diam (harus sekitar 3,3 V) | LV1 = LV2 = **3,26 V**; HV1 = HV2 = **4,98 V** (modul hanya dicatu HV, LV, GND; tanpa LCD; 17 Okt. 2026) |
| 5 | Pull-up tiap kanal (kode SMD dan ukuran, catu mati, modul lepas) | 8 resistor bertanda `103`, satu per sisi per kanal. LV1 dan LV2 ke LV: **9,86 kΩ**; HV1 dan HV2 ke HV: **9,90 kΩ** (17 Okt. 2026) |
| 6 | Kontinuitas antarpin bertetangga setelah header disolder | Jembatan solder LV1-LV2 dan HV1-HV2 ditemukan dan diperbaiki; semua pasangan tidak berbunyi (17 Okt. 2026) |

### Daftar pustaka DS-LLS1 (IEEE)

[17] SparkFun Electronics, "SparkFun Logic Level Converter - Bi-Directional," product page. [Online]. Available: https://www.sparkfun.com/sparkfun-logic-level-converter-bi-directional.html (diakses 15 Okt. 2026).

[18] ProtoSupplies, "4-Channel Logic Level Converter Module," product page. [Online]. Available: https://protosupplies.com/?p=5169 (diakses 15 Okt. 2026).

---

## DS-INS1: Multimeter digital (buku petunjuk bawaan paket)

### Identitas dan bukti

| Item | Isi |
|---|---|
| Alat | Multimeter digital genggam, LCD 3-1/2 digit (1999 hitungan), A/D dual-slope, dicatu baterai 9 V (NEDA 1604 / 6F22). Model **DT9205A**, tertulis pada kotak kemasan (keterangan pengguna) |
| Sumber utama | Buku petunjuk pengoperasian yang **ikut dalam paket** [15]. Sampulnya "Digital Multimeter Operator's Instruction Manual"; nama model tidak tertulis di sampul, tetapi tertulis pada kotak kemasan yang sama dengan buku ini (keterangan pengguna); pabrikan, tanggal, dan revisi tidak tertulis pada halaman yang difoto. Spesifikasi berlaku satu tahun setelah kalibrasi pada 18 sampai 28 derajat C, RH sampai 80% |
| Sumber pembanding | Salinan "DT9205A" dari distributor [16] (kode halaman TOODMM02). **Angkanya berbeda dari buku bawaan** (lihat tabel perbandingan). Yang dipakai di entri ini adalah buku bawaan, karena itu yang menyertai alat |
| Kegunaan di proyek | Memeriksa tegangan catu (3V3, 5 V, eksitasi load cell), resistansi jembatan load cell, kontinuitas kabel dan perisai, serta arus total dari USB |

### Tabel spesifikasi yang dipakai (dari [15])

| Fungsi | Rentang | Akurasi | Resolusi (1 digit) | Dipakai untuk |
|---|---|---|---|---|
| Tegangan DC | 200 mV, 2 V, 20 V, 200 V | 0,5% + 2 digit | 0,1 mV; 1 mV; 10 mV; 0,1 V | 3V3 dan 5 V (rentang 20 V), keluaran jembatan (200 mV) |
| Tegangan DC | 1000 V | 0,8% + 2 digit | 1 V | Tidak dipakai |
| Arus DC | 2 mA, 20 mA | 1,2% + 2 digit | 1 uA; 10 uA | Arus kecil (LED, pull-up) |
| Arus DC | 200 mA | 1,4% + 2 digit | 0,1 mA | Arus total papan |
| Arus DC | 20 A | 2,0% + 2 digit | 10 mA | Tidak dipakai |
| Resistansi | 200 Ω | 1,0% + 2 digit | 0,1 Ω | Kontinuitas, kabel pendek |
| Resistansi | 2 kΩ, 20 kΩ, 200 kΩ, 2 MΩ | 0,8% + 2 digit | 1 Ω; 10 Ω; 100 Ω; 1 kΩ | **Jembatan load cell 350 / 400 Ω (rentang 2 kΩ)**, resistor LED |
| Resistansi | 20 MΩ | 1,2% + 2 digit | 10 kΩ | Resistansi isolasi bila perlu |
| Resistansi | 200 MΩ; 2000 MΩ | 5,0% + 10 digit; 10,0% + 10 digit | 100 kΩ; 1 MΩ | Tidak dipakai |
| Resistansi | Tegangan buka maksimum | 3,2 V | - | Aman untuk jembatan load cell |
| Proteksi | Sekring | **F 200 mA / 250 V (cepat)**; rentang 20 A tanpa sekring | - | Lihat peringatan 1 |
| Proteksi | Lebih beban masukan | 250 V DC atau rms AC (rentang 200 mV dan 1000 V DC: lihat halaman 3 buku) | - | - |
| Layar | Pembaruan | 2 sampai 3 detik | - | Puncak sesaat tidak terbaca |
| Lingkungan | Operasi | 0 sampai 40 derajat C | - | - |

Hal yang **tidak tertulis** di buku bawaan: impedansi masukan tegangan, jatuh tegangan pada rentang arus, batas bunyi kontinuitas, dan mati otomatis. Angka 10 MΩ, 200 mV, 30 ± 10 Ω, dan 15 menit pada dokumen [16] tidak boleh dianggap berlaku untuk alat ini tanpa diuji (lihat peringatan 2 dan 4).

### Perbandingan dengan salinan distributor [16]

| Item | Buku bawaan [15] | Salinan distributor [16] |
|---|---|---|
| DCV 200 mV sampai 200 V | 0,5% + 2 | 0,5% + 1 |
| DCA 2 mA / 20 mA | 1,2% + 2 | 1% + 3 |
| DCA 200 mA | 1,4% + 2 | 1,8% + 3 |
| Resistansi 200 Ω | 1,0% + 2 | 0,8% + 3 |
| Resistansi 2 kΩ sampai 2 MΩ | 0,8% + 2 | 0,8% + 1 (sebagian ambigu) |
| Sekring | 200 mA / 250 V | 0,5 A / 250 V |
| Kondisi akurasi | 18 sampai 28 derajat C, RH 80% | 23 ± 5 derajat C, RH di bawah 75% |

Kemungkinan besar keduanya adalah revisi atau varian yang berbeda dari keluarga model yang sama. Entri ini memakai angka yang lebih longgar dari buku bawaan, jadi ketidakpastian yang dihitung tidak terlalu optimistis.

### Ketidakpastian pada pengukuran yang direncanakan (hitungan kita, dari [15])

| Pengukuran | Rentang | Ketidakpastian | Cara hitung |
|---|---|---|---|
| 3V3 (nilai nominal 3,3 V) | 20 V DC | ± 0,037 V | 0,5% × 3,3 V = 0,0165 V, ditambah 2 digit (0,02 V) |
| 5 V | 20 V DC | ± 0,045 V | 0,5% × 5 V = 0,025 V, ditambah 0,02 V |
| Jembatan load cell sekitar 400 Ω | 2 kΩ | ± 5 Ω | 0,8% × 400 Ω = 3,2 Ω, ditambah 2 digit (2 Ω) |
| Jembatan sekitar 350 Ω | 2 kΩ | ± 5 Ω | 0,8% × 350 Ω = 2,8 Ω, ditambah 2 Ω |
| Arus total sekitar 70 mA | 200 mA DC | ± 1,2 mA | 1,4% × 70 mA = 0,98 mA, ditambah 2 digit (0,2 mA) |
| Arus total sekitar 30 mA | 200 mA DC | ± 0,6 mA | 1,4% × 30 mA = 0,42 mA, ditambah 0,2 mA |

Dengan ketidakpastian 5 Ω, selisih 350 Ω dan 400 Ω jelas terbedakan. Untuk arus 80 MHz lawan 240 MHz, selisih 20 sampai 30 mA lebih besar daripada ketidakpastian, jadi layak dibandingkan.

### Batas dan peringatan

1. **Arus lewat jack "mA"** (bukan "V Ω"). Sekring alat ini **200 mA**: arus di atas itu memutus sekring. Pada pengukuran arus total, jangan menyalakan Wi-Fi (puncak ESP32 bisa melewati 200 mA) dan jangan menghidupkan beban besar. Rentang 20 A tidak berpengaman: **jangan dipakai** untuk papan ini.
2. **Jatuh tegangan saat mengukur arus tidak tertulis di buku bawaan.** Dokumen [16] menyebut 200 mV. Anggap ada penurunan; pasang seri hanya di jalur 5 V dari USB, **bukan** di jalur 3V3 (penurunan dapat membuat HX711 atau ESP32 reset). Bila papan reset saat terukur, ganti dengan USB inline meter.
3. **Layar diperbarui tiap 2 sampai 3 detik.** Arus sesaat tidak terbaca; hanya rata-rata yang tenang.
4. **Mati otomatis** tidak tertulis di buku bawaan. Kalau layar mati sendiri saat pengukuran lama, catat berapa menit.
5. **Tampilan "1"** berarti melebihi rentang; naikkan rentang. Bila tegangan tidak diketahui, mulai dari rentang tertinggi.
6. **Resistansi jembatan diukur tanpa catu.** Lepaskan load cell dari HX711 sebelum mengukur Ω, dan jangan menekan load cell saat mengukur. Buku bawaan: jangan ukur resistansi pada rangkaian bertegangan.
7. Cabut kabel ukur dari rangkaian sebelum memutar saklar rentang atau fungsi. Periksa isolasi kabel ukur dan kontinuitasnya sebelum dipakai.
8. Soket kapasitansi: kapasitor harus sudah dikosongkan, dan adaptor dilepas sebelum ganti fungsi (peringatan buku).

### Langkah pengukuran DS-ESP1 dan DS-LC1 (usulan, belum dijalankan)

| No | Pengukuran | Langkah singkat | Dicatat |
|---|---|---|---|
| 1 | Tegangan 3V3 tanpa beban | USB tercolok; kabel hitam ke GND, merah ke pin 3V3; rentang 20 V DC | Nilai, tanggal |
| 2 | Tegangan 3V3 dengan beban | Ulangi dengan HX711, LCD, dan LED terpasang dan firmware proyek berjalan | Nilai; turun berapa dari no. 1 |
| 3 | Tegangan 5 V di pin VIN | Sama seperti no. 1 | Nilai |
| 4 | Arus total pada 240 MHz dan 80 MHz | Putus jalur 5 V USB, pasang multimeter seri lewat jack mA, rentang 200 mA, Wi-Fi mati | mA pada tiap frekuensi |
| 5 | Resistansi jembatan load cell | Load cell terlepas dari modul; ukur antar kabel (E+ ke E-, A+ ke A-) pada rentang 2 kΩ; catat warna kabel | Ω per pasangan |
| 6 | Kontinuitas perisai (kawat kelima) | Mode kontinuitas antara perisai dan tiap kabel sinyal, dan dengan badan load cell | Bunyi atau tidak |

Hasil pengukuran ditulis di tabel "Daftar cek saat modul tiba" masing-masing entri (DS-ESP1 no. 2 dan 2a, DS-LC1 untuk resistansi jembatan). Hasil di entri itu masih kosong sampai pengukuran dilakukan.

### Daftar pustaka DS-INS1 (IEEE)

[15] "Digital Multimeter Operator's Instruction Manual," buku petunjuk bawaan paket (foto halaman 1 sampai 7); model DT9205A dari kotak kemasan; pabrikan, tanggal, dan revisi tidak tertulis pada halaman yang difoto. Berkas PDF unggahan pengguna, 10 Okt. 2026.

[16] "DT9205A Digital Multimeter," buku petunjuk pengoperasian (kode halaman TOODMM02), salinan dari distributor, mantech.co.za/datasheets/products/DT9205A-190514A.pdf. Pabrikan dan revisi tidak tertulis; berbeda dari [15] pada beberapa angka akurasi dan sekring. Diakses 10 Okt. 2026.


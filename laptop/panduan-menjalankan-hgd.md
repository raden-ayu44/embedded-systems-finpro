---
title: Panduan Menjalankan Purwarupa - Hand Grip Dynamometer

---

# Panduan Menjalankan Purwarupa - Hand Grip Dynamometer (v1)
**Proyek akhir - embedded system**

Dari nol sampai satu sesi tercatat di CSV, di laptop Windows mana pun: pasang perangkat lunak, flash firmware, jalankan `logger.py`, uji bench satu sesi, dan buka hasilnya. Ditulis dari uji bench resmi v1 (pra-kalibrasi) 24 Okt. 2026 dan masalah yang ditemui saat menyiapkannya. Isi teknis firmware dan logger ada di [alur-program-hgd.md](../firmware/alur-program-hgd.md); di sini hanya cara menjalankannya.

## 1. Yang dibutuhkan

| Perangkat lunak | Versi | Catatan |
|---|---|---|
| Arduino IDE | 2.x | untuk build dan flash firmware |
| Paket papan **esp32** (Espressif Systems) | **3.3.11** | build akhir UTS diuji dengan versi ini. **Jangan klik Update** walau versi lebih baru ditawarkan |
| Library **LiquidCrystal_I2C** | **1.1.4** | lewat Library Manager |
| Python 3 + **pyserial** | pyserial **3.5** | untuk `logger.py` (diuji dengan Python 3.11) |
| VS Code (opsional) | | hanya terminalnya yang dipakai; lihat Bagian 3 |

Papan: ESP32 DevKit V1 dengan chip USB **CP210x** (driver Silicon Labs; biasanya terpasang otomatis di Windows 10/11). Kabel USB harus kabel data, bukan kabel pengisi daya saja.

## 2. Python dan pyserial: cek dulu, baru pasang

Satu laptop bisa punya beberapa Python. Yang penting: pyserial terpasang di **Python yang dipakai untuk menjalankan `logger.py`**. Di terminal (PowerShell atau terminal VS Code):

```powershell
python --version
python -c "import sys; print(sys.executable)"
python -c "import serial; print(serial.__version__)"
```

| Hasil `sys.executable` | Arti | Tindakan |
|---|---|---|
| path Python biasa (mis. `...\Python311\python.exe` dari python.org) dan baris ketiga galat `No module named 'serial'` | pyserial belum ada | `python -m pip install pyserial==3.5`, lalu cek lagi baris ketiga: harus `3.5` |
| baris ketiga sudah `3.5` | siap | tidak perlu apa-apa |
| path berisi **`msys64`** (Python dari MSYS2) | Python ini dikelola MSYS2; `pip install` ditolak ("externally-managed-environment") atau mencampur paket ke MSYS2 | **jangan pakai pip di sini.** Pakai Python PlatformIO (di bawah) atau pasang Python dari python.org |
| `python` tidak dikenal | tidak ada Python di PATH | pasang Python dari python.org (centang "Add python.exe to PATH"), atau pakai Python PlatformIO |

**Python PlatformIO.** Bila ekstensi PlatformIO di VS Code pernah dipasang, ia membawa Python sendiri yang sudah berisi pyserial 3.5 (dipakai esptool):
```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\python.exe" -c "import serial; print(serial.__version__)"
```
Bila tercetak `3.5`, Python ini bisa langsung dipakai untuk `logger.py` (Bagian 6) tanpa memasang apa pun.

## 3. VS Code: hanya terminal

- Buka folder repo: **File → Open Folder…**. Terminal: **Terminal → New Terminal** (atau **Ctrl + `**).
- **Jangan klik tombol PlatformIO di bilah biru bawah**: **Upload** (→) membuat build dengan core 2.0.17 (bukan 3.3.11 yang diuji) dan menimpa firmware di papan; **Serial Monitor** (ikon steker) memegang port COM sehingga `logger.py` tidak bisa membukanya. Build (✓) aman tetapi tidak diperlukan.

## 4. Flash firmware (Arduino IDE)

Perlu hanya bila papan belum berisi firmware UTS, atau setelah kode berubah.

1. Tutup `logger.py` (**Ctrl + C**) dan Serial Monitor apa pun: hanya satu program yang bisa memegang port.
2. **File → Open…** → `firmware/hgd/hgd/hgd.ino` di folder repo. Tab `button.cpp`, `config.h`, `fsm.cpp`, ... muncul: semuanya satu program. Bila IDE menawarkan memindahkan berkas ke folder "hgd", pilih **Cancel**: berkas yang dibuka salah.
3. **Tools → Board → esp32 → DOIT ESP32 DEVKIT V1**; **Tools → Port →** port papan (Bagian 5).
4. Cek sekali: **Tools → Board → Boards Manager…**, cari `esp32`: harus **3.3.11 installed**.
5. Klik **Upload** (→). **Tombol BOOT tidak perlu ditekan**: chip USB memasukkan ESP32 ke mode unggah sendiri. Tekan dan tahan BOOT hanya bila macet di `Connecting....____` lebih dari ≈ 5 s, lepas begitu muncul "Connected" atau "Writing at". Jangan tekan EN.
6. **Jangan sentuh papan dan kabel selama unggah.** Uji 24 Okt.: unggah pertama gagal di akhir ("Serial data stream stopped: Possible serial noise or corruption") saat papan disentuh; unggah ulang tanpa disentuh berhasil.
7. **Berhasil** bila bagian terakhir (`hgd.ino.bin` di 0x00010000) diakhiri **"Hash of data verified."** lalu **"Hard resetting via RTS pin..."** tanpa "fatal error". Papan menyala ulang: LED D1 berkedip 5 s, `Menyesuaikan nol`, lalu `SIAP - UKUR 1/3`.
8. Bila gagal lagi: **Tools → Upload Speed → 460800** (atau 115200), coba port USB lain langsung di laptop (tanpa hub), atau kabel lain.

Peringatan `library LiquidCrystal_I2C claims to run on avr architecture(s)` normal dan tidak berpengaruh.

**Pengaturan Tools yang dipakai untuk build UTS** (nilai bawaan papan, tidak perlu diubah; tercatat di `build.options.json` build 24 Okt. 2026): Core Debug Level **None**, Flash Frequency **80 MHz**, Upload Speed **921600**, Erase All Flash Before Upload **Disabled**. Menu "Events Run On" tidak ada untuk papan ini. Ukuran: 313 739 byte flash (23 %), 25 404 byte RAM (7 %); build `arduino-cli` dari salinan folder bersih 313 675 byte dengan pengaturan sama (selisih 64 byte kemungkinan dari path berkas yang ikut dikompilasi; perilaku sama).

## 5. Mencari port COM

- **Device Manager → Ports (COM & LPT)**: "Silicon Labs CP210x USB to UART Bridge (COMx)". Atau di Arduino IDE **Tools → Port**.
- Port bisa berubah setelah papan dicolok ke port USB lain. Pakai nomor yang terlihat di semua perintah di bawah (contoh di sini: `COM3`).

| Galat `logger.py` | Arti | Tindakan |
|---|---|---|
| `could not open port 'COM3': FileNotFoundError` | papan tidak tersambung, atau nomornya lain | colok papan; cek nomor port |
| `could not open port 'COM3': PermissionError` / `Access is denied` | port dipakai program lain (Serial Monitor Arduino atau PlatformIO, `logger.py` lain) | tutup program itu |

## 6. Menjalankan `logger.py`

Dari folder repo (CSV ditulis ke folder `data` di **samping** repo, bukan di dalam repo):
```powershell
python laptop\logger.py COM3 --dir ..\data
```
Dengan Python PlatformIO (Bagian 2):
```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\python.exe" laptop\logger.py COM3 --dir ..\data
```
Tekan **↑** di terminal untuk memanggil perintah yang sama lagi.

- Harus muncul `[logger] port COM3 terbuka (115200 baud), CSV di ...`.
- **Beberapa baris `[INFO] loop terlama ...` sekaligus di awal itu normal**: isi buffer chip USB selama logger tertutup. Setelah habis, satu baris tiap 5 s.
- Tekan **p** (tanpa Enter): harus muncul `[CMD] p: boot=XXXX ... kg=+0.00x state=READY`. Itu bukti tombol diteruskan dan papan menjawab.
- Membuka atau menutup `logger.py` **tidak me-reset** papan. Bila kabel USB tercabut, `logger.py` menunggu dan tersambung lagi sendiri.
- Berhenti: **Ctrl + C**. Baris terakhir harus diakhiri `rekaman hilang: 0 upaya, 0 baris G`.

## 7. Uji bench satu sesi

1. `logger.py` berjalan. Jangan sentuh load cell; tunggu `Tekan tmbl START`.
2. **Percobaan 1:** tekan tombol, lalu tekan load cell **kuat dan stabil** sampai LED D1 menyala dan LCD `Lepas ok`, lalu lepas. Hitung mundur istirahat 60 s berjalan; di `NEXT > lanjut` tekan tombol.
3. **Percobaan 2:** sama.
4. **Percobaan 3:** sama; setelah dilepas **tanpa** istirahat, langsung `NEXT > ringkasan`; tekan tombol.
5. **Ringkasan:** `Mx … Av …` lalu `Data tersimpan` / `Tekan: RESTART`. Tekan tombol sekali: sesi baru.
6. **Ctrl + C**, cek baris terakhir (Bagian 6).

Yang wajar terjadi:
- **Tekanan ringan (≈ 1 kg)** mudah turun di bawah ambang 1 kg dan tercatat `EARLY_RELEASE` (dilepas terlalu awal). Itu tangan, bukan firmware: tekan lebih kuat.
- Kadang 64 byte `\xff` muncul di konsol (gangguan jalur USB) dan satu baris rekaman hilang (`n=59, diterima 58`). Data hasil tetap aman (baris hasil diperiksa checksum dan dikirim ulang).

Hasil uji bench resmi v1 (pra-kalibrasi, 24 Okt. 2026): 3 percobaan 2,89 / 1,76 / 2,75 kg, puncak 2,89, rerata 2,47 kg, semua baris ringkasan ber-ACK, `rekaman hilang: 0`. Nilai gaya memakai skala nominal 53 687 counts/kg (belum terkalibrasi).

## 8. Membuka CSV

Berkas di folder `data` (satu set per hari): `hgd_YYYYMMDD.csv` (hasil T dan ringkasan S), `hgd_galat_YYYYMMDD.csv` (galat), `hgd_genggam_YYYYMMDD.csv` (tiap sampel genggaman + penanda per upaya). Sesi terbaru ada di **akhir** berkas.

- **Hentikan `logger.py` dulu** sebelum membuka di Excel: Excel mengunci berkas, dan logger yang berjalan tidak bisa menulis (uji X14, X16).
- **Lihat saja:** buka di VS Code (**File → Open File…**); tidak mengunci berkas.
- **Excel:** jangan klik dua kali bila Windows memakai pengaturan angka Indonesia (pemisah `;`, desimal `,`): semua kolom masuk satu kolom atau 2.89 terbaca salah. Pakai **Data → From Text/CSV**, delimiter **Comma**, File Origin **65001: Unicode (UTF-8)**; bila angka masih salah: **Transform Data → Change Type → Using Locale… → Decimal, English (United States)**. Simpan salinan sebagai `.xlsx` dengan nama baru; jangan menimpa `.csv`. Tutup Excel sebelum menjalankan logger lagi.
- Berkas genggaman besar: **Data → Filter** pada kolom `jenis` (`G` = sampel, `M` = penanda per upaya).

## 9. Grafik genggaman (`plot_grip.py`, `plot_session.py`)

Sumber: `hgd_genggam_YYYYMMDD.csv`. Butuh **pandas** dan **matplotlib**. Tiap sampel HX711 (≈ 85 ms) digambar sebagai titik; PNG ditulis di samping CSV.

| Skrip | Gambar | Isi |
|---|---|---|
| `plot_grip.py` | tiga, satu per percobaan: `grafik_genggam_<boot>_sesi<n>_p<k>.png` | panel atas: seluruh genggaman, onset, akhir upaya, ambang 1 kg, jendela ref, puncak, dan strip LED D1 (menyala = plateau); panel bawah: perbesaran jendela ref dengan nilai kelima sampel, rentang maks - min terhadap batas 0,5 kg, dan ref = rata-rata |
| `plot_session.py` | dua: `grafik_sesi_<boot>_sesi<n>_berurutan.png` dan `..._bertumpuk.png` | berurutan: tiga genggaman sesuai urutan waktu, jeda istirahat ditulis (jam laptop, ±1 s; tidak sesuai skala); bertumpuk: ketiganya disejajarkan di onset |

Aturan jendela yang digambar sama dengan firmware: jendela = 5 sampel terakhir sesudah onset; plateau = kelima sampel di atas 1 kg dan maks - min ≤ 0,5 kg (LED D1 menyala); ref = rata-rata jendela plateau tertinggi. `plot_grip.py` menghitung ulang jendela ref dari sampel dan memberi peringatan bila berbeda dari catatan firmware. Strip LED dihitung ulang dari CSV (kg dibulatkan 0,01), jadi jendela tepat di batas 0,50 kg bisa berbeda dari LED nyata.

**Google Colab** (pandas dan matplotlib sudah terpasang):
1. Buka notebook baru. Sel 1: tempel seluruh isi `laptop/plot_grip.py`, jalankan (tidak ada keluaran). Bila perlu juga grafik sesi: sel berikutnya, tempel `laptop/plot_session.py`, jalankan.
2. Sel berikutnya:
   ```python
   from google.colab import files
   up = files.upload()                       # pilih hgd_genggam_YYYYMMDD.csv
   csv = list(up)[0]
   pngs = plot_trials(csv)                  # tiga gambar per percobaan (plot_grip.py)
   pngs += plot_overview(csv)                # dua gambar sesi (plot_session.py)
   for p in pngs: files.download(p)
   ```
   Sesi tertentu: `plot_trials(csv, boot="5068", sesi=3)`, sama untuk `plot_overview`.

**Laptop** (Python dengan pandas dan matplotlib; Python PlatformIO tidak membawanya):
```powershell
python laptop\plot_grip.py ..\data\hgd_genggam_20261025.csv --boot 5068 --sesi 3
python laptop\plot_session.py ..\data\hgd_genggam_20261025.csv --boot 5068 --sesi 3
```

- Tanpa boot/sesi: dipakai sesi lengkap (tiga percobaan dengan ref) yang paling akhir di berkas. Boot dan sesi tiap baris ada di kolom `boot` dan `sesi`.
- Tiap percobaan memakai upaya terakhirnya yang punya ref; upaya yang berakhir galat atau dilepas terlalu awal tidak digambar.
- Grafik dimulai 1 s sebelum onset; masa menunggu sebelumnya datar di 0.

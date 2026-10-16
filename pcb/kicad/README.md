# KiCad: simbol dan footprint kustom (v1)
**Proyek akhir - embedded system**

Folder ini adalah pusat semua berkas yang berkaitan dengan KiCad untuk PCB hand grip dynamometer. Daftar komponen, nama part resmi, dan nama simbol/footprint tiap komponen ada di [../pcb_netlist.xlsx](../pcb_netlist.xlsx) (sheet Komponen dan Modul luar PCB); desain dan keputusannya di [../desain-pcb-hgd.md](../desain-pcb-hgd.md).

## Struktur folder

Folder dibuat saat berkasnya ada; yang belum ada ditandai "belum".

| Jalur | Isi | Status |
|---|---|---|
| `README.md` | Hub ini: daftar simbol dan footprint kustom, aturan penamaan, kaitan ke berkas lain | Ada |
| `simbol/hgd.kicad_sym` | Pustaka simbol kustom proyek | Belum |
| `footprint/hgd.pretty/` | Pustaka footprint kustom proyek (satu berkas `.kicad_mod` per footprint) | Belum |
| `3d/` | Model 3D (`.step`) yang dipakai footprint kustom | Belum |
| `proyek/` | Berkas proyek KiCad (`.kicad_pro`, `.kicad_sch`, `.kicad_pcb`) | `hgd-v1.kicad_pcb` ada (16 Okt. 2026): penempatan final v1, belum ada jalur; `.kicad_pro` dan `.kicad_sch` belum diunggah |
| `pdf/` | Keluaran PDF: skema, penempatan, lembar uji pas skala 1:1 | Belum |
| `gerber/` | Keluaran produksi (Gerber, bor) | Belum |

Pustaka standar KiCad (Device, Connector_Generic, Switch, LED_THT, dan lainnya) dipakai apa adanya dan tidak disalin ke sini.

## Aturan penamaan

- Pustaka kustom bernama `HGD`; simbol dipanggil `HGD:NamaSimbol`, footprint `HGD:NamaFootprint`.
- Nama simbol dan footprint mengikuti nama part resmi di datasheet bila ada (sama persis dengan kolom "Nama part resmi" di netlist).
- Tiap simbol dan footprint kustom diberi kolom `Datasheet` yang menunjuk ke berkas di `datasheet/pdf/` atau kode DS-* di `datasheet/datasheet-library.md`.

## Simbol dan footprint kustom yang direncanakan

Semuanya opsional: PCB v1 memakai soket dan header standar (J1 sampai J4), jadi papan dan modul tidak wajib punya simbol sendiri. Dibuat bila ingin skema yang menampilkan pin modul dengan nama aslinya.

| Nama | Jenis | Untuk | Keterangan | Sumber ukuran | Status |
|---|---|---|---|---|---|
| `HGD:ESP32_DevKit_V1_30pin` | Simbol | ESP32 DevKit V1 | Dua baris 15 pin, nama pin sesuai cetakan papan (EN, D18, D19, D21, D22, D25, D27, VIN, 3V3, GND, dan lainnya) | `datasheet/datasheet-library.md` (DS-ESP2, posisi pin) | Belum |
| `HGD:ESP32_DevKit_V1_Outline` | Footprint | Kontur papan di PCB pembawa | 51,5 x 28,2 mm, dua baris soket 15 pin jarak 25,4 mm (hasil ukur 25,3), area antena bebas tembaga | Tab ESP32 di [../../pengukuran/tabel-pengukuran-hgd.xlsx](../../pengukuran/tabel-pengukuran-hgd.xlsx) | Belum |
| `HGD:HX711_Module_XFW` | Simbol | Modul HX711 | Sisi sinyal GND, DT, SCK, VCC; sisi load cell E+, E-, A-, A+, B-, B+ | DS-HX1 | Belum |
| `HGD:LCD1602_I2C_Backpack` | Simbol | LCD dengan backpack | GND, VCC, SDA, SCL | DS-LCD1, DS-I2C1 | Belum |

## Pemeriksaan sebelum memakai nama dari netlist

- Nama pustaka standar di `pcb_netlist.xlsx` ditulis dari pustaka KiCad yang umum; cocokkan dengan yang benar-benar ada di instalasi Anda (nama bisa berbeda antar versi KiCad).
- Skema KiCad masih perlu perbaikan (simbol modul, USB tak tersambung, kapasitor, PWR_FLAG, 3V3 ke VCC modul); lihat [../desain-pcb-hgd.md](../desain-pcb-hgd.md).

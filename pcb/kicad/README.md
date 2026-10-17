# KiCad: simbol dan footprint kustom (v5)
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
| `proyek/` | Berkas proyek KiCad (`.kicad_pro`, `.kicad_sch`, `.kicad_pcb`) | Sebagian: `hgd-v1.kicad_pcb` (17 Okt. 2026: D2/R2, pour GND dua lapis, 57 via jahit, silkscreen tim; model 3D DevKit tertanam di berkas, `kicad-embed://`, tanpa jalur lokal); `hgd-v1.kicad_sch` (17 Okt. 2026: D2/R2, 24 no-connect, `PWR_FLAG`, title block; ERC 0 error); `hgd-v1-verbatim.kicad_pcb` (salinan khusus 3D: papan yang sama + dua soket 1x15 `REF**` di pad U2 supaya soket DevKit tampak di model 3D; **jangan dipakai untuk Gerber**, karena lubangnya ganda); `hgd-v1.step` (model 3D rakitan dari salinan verbatim, untuk Fusion); `.kicad_pro` belum |
| `pdf/` | Keluaran PDF: skema, penempatan, lembar uji pas skala 1:1 | Belum |
| `gerber/` | Keluaran produksi (Gerber, bor) | Belum |

Pustaka standar KiCad (Device, Connector_Generic, Switch, LED_THT, dan lainnya) dipakai apa adanya dan tidak disalin ke sini.

## Aturan penamaan

- Pustaka kustom bernama `HGD`; simbol dipanggil `HGD:NamaSimbol`, footprint `HGD:NamaFootprint`.
- Nama simbol dan footprint mengikuti nama part resmi di datasheet bila ada (sama persis dengan kolom "Nama part resmi" di netlist).
- Tiap simbol dan footprint kustom diberi kolom `Datasheet` yang menunjuk ke berkas di `datasheet/pdf/` atau kode DS-* di `datasheet/datasheet-library.md`.

## Simbol dan footprint kustom yang direncanakan

Proyek KiCad `hgd-v1` memakai simbol dan footprint DevKit dari SnapEDA ("ESP32-DEVKIT-V1" oleh "Do it"; diunduh dari snapeda.com), sedangkan modul lain diwakili soket standar J1-J4. Simbol kustom di bawah tetap opsional.

| Nama | Jenis | Untuk | Keterangan | Sumber ukuran | Status |
|---|---|---|---|---|---|
| `HGD:ESP32_DevKit_V1_30pin` | Simbol | ESP32 DevKit V1 | Dua baris 15 pin, nama pin sesuai cetakan papan (EN, D18, D19, D21, D22, D25, D27, VIN, 3V3, GND, dan lainnya) | `datasheet/datasheet-library.md` (DS-ESP2, posisi pin) | Tidak perlu: dipakai simbol SnapEDA `ESP32-DEVKIT-V1` (U2) |
| `HGD:ESP32_DevKit_V1_Outline` | Footprint | Kontur papan di PCB pembawa | 51,5 x 28,2 mm, dua baris soket 15 pin jarak 25,4 mm (hasil ukur 25,3), area antena bebas tembaga | Tab ESP32 di [../../pengukuran/tabel-pengukuran-hgd.xlsx](../../pengukuran/tabel-pengukuran-hgd.xlsx) | Tidak perlu: dipakai footprint SnapEDA `MODULE_ESP32_DEVKIT_V1` (diperiksa: pitch 2,54, baris 25,4, bor 1,02, pad 1 = 3V3, pad 16 = EN; lubang NPTH bawaan dihapus) |
| `HGD:HX711_Module_XFW` | Simbol | Modul HX711 | Sisi sinyal GND, DT, SCK, VCC; sisi load cell E+, E-, A-, A+, B-, B+ | DS-HX1 | Belum |
| `HGD:LCD1602_I2C_Backpack` | Simbol | LCD dengan backpack | GND, VCC, SDA, SCL | DS-LCD1, DS-I2C1 | Belum |

## Pemeriksaan sebelum memakai nama dari netlist

- Nama pustaka standar di `pcb_netlist.xlsx` ditulis dari pustaka KiCad yang umum; cocokkan dengan yang benar-benar ada di instalasi Anda (nama bisa berbeda antar versi KiCad).
- Skema KiCad `hgd-v1` sudah diperiksa (15 Okt. 2026): netlist benar, termasuk kedua GND DevKit. Pembersihan berkas PCB sebelum gerber ada di Bagian 8.3 [../desain-pcb-hgd.md](../desain-pcb-hgd.md).
- **Skema dan PCB v1 selesai (17 Okt. 2026), belum disetujui.** Di `proyek/`: `hgd-v1.kicad_sch` (ERC 0 error), `hgd-v1.kicad_pcb` (DRC 0 error; paritas skema-PCB 0 beda), `hgd-v1-verbatim.kicad_pcb` (hanya untuk model 3D), `hgd-v1.step`. Gerber v1 diekspor dan dicek, disimpan di luar repo sampai disetujui. Langkah kerja: [../log-kerja-kicad-hgd.md](../log-kerja-kicad-hgd.md).

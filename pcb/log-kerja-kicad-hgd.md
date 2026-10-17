---
title: Log Kerja KiCad - PCB hgd-v1

---

# Log Kerja KiCad - PCB hgd-v1 (v1)
**Proyek akhir - embedded system**

> Catatan langkah kerja KiCad yang dikerjakan penulis sendiri, dengan bukti hasil tiap langkah. Kolom "Pemeriksaan" berisi hasil cek oleh alat AI (Claude) terhadap berkas atau tangkapan layar; pekerjaan di KiCad seluruhnya oleh penulis. Desain: [desain-pcb-hgd.md](desain-pcb-hgd.md); berkas: [kicad/proyek/](kicad/proyek/); induk: [catatan-proyek-hgd.md](../catatan-proyek-hgd.md).

---

| # | Tanggal | Langkah | Yang dikerjakan | Hasil | Pemeriksaan |
|---|---|---|---|---|---|
| 1 | 17 Okt. 2026 | Assign Footprints (Tools > Assign Footprints) | 14 simbol diberi footprint: C1, C4 `C_Disc_D7.0mm_W2.5mm_P5.00mm`; C2, C3 `CP_Radial_D5.0mm_P2.50mm`; D1 `LED_D5.0mm`; D2 `LED_D3.0mm`; J1, J2 `PinSocket_1x04_P2.54mm_Vertical`; J3, J4 `PinSocket_1x06_P2.54mm_Vertical`; R1, R2 `R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal`; SW1 `SW_PUSH_6mm_H13mm`; U2 `ESP32-DEVKIT-V1:MODULE_ESP32_DEVKIT_V1` (SnapEDA) | Semua simbol punya footprint | Cocok dengan berkas papan. BOM dari KiCad tidak memuat 2 soket betina 1x15 untuk DevKit dan modul yang dicolok (HX711, LCD + backpack, level shifter, load cell); tambahkan manual di BOM |
| 2 | 17 Okt. 2026 | ERC (Inspect > Electrical Rules Checker) | 24 tanda no-connect pada pin tak terpakai (20 pin U2; J3 dan J4 pin 5-6); `PWR_FLAG` di GND; 7 peringatan `lib_symbol_mismatch` dikecualikan | **0 error, 0 warning**, 7 pengecualian (sebelumnya 28 error, 7 warning) | Pengecualian beralasan: simbol J1-J4, C1, C4, SW1 sudah diubah (nama pin); memperbarui dari pustaka akan menghapus nama pin itu |
| 3 | 17 Okt. 2026 | Page Settings skema (File > Page Settings) | Title "Hand Grip Dynamometer", Company "Kelompok 01", Revision 1, Issue Date 2026-10-17 | Title block terisi; berkas skema di repo diperbarui | Tidak ada nama orang dan jalur lokal di berkas skema |
| 4 | 17 Okt. 2026 | Simpan skema ke repo | Berkas `hgd-v1.kicad_sch` hasil langkah 1-3 disimpan dan diserahkan untuk repo | [kicad/proyek/hgd-v1.kicad_sch](kicad/proyek/hgd-v1.kicad_sch) di PR #5 (commit oleh Claude) | Berkas sama dengan yang diserahkan; tanpa jalur lokal |
| 5 | 17 Okt. 2026 | Plot Gerber dan bor (File > Fabrication Outputs) | **Plot:** format Gerber, folder `gerber\`; lapis F.Cu, B.Cu, F.Silkscreen, B.Silkscreen, F.Mask, B.Mask, Edge.Cuts; opsi umum: Subtract soldermask from silkscreen, Indicate DNP (cross-out), Check zone fills before plotting, tanpa drawing sheet, skala 1:1; opsi Gerber: Use Protel filename extensions, Generate Gerber job file, format koordinat 4.6 (mm), Use extended X2 format, Include netlist attributes. **Bor:** Excellon, metrik (mm), desimal, absolut; PTH dan NPTH di berkas terpisah; peta bor PDF | 12 berkas (19:44-19:45): 7 Gerber, job file (Revision 1), `PTH.drl`, `NPTH.drl`, 2 peta bor PDF; DRC 0 pelanggaran, 4 pengecualian | Outline 80 x 55 mm; 129 PTH (70 kaki komponen, 59 via), 4 NPTH Ø3,0; pour GND di kedua lapis, area antena kosong; silkscreen tim ada; tanpa jalur lokal. Belum dicek di viewer fabrikator |
| 6 | 17 Okt. 2026 | Cek di viewer fabrikator (JLCPCB, juga ekspor SVG viewer PCBWay) | ZIP Gerber diunggah ke penawaran daring JLCPCB (tanpa memesan); pratinjau atas dan bawah diperiksa; SVG lapis dari viewer PCBWay diunduh | JLCPCB: "Detected 2 layer board of 55x80mm", dimensi 80 x 55, 2 lapis, FR-4. Pratinjau sama dengan papan: outline persegi, H1-H4, baris DevKit dan J1-J4, pour GND kedua sisi dengan jalur antena kosong, silkscreen tim dan D2 PWR terbaca | Pratinjau cocok dengan berkas papan; viewBox SVG PCBWay 80 x 55 mm pada koordinat papan yang sama |
| 7 | 17 Okt. 2026 | Title block papan, salinan 3D, dan ekspor STEP | Page Settings papan diisi (Title, Company, Revision 1, tanggal); salinan `hgd-v1-verbatim.kicad_pcb` dengan dua soket 1x15 di pad U2; STEP diekspor dari salinan itu (File > Export > STEP) | `hgd-v1.kicad_pcb`, `hgd-v1-verbatim.kicad_pcb`, `hgd-v1.step` di [kicad/proyek/](kicad/proyek/) | Berkas papan utama hanya berbeda di title block dari versi sebelumnya; tanpa jalur lokal di ketiga berkas. Salinan verbatim punya lubang ganda di pad U2, jadi Gerber tetap dari `hgd-v1.kicad_pcb` |

---

## Perubahan

| Versi | Tanggal | Perubahan |
|---|---|---|
| v1 | 17 Okt. 2026 | Log pertama: langkah 1-7 |

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

---

## Perubahan

| Versi | Tanggal | Perubahan |
|---|---|---|
| v1 | 17 Okt. 2026 | Log pertama: langkah 1-3 |

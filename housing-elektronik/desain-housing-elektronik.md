---
title: Desain Housing Elektronik - Hand Grip Dynamometer

---

# Desain Housing Elektronik - Hand Grip Dynamometer (v1)
**Proyek akhir - embedded system**

> Rencana housing elektronik (belum dimodelkan). Housing grip (load cell) ada di [desain-housing-load-cell.md](../housing-load-cell/desain-housing-load-cell.md); ukuran komponen diambil dari [tabel-pengukuran-hgd.xlsx](../pengukuran/tabel-pengukuran-hgd.xlsx); induk: [catatan-proyek-hgd.md](../catatan-proyek-hgd.md).

---

## 1. Housing elektronik ▣
Wadah cetak 3D dua bagian (body dan lid) dimodelkan di perangkat lunak CAD. Memuat PCB, ESP32, modul HX711; LCD dipasang di lid; tombol dan LED di lid. Cutout samping untuk kabel USB dan kabel load cell. Kabel load cell pendek (≈28 cm), jadi housing elektronik diletakkan dalam jangkauan kabel; kabel sambungan di luar lingkup dokumen housing.
Ukuran ruang diambil dari tabel ukur di [tabel-pengukuran-hgd.xlsx](../pengukuran/tabel-pengukuran-hgd.xlsx): DevKit V1 51,5 x 28,2 mm, tinggi total 12,4 mm dengan pin.

---

## 2. Belum dirancang (housing grip)
- **Stop overload mekanis.** Pada 70 kgf pelat melentur ≈ 0,24 mm ke arah load cell; fitur penahan beban lebih belum didesain. Pada laporan dinyatakan sebagai batasan.
- **Penjepit A ke meja untuk kalibrasi** (memakai sisa batang ≈ 11,5 mm di ujung); sementara alas karet menahan A.

---
title: Teks LCD - Hand Grip Dynamometer

---

# Teks LCD - Hand Grip Dynamometer (v1)
**Proyek akhir - embedded system**

Tabel semua layar LCD 16x2 persis seperti yang dicetak firmware (cabang `feat/logger`, langkah 10), untuk ditinjau dan didiskusikan. Rujukan utama tetap [alur-program-hgd.md](alur-program-hgd.md) Bagian 10.2; bila satu teks diubah, kode (`fsm.cpp` `drawScreen()`, `fault.cpp` `faultDraw()`), Bagian 10.2, dan berkas ini diubah bersama.

Nama state mengikuti dokumen alur program (Bagian 1.4a): nama dokumen, lalu nama di kode dalam kurung.

**Aturan untuk teks baru:**
- Paling banyak 16 karakter per baris (LCD hanya menampilkan 16; kelebihannya terpotong).
- Hanya ASCII biasa: tanpa huruf beraksen, tanpa simbol derajat; hindari `\` dan `~` (di LCD ini tampil sebagai simbol lain).
- Angka ditulis dengan lebar tetap (`%5.2f` = 5 karakter, mis. `25.34`, ` 2.53`) supaya teks tidak bergeser saat nilai berubah. Semua nilai kg di LCD dua desimal (uji papan langkah 10).
- Baris 2 lebih sering berubah daripada baris 1; baris yang tidak berubah tidak dikirim ulang.

## Layar per state

| State | Kapan | Baris 1 | Baris 2 | Karakter (baris 1 / baris 2) | Format di kode |
|---|---|---|---|---|---|
| SIAP (`READY`) | Tare berjalan (sekitar 1 s setelah masuk SIAP atau setelah TAHAN) | `SIAP - UKUR 1/3` | `Menyesuaikan nol` | 15 / 16 | `"SIAP - UKUR %u/%u"` |
| SIAP (`READY`) | Tare selesai, alat tidak ditekan | `SIAP - UKUR 1/3` | `Tekan tmbl START` | 15 / 16 | |
| SIAP (`READY`) | Tare selesai, alat masih ditekan (> 1 kg), START diabaikan | `SIAP - UKUR 1/3` | `Lepas genggaman` | 15 / 15 | |
| SIAP (`READY`) | Tare diulang karena nol di luar jendela (tangan bertumpu di load cell) | `SIAP - UKUR 1/3` | `Nol ulang: lepas` | 15 / 16 | |
| SIAP (`READY`) | Skala demo `x` aktif (menggantikan baris 1 di semua kasus SIAP) | `SIAP x40 1/3` | (sama seperti di atas) | 12 / — | `"SIAP x%.0f %u/%u"` |
| GENGGAM (`GRIP`) fase 1 | Menunggu genggaman (di bawah 1 kg) | `Menunggu genggam` | ` 0.42kg >1.00kg` | 16 / 15 | `"%5.2fkg >%.2fkg"` |
| GENGGAM (`GRIP`) fase 1 | Tare ulang setelah TAHAN 1,5 s | `Menunggu genggam` | `Menyesuaikan nol` | 16 / 16 | |
| GENGGAM (`GRIP`) fase 1 | Tare ulang diulang lagi, nol di luar jendela | `Menunggu genggam` | `Nol ulang: lepas` | 16 / 16 | |
| GENGGAM (`GRIP`) fase 2 | Mengukur, sebelum 3 s atau belum stabil | `Tahan 3-5 detik` | `25.34 kg  3.4s` | 15 / 14 | `"%5.2f kg  %.1fs"` |
| GENGGAM (`GRIP`) fase 2 | Sejak 3 s dan stabil (LED menyala): boleh dilepas | `Tahan 3-5 detik` | `25.34kg Lepas ok` | 15 / 16 | `"%5.2fkg Lepas ok"` |
| HITUNG (`COMPUTE`) | One-shot, tanpa layar sendiri (layar sebelumnya tetap beberapa ms) | — | — | — | |
| RESPON_ISTIRAHAT (`REST`) | Hasil, genggaman belum dilepas | `Hasil 47.23 kg` | `Lepas>Rest 60s` | 14 / 14 | `"Hasil %5.2f kg"`, `"Lepas>Rest %2lus"` |
| RESPON_ISTIRAHAT (`REST`) | Sudah dilepas: hitung mundur istirahat (angka berhenti bila digenggam lagi) | `Hasil 47.23 kg` | `Lepas>Rest 58s` | 14 / 14 | |
| RESPON_ISTIRAHAT (`REST`) | Istirahat selesai, percobaan 1 atau 2 | `Hasil 47.23 kg` | `NEXT > lanjut` | 14 / 13 | |
| RESPON_ISTIRAHAT (`REST`) | Percobaan 3, genggaman belum dilepas (tanpa istirahat setelah percobaan 3) | `Hasil 46.90 kg` | `Lepas>ringkasan` | 14 / 15 | |
| RESPON_ISTIRAHAT (`REST`) | Percobaan 3, langsung setelah dilepas | `Hasil 46.90 kg` | `NEXT > ringkasan` | 14 / 16 | |
| RINGKASAN (`SUMMARY`) | Mengirim 4 baris log, menunggu ACK (normal < 0,1 s, sekitar 1 s tanpa `logger.py`) | `Mx47.23 Av44.81` | `Menyimpan...` | 15 / 12 | `"Mx%5.2f Av%5.2f"` (tanpa "kg": dengan "kg" 18 karakter) |
| RINGKASAN (`SUMMARY`) | Semua baris tersimpan: baris 2 bergantian tiap 2 s (`SUMMARY_PAGE_MS`), mulai dari `Data tersimpan` | `Mx47.23 Av44.81` | `Data tersimpan` / `Tekan: RESTART` | 15 / 14, 14 | |
| ERROR (`FAULT`) | HX711 tidak menjawab (tanpa sampel selama 1 s) | `ERR:HX711 TMOUT` | `RAW -38650` | 15 / sampai 12 | `"RAW %ld"` (live) |
| ERROR (`FAULT`) | ADC mendekati skala penuh (termasuk simulasi X2, `s`) | `ERR:ADC SATURASI` | `RAW 8388607` | 16 / sampai 12 | |
| ERROR (`FAULT`) | Di luar rentang (> 80 kg live, atau hasil <= 0 / > 80 kg) | `ERR:DI LUAR RNTG` | `RAW 70713` | 16 / sampai 12 | |
| ERROR (`FAULT`) | Pembacaan tidak valid (SCK tercabut, nilai identik, atau nol di luar jendela selama 10 s) | `ERR:BACA TDK VLD` | `RAW 0` | 16 / sampai 12 | |
| GAGALSIMPAN (`SAVE_FAILED`) | Halaman 1 (3 halaman, masing-masing 3 s) | `P1 47.23 kg` | `P2 43.10 kg` | 11 / 11 | `"P1 %5.2f kg"` |
| GAGALSIMPAN (`SAVE_FAILED`) | Halaman 2 | `P3 44.10 kg` | `GAGAL SIMPAN` | 11 / 12 | `"P3 %5.2f kg"` |
| GAGALSIMPAN (`SAVE_FAILED`) | Halaman 3 (`Mx` = label puncak yang sama dengan RINGKASAN) | `Mx47.23 Av44.81` | `GAGAL SIMPAN` | 15 / 12 | `"Mx%5.2f Av%5.2f"` |
| State apa pun | Tombol ditahan 3-15 s: hitung mundur reset total (menutupi layar state) | `Reset total 12` | `Lepas = batal` | 14 / 13 | `"Reset total %lu"` |

## Catatan

- Langkah 10: dua keadaan SIAP kini punya teks berbeda: `Lepas genggaman` = alat masih ditekan setelah tare; `Nol ulang: lepas` = tare diulang karena nol di luar jendela (juga di GENGGAM fase 1).
- Perbaikan uji papan langkah 10 (24 Okt. 2026): semua nilai kg dua desimal; target onset di GENGGAM fase 1 `>1.00kg` (dengan satuan, gaya sama dengan `Lepas ok`); RINGKASAN tanpa "kg" supaya muat; RINGKASAN setelah tersimpan bergantian `Data tersimpan` / `Tekan: RESTART` (tanpa penundaan buatan, TAP langsung berlaku); GAGALSIMPAN tiga halaman, bukan dua; puncak diberi label `Mx` di semua layar (tidak ada lagi `Pk`).
- Tanpa istirahat setelah percobaan 3 (24 Okt. 2026): `Lepas>ringkasan` (gaya sama dengan `Lepas>Rest`) sampai dilepas, lalu langsung `NEXT > ringkasan`. `Lepas genggaman` tidak dipakai karena di SIAP artinya "alat masih ditekan".
- Teks `ERR:...` ringkas untuk pengawas, bukan peserta; nilai `RAW` live membantu pengawas melihat kapan kabel sudah diperbaiki.
- GAGALSIMPAN menampilkan dua desimal, nilai yang sama dengan serial (`[GAGAL SIMPAN] ... P1=3.15 ...`).
- Arti LED (tidak tampil di LCD): menyala = genggaman stabil; tetap menyala setelah keluar 5 s dalam keadaan stabil sampai dilepas; kedip lambat 5 s setelah tiap restart; dipaksa mati di ERROR dan GAGALSIMPAN.

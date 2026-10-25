# Hand grip dynamometer

Design and documentation for a hand grip strength meter, made for an embedded system course project. Two aluminium plates are squeezed, a load cell between them measures the force, an HX711 digitizes it, and an ESP32 shows the result on a 16×2 I2C LCD and logs it over USB serial.

- Target range: 20 to 70 kgf.
- Parts: ESP32 DevKit V1, HX711 module, CZL 601 80 kg bending-beam load cell, 16×2 I2C LCD (5 V) with a BSS138 4-channel level shifter, push button, LED. Powered from USB only.
- Housing: two plain Al 6061-T6 plates (170 × 30 × 10 mm) with PLA spacers (28 × 28 × 2.5 mm), joined to the load cell with M6 × 25 bolts. The cable clamp is printed as one piece with spacer A.
- Budget: maximum Rp300.000.

**Status (24 Oct. 2026):** breadboard prototype working.
- **Firmware:** steps 0-11 written and tested on the board (state machine, non-blocking HX711 reading, LED rule, fault handling, watchdog, logging with ACK), built with Arduino IDE core 3.3.11. **Frozen for the UTS**: bug fixes only.
- **Laptop logger:** `laptop/logger.py` works: it writes three daily CSV files (results, faults, grip traces), answers ACK/NAK, and reconnects after a USB unplug.
- **Hardware design:** KiCad schematic and PCB v1 done; electronics housing and grip housing designed (not yet built). Bench tests: ESP32 pins, load cell, LCD (I2C 0x27), HX711; unloaded noise σ ≈ 0.7 g.
- **Not yet done:** calibration (moved to the final project; force values use the nominal scale, uncalibrated), building the housings.

Documents are written in Indonesian.

## Mulai di sini untuk UTS

Urutan baca untuk memahami purwarupa UTS (firmware, logger, layar), dari nol:

1. `README.md` (berkas ini): gambaran dan status.
2. [`catatan-proyek-hgd.md`](catatan-proyek-hgd.md): **hanya** gambaran proyek (Bagian 1), tujuan dan spesifikasi target S1-S5 (Bagian 2), fungsi perangkat (Bagian 6), riwayat keputusan (Bagian 14), dan status (Bagian 15). Bagian lain (sumber, anggaran, risiko, audit) tidak diperlukan untuk UTS.
3. [`firmware/alur-program-hgd.md`](firmware/alur-program-hgd.md) **Bagian 1-13**: arsitektur, struktur data, konstanta, fungsi, state machine, HX711, logging, antarmuka, uji, build, memori. Bagian 15-17 adalah lampiran lama dari catatan proyek; Bagian 6 dan 10 yang berlaku bila berbeda.
4. [`firmware/teks-lcd-hgd.md`](firmware/teks-lcd-hgd.md): semua layar LCD persis seperti dicetak firmware.
5. Kode firmware di [`firmware/hgd/hgd/`](firmware/hgd/hgd/) (mulai dari `hgd.ino`, lalu `config.h`, `types.h`, `fsm.cpp`). Folder ini juga isi ZIP firmware UTS.
6. [`laptop/logger.py`](laptop/logger.py): pencatat di laptop (CSV, ACK/NAK).
7. [`laptop/panduan-menjalankan-hgd.md`](laptop/panduan-menjalankan-hgd.md): cara menjalankan dari nol di laptop mana pun (pasang, flash, logger, uji bench satu sesi, buka CSV, grafik genggaman).

**Tidak diperlukan untuk UTS:** `housing-load-cell/`, `housing-elektronik/`, `kalibrasi/` (kalibrasi dipindah ke proyek akhir), `sitasi/`, PDF datasheet di `datasheet/pdf/`, dan sketsa uji lama di `firmware/sketches/` (hanya untuk uji modul awal).

## Start here

[`catatan-proyek-hgd.md`](catatan-proyek-hgd.md) is the main project notes and the index to everything else: requirements, specification, BOM, risks, schedule, decisions and changelog.

## Folders

| Folder | Contents |
|---|---|
| [`firmware/`](firmware/) | `hgd/`: the main ESP32 firmware (PlatformIO project; the sketch folder `hgd/hgd/` also opens in Arduino IDE and is the firmware ZIP); `alur-program-hgd.md`: program flow for the ESP32 firmware and the laptop-side logger (data, constants, functions, state machine, logging, tests, build); `teks-lcd-hgd.md`: every LCD screen exactly as the firmware prints it; `protokol-uji-bench-hgd.md`: bench test procedure and results; `sketches/` holds the early test sketches (pin loopback, LCD, HX711, button and LED, and the combined bench test) |
| [`laptop/`](laptop/) | `logger.py`: laptop-side logger (Python 3 + pyserial): reads the ESP32 log lines over USB serial, writes the daily CSV files, replies ACK/NAK; `plot_grip.py` and `plot_session.py`: force-time graphs from the grip CSV, one per trial with the ref window zoomed, and the whole session in sequence and overlapped (pandas + matplotlib, laptop or Google Colab); `panduan-menjalankan-hgd.md`: step-by-step guide to install, flash, run the logger, do a one-session bench test and open the CSV files on any Windows laptop |
| [`pcb/`](pcb/) | `desain-pcb-hgd.md` (schematic and PCB design), `pcb_netlist.xlsx` (components with official part names, KiCad symbols and footprints, netlist), `gambar/` (placement drawing, SVG), and [`kicad/`](pcb/kicad/README.md), the hub for KiCad files |
| [`housing-load-cell/`](housing-load-cell/) | Grip housing design, assumptions (`asumsi.md`), drawings in `gambar/` (SVG), Fusion 360 parameters and instructions in `fusion/` |
| [`housing-elektronik/`](housing-elektronik/) | Electronics housing design (LCD on the lid, shrink compensation, print rules) and Fusion 360 parameters in `fusion/` |
| [`kalibrasi/`](kalibrasi/) | Calibration protocol, `hgd_perhitungan.xlsx` (mechanical and electrical calculations with formulas), dumbbell dimensions, calibration drawings in `gambar/` |
| [`pengukuran/`](pengukuran/) | `tabel-pengukuran-hgd.xlsx`: caliper measurements, one tab per component, with official numbers where a datasheet gives them; `e1-noise-hgd.xlsx`: load cell noise test data; `hasil-multimeter-hgd.md`: every multimeter reading in one place, by date; `protokol-arus-rail-hgd.md`: how to measure the current on each rail with one multimeter (results in `pcb/desain-pcb-hgd.md` §5.1), with the breadboard wiring drawing `gambar/wiring-breadboard-hgd.png` (Wokwi, picture only; source `wokwi/diagram.json`) |
| [`datasheet/`](datasheet/) | Datasheet notes and the checklist of values still to fill in; the datasheet PDFs are in `pdf/` |
| [`sitasi/`](sitasi/) | Book and paper notes, local citation codes and the IEEE reference list; the open-access papers are in `pdf/` |

Drawings are SVG and open in any browser.

## License

Shared as-is for reference. Rights to the datasheets and papers in the `pdf/` folders stay with their publishers and manufacturers.

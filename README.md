# Hand grip dynamometer

Design and documentation for a hand grip strength meter, made for an embedded system course project. The load cell's fixed end is clamped to a table and the user presses its free end with the palm, through a printed PLA spacer; an HX711 digitizes the force, and an ESP32 shows the result on a 16×2 I2C LCD and logs it over USB serial.

- Target range (spec S1): 20 to 70 kgf. In the palm-press setup the load is limited to 40 kg, so the PLA spacer under the fixed end keeps a safety factor of about 3 (see the calibration protocol).
- Parts: ESP32 DevKit V1, HX711 module, CZL 601 80 kg bending-beam load cell, 16×2 I2C LCD (5 V) with a BSS138 4-channel level shifter, push button, LED. Powered from USB only.
- Grip housing (since 18 Nov. 2026, no plates): three PLA parts printed at 100 % infill. Spacer A (14 mm) sits under the fixed end and holds the cable clamp (notch + zip tie); Spacer B (14 mm) sits on the free end, with two ridges for the calibration sling, and takes the palm; Clamp_Pad (3 mm) sits under the C-clamp screw. Spacers are joined to the load cell with M6 × 25 stainless flange bolts, no washers.
- Budget: maximum Rp300.000.

**Status (21 Nov. 2026):** breadboard prototype working; the mid-term (UTS) stage is finished (firmware, brief and video submitted 27 Oct. 2026). Now in the final-project stage: PCB being printed at the fab, housings, calibration and the planned firmware changes.
- **Firmware:** steps 0-11 written and tested on the board (state machine, non-blocking HX711 reading, LED rule, fault handling, watchdog, logging with ACK), built with Arduino IDE core 3.3.11. This is the version submitted for the UTS; outdated comments were cleaned up on 12 Nov. 2026 (no code change). The changes planned for the final project (calibration, version number, countdown timing, debug switch, LCD text redesign) are listed in `firmware/alur-program-hgd.md` section 19 and are not coded yet.
- **Laptop logger:** `laptop/logger.py` works: it writes three daily CSV files (results, faults, grip traces), answers ACK/NAK, and reconnects after a USB unplug.
- **Hardware design:** KiCad schematic and PCB v1 approved; Gerber Revision 2 (12 Nov. 2026) sent for fabrication and the board is being printed (21 Nov. 2026); the same ZIP is in `pcb/kicad/gerber/`. Push button from lab stock; female header sockets wait for the board and a lab stock check. Electronics housing modelled in Fusion 360 (STEP and PDF drawings, 16 Nov. 2026). Grip housing for the palm press modelled in Fusion 360 (Spacer A with cable clamp, Spacer B with ridges, Clamp_Pad; STEP checked for sizes and clashes, 20 Nov. 2026). Calibration protocol v2: 0–6 kg with a dumbbell bag hung from Spacer B, plus a bathroom-scale check of the palm press up to 40 kg. Neither housing is printed yet. Bench tests: ESP32 pins, load cell, LCD (I2C 0x27), HX711; unloaded noise σ ≈ 0.7 g.
- **Not yet done:** printing the grip parts and the electronics housing, assembling the PCB when it arrives, calibration (force values still use the nominal scale, uncalibrated), the final-project firmware changes.

If two documents disagree, the current status is in `catatan-proyek-hgd.md` section 15 and the newest dated entry wins.

Documents are written in Indonesian.

## Mulai di sini: firmware dan purwarupa

Urutan baca untuk memahami purwarupa (firmware, logger, layar), dari nol. Firmware di repo sama dengan yang dikumpulkan untuk UTS (hanya komentar dirapikan 12 Nov. 2026); rencana perubahan untuk proyek akhir ada di Bagian 19 alur program.

1. `README.md` (berkas ini): gambaran dan status.
2. [`catatan-proyek-hgd.md`](catatan-proyek-hgd.md): **hanya** gambaran proyek (Bagian 1), tujuan dan spesifikasi target S1-S5 (Bagian 2), fungsi perangkat (Bagian 6), riwayat keputusan (Bagian 14), dan status (Bagian 15). Bagian lain (sumber, anggaran, risiko, audit) tidak diperlukan untuk memahami firmware.
3. [`firmware/alur-program-hgd.md`](firmware/alur-program-hgd.md) **Bagian 1-13 dan 19**: arsitektur, struktur data, konstanta, fungsi, state machine, HX711, logging, antarmuka, uji, build, memori; Bagian 19 = rencana perubahan firmware untuk proyek akhir. Bagian 15-17 adalah lampiran lama dari catatan proyek; Bagian 6 dan 10 yang berlaku bila berbeda.
4. [`firmware/teks-lcd-hgd.md`](firmware/teks-lcd-hgd.md): semua layar LCD persis seperti dicetak firmware.
5. Kode firmware di [`firmware/hgd/hgd/`](firmware/hgd/hgd/) (mulai dari `hgd.ino`, lalu `config.h`, `types.h`, `fsm.cpp`). Folder ini juga isi ZIP firmware yang dikumpulkan untuk UTS.
6. [`laptop/logger.py`](laptop/logger.py): pencatat di laptop (CSV, ACK/NAK).
7. [`laptop/panduan-menjalankan-hgd.md`](laptop/panduan-menjalankan-hgd.md): cara menjalankan dari nol di laptop mana pun (pasang, flash, logger, uji bench satu sesi, buka CSV, grafik genggaman).

**Tidak diperlukan untuk memahami firmware:** `housing-load-cell/`, `housing-elektronik/`, `kalibrasi/` (kalibrasi dikerjakan di proyek akhir setelah housing grip dirakit), `sitasi/`, PDF datasheet di `datasheet/pdf/`, dan sketsa uji lama di `firmware/sketches/` (hanya untuk uji modul awal).

## Start here

[`catatan-proyek-hgd.md`](catatan-proyek-hgd.md) is the main project notes and the index to everything else: requirements, specification, BOM, risks, schedule, decisions and changelog.

## Folders

| Folder | Contents |
|---|---|
| [`firmware/`](firmware/) | `hgd/`: the main ESP32 firmware (PlatformIO project; the sketch folder `hgd/hgd/` also opens in Arduino IDE and is the firmware ZIP); `alur-program-hgd.md`: program flow for the ESP32 firmware and the laptop-side logger (data, constants, functions, state machine, logging, tests, build); `teks-lcd-hgd.md`: every LCD screen exactly as the firmware prints it; `protokol-uji-bench-hgd.md`: bench test procedure and results; `sketches/` holds the early test sketches (pin loopback, LCD, HX711, button and LED, and the combined bench test) |
| [`laptop/`](laptop/) | `logger.py`: laptop-side logger (Python 3 + pyserial): reads the ESP32 log lines over USB serial, writes the daily CSV files, replies ACK/NAK; `plot_grip.py` and `plot_session.py`: force-time graphs from the grip CSV, one per trial with the ref window zoomed, and the whole session in sequence and overlapped (pandas + matplotlib, laptop or Google Colab); `panduan-menjalankan-hgd.md`: step-by-step guide to install, flash, run the logger, do a one-session bench test and open the CSV files on any Windows laptop |
| [`pcb/`](pcb/) | `desain-pcb-hgd.md` (schematic and PCB design), `pcb_netlist.xlsx` (components with official part names, KiCad symbols and footprints, netlist), `gambar/` (placement drawing, SVG), and [`kicad/`](pcb/kicad/README.md), the hub for KiCad files (project, 3D model `proyek/hgd-v1.step`, schematic PDF, Gerber ZIP in `gerber/`) |
| [`housing-load-cell/`](housing-load-cell/) | Grip housing design and assumptions (`asumsi.md`); in `fusion/`: Fusion 360 parameters, work instructions (v13) and the checked palm-press model `load_cell_housing.step` (Spacer A with cable clamp, Spacer B, Clamp_Pad). The SVG drawings in `gambar/` and `generate_drawings.py` belong to the earlier plate design (history) |
| [`housing-elektronik/`](housing-elektronik/) | Electronics housing design (LCD on the lid, shrink compensation, print rules), Fusion 360 parameters, instructions and the model `electronics_housing_lipped.step` in `fusion/`, drawings in `gambar/` (SVG, plus the Fusion PDF drawing and exploded view) |
| [`kalibrasi/`](kalibrasi/) | Calibration protocol v2 (palm press, 0–6 kg bag calibration, force and pressure calculation), `hgd_perhitungan.xlsx` (mechanical and electrical calculations with formulas), dumbbell dimensions, calibration pictures and the free-body diagram in `gambar/` (PNG) |
| [`pengukuran/`](pengukuran/) | `tabel-pengukuran-hgd.xlsx`: caliper measurements, one tab per component, with official numbers where a datasheet gives them; `e1-noise-hgd.xlsx`: load cell noise test data; `hasil-multimeter-hgd.md`: every multimeter reading in one place, by date; `protokol-arus-rail-hgd.md`: how to measure the current on each rail with one multimeter (results in `pcb/desain-pcb-hgd.md` §5.1), with the breadboard wiring drawing `gambar/wiring-breadboard-hgd.png` (Wokwi, picture only; source `wokwi/diagram.json`) |
| [`datasheet/`](datasheet/) | Datasheet notes and the checklist of values still to fill in; the datasheet PDFs are in `pdf/` |
| [`sitasi/`](sitasi/) | Book and paper notes, local citation codes and the IEEE reference list; the open-access papers are in `pdf/` |

Drawings are SVG or PNG and open in any browser.

## License

Shared as-is for reference. Rights to the datasheets and papers in the `pdf/` folders stay with their publishers and manufacturers.

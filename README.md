# Hand grip dynamometer

Design and documentation for a hand grip strength meter, made for an embedded system course project. Two aluminium plates are squeezed, a load cell between them measures the force, an HX711 digitizes it, and an ESP32 shows the result on a 16×2 I2C LCD and logs it over USB serial.

- Target range: 20 to 70 kgf.
- Parts: ESP32 DevKit V1, HX711 module, CZL 601 80 kg bending-beam load cell, 16×2 I2C LCD (5 V) with a BSS138 4-channel level shifter, push button, LED. Powered from USB only.
- Housing: two plain Al 6061-T6 plates (170 × 30 × 10 mm) with PLA spacers (28 × 28 × 2.5 mm), joined to the load cell with M6 × 25 bolts. The cable clamp is printed as one piece with spacer A.
- Budget: maximum Rp300.000.

**Status:** design and breadboard stage. The ESP32 pins, the load cell (resistance and dimensions), the LCD (I2C address 0x27) and the HX711 have been tested on the bench; the unloaded noise test gave σ ≈ 0.7 g. The KiCad schematic is checked and the PCB placement is a proposal; the housing and calibration are not yet built or tested. Documents are written in Indonesian.

## Start here

[`catatan-proyek-hgd.md`](catatan-proyek-hgd.md) is the main project notes and the index to everything else: requirements, specification, BOM, risks, schedule, decisions and changelog.

## Folders

| Folder | Contents |
|---|---|
| [`firmware/`](firmware/) | `alur-program-hgd.md`: program flow for the ESP32 firmware and the laptop-side logger (data, constants, functions, state machine, logging); `protokol-uji-bench-hgd.md`: bench test procedure and results; `sketches/` holds the test sketches (pin loopback, LCD, HX711, button and LED, and the combined bench test) |
| [`pcb/`](pcb/) | `desain-pcb-hgd.md` (schematic and PCB design), `pcb_netlist.xlsx` (components with official part names, KiCad symbols and footprints, netlist), `gambar/` (placement drawing, SVG), and [`kicad/`](pcb/kicad/README.md), the hub for KiCad files |
| [`housing-load-cell/`](housing-load-cell/) | Grip housing design, assumptions (`asumsi.md`), drawings in `gambar/` (SVG), Fusion 360 parameters and instructions in `fusion/` |
| [`housing-elektronik/`](housing-elektronik/) | Electronics housing design (LCD on the lid, shrink compensation, print rules) and Fusion 360 parameters in `fusion/` |
| [`kalibrasi/`](kalibrasi/) | Calibration protocol, `hgd_perhitungan.xlsx` (mechanical and electrical calculations with formulas), dumbbell dimensions, calibration drawings in `gambar/` |
| [`pengukuran/`](pengukuran/) | `tabel-pengukuran-hgd.xlsx`: caliper measurements, one tab per component, with official numbers where a datasheet gives them |
| [`datasheet/`](datasheet/) | Datasheet notes and the checklist of values still to fill in; the datasheet PDFs are in `pdf/` |
| [`sitasi/`](sitasi/) | Book and paper notes, local citation codes and the IEEE reference list; the open-access papers are in `pdf/` |

Drawings are SVG and open in any browser.

## License

Shared as-is for reference. Rights to the datasheets and papers in the `pdf/` folders stay with their publishers and manufacturers.

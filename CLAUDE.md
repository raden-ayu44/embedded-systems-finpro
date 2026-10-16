# Notes for Claude sessions

Read this first. Claude has no memory between sessions; this file is the handover log. Docs are in Indonesian; the owner writes to Claude in English. Main index: `catatan-proyek-hgd.md`.

## Ground rules learned so far

- **Check `main` before assuming anything is open.** Other sessions (the owner's claude.ai Project) update the repo too. On 16 Oct. 2026 Claude asked whether the LCD backpack SDA/SCL had been measured; it had (4.93 V, recorded on `main` as U5). Search the docs (and `git log origin/main`) before asking the owner.
- The owner's claude.ai Project files are not reachable from a cloud session; only this repo and files uploaded in the chat are.
- The owner keeps placement decisions tied to the Fusion model. Example: the PCB mounting holes (Ø3.0, 75 x 50 mm) line up with the LCD PCB holes; do not suggest moving them.
- Do not duplicate files that already exist under another name (e.g. bench sketch `firmware/sketches/uji_bench_hgd/`).

## Log

### 15 Oct. 2026
- Bench test on breadboard (`uji_bench_hgd`): LCD at 0x27, HX711 reads the load cell, button TAP/HOLD correct.
- Run 1 had a loose DT/SCK jumper: one window showed `win_avg=+23.837 spread=78.850` while every printed value was under 1 kg. Reconstructed as unprinted samples of about 79 kg and 40 kg (top 2-3 bits read as 0). Cause recorded on `main`: loose jumpers; fixed in hardware by plugging the HX711 straight into J1.
- E1 (cold): n = 706 in 60 s, sigma 38.2 counts (about 0.7 g), so HX711 runs at about 11.8 SPS, not 10.

### 16 Oct. 2026
- Owner uploaded the final PCB placement: `pcb/kicad/proyek/hgd-v1.kicad_pcb` (no traces yet). Board 80 x 55 mm; holes NPTH Ø3.0 at (112.25; 95.5), (187.25; 95.5), (112.25; 145.5), (187.25; 145.5); U2 on the bottom at (136.25; 127.5) rotation -90; antenna keepout x 155.2-170.3, y 103.7-148. Netlist checked: identical to `desain-pcb-hgd.md` 8.2.
- Open PCB checks (recorded in `desain-pcb-hgd.md` 8.4 and 10): J3-J4 spacing is 10.04 mm in the file (measure the shifter; likely 10.16); direction the HX711 module hangs from J1 (inward it sits over C1/C2 and the shifter, outward it sticks out about 20 mm); nearest hole to SW1 is about 23 mm; label J1/J2 pin names on B.Silkscreen (wrong cable into J2 puts 5 V on HX711 DT); screw M2.5 fits Ø3.0, M3 needs Ø3.2 with the same centres.
- Firmware design review findings B1-B5 added to `firmware/alur-program-hgd.md` 18.1 (await team decision): release between 3-5 s without plateau gives a fake result; stored value is plateau mean, not peak; single-sample spike guard; send each `T` line right after its trial; logic tests before the housing via a simulation mode.
- Session-history question closed as option (c), matching alur 6.2.
- Docs reconciled with `main` in one PR (see `catatan-proyek-hgd.md` 16, entry v46).

## Still waiting on the owner or the team

- Level shifter arrival: measure row spacing and LV voltage (DS-LLS1).
- Decisions on B1-B5.
- `.kicad_sch` and `.kicad_pro` not in the repo yet.
- U6 (HX711 E+/E-), U8 full 10/10, calibration, housing.

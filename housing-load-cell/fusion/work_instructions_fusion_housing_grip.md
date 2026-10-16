# Work instructions: grip housing in Fusion (v5 layout, CZL 601 load cell)

> **Outdated (12 Oct 2026):** this file still follows the v5 layout (total thickness 52 mm, no cable clamp, cable hole on the end face). The current values are in `fusion_parameters_housing_grip.csv` and `../asumsi.md`; drawings in `../gambar/`. Update before modelling.

Scope: the load cell grip housing only (two bars, load cell, ridges, calibration platform, cable clamp). The electronics housing is separate. All values marked "working" or "TBD" are provisional; the model is built so they can be changed in one place.

Inputs you need open: `potongan_memanjang_v5_logam.svg`, `exploded_isometrik_v5_logam.svg`, `../asumsi.md` (also summarised as Appendix A of `../desain-housing-load-cell.md`), `fusion_parameters_housing_grip.csv`, and `../desain-housing-load-cell.md` (and `catatan-proyek-hgd.md` for the caliper table).

## Layout reference (all numbers come from parameters)

Directions: length axis = along the load cell (fixed end on the left, free end on the right); width axis = across the bars (30 mm); stack axis = A at the top, B at the bottom. In Fusion, use whichever origin plane contains length and width, and extrude along the stack axis.

Stack levels, with the bottom face of the load cell at 0:

| Part | Stack range |
|---|---|
| Bar A (palm side, fixed) | `LC_H + gap` to `LC_H + gap + bar_t` |
| Spacer A (PLA, under bar A, left end) | `LC_H` to `LC_H + gap` |
| Load cell | 0 to `LC_H` |
| Spacer B (PLA, above bar B, right end) | `-gap` to 0 |
| Bar B (finger side, moving) | `-gap - bar_t` to `-gap` |
| Ridges (PLA, outside bar B) | below bar B, height `ridge_h` |

Length positions. The model is built with the load cell centred on the origin (as in the actual Fusion file), so the length coordinate is 0 at the middle of the cell, negative toward the fixed (left) end and positive toward the free (right) end:
- Load cell: `-LC_L / 2` to `LC_L / 2`.
- Both bars: `-bar_L / 2` to `bar_L / 2` (centred on the cell, 20 mm of spare bar on each end).
- Spacer A: centre at `spA_c` = -(`LC_L` / 2) + (`boss_L` / 2) = -51 mm, so it spans -65 to -37 mm. Spacer B: centre at `spB_c` = (`LC_L` / 2) - (`boss_L` / 2) = +51 mm, spanning +37 to +65 mm. Both are `boss_L` long and `LC_W` wide.
- Grip zone centre: `LC_L / 2`. This is 0 on the length axis, and `lever_arm` (37 mm) from the inner edge of each spacer.

Total thickness: `2*bar_t + 2*gap + LC_H + ridge_h` = 52 mm.

## Phase 1: setup

- [ ] Create a new design and save it with a clear name. If this part is also submitted for the 3D printing module, use the module's naming format.
- [ ] Check the document units are mm (Browser, Document Settings; menu names can vary by version).
- [ ] Open Change Parameters and use Import Parameters with `fusion_parameters_housing_grip.csv`.
- [ ] Check the Value column: `total_t` reads 52 mm, `gap` reads 2.5 mm, `boss_L` reads 28 mm, `lever_arm` reads 37 mm, `spA_c` reads -51 mm. If decimals look wrong, retype them.
- [ ] Do not model holes yet. The hole parameters are placeholders until the load cell is measured.

## Phase 2: reference load cell

- [ ] New component `LoadCell_ref`. Activate it before sketching.
- [ ] Sketch a rectangle on the horizontal plane (`LC_L` by `LC_W`), centred on the width axis, and extrude `LC_H`. Type parameter names in the dimension boxes, not numbers.
- [ ] Ground it (right-click in the Browser).
- [ ] Mark it as reference only (do not export it for printing).
- [ ] Add a note in the component description: orientation of the load arrow is unconfirmed (C5).

## Phase 3: Bar A and Bar B (plain plates)

For each bar, create a new component and activate it first.

- [ ] Create offset construction planes at the stack levels in the table above, using parameter expressions (for example `LC_H + gap`).
- [ ] Bar A plate: rectangle `bar_L` by `bar_W`, centred on the cell, extruded `bar_t` from the Bar A level.
- [ ] Bar A is just the plate (no boss). The 2.5 mm gap is made by the PLA spacers in Phase 4.
- [ ] Bar B: the same plate on the lower level.
- [ ] Assign aluminium to both bars (material is only used for appearance and the mass check).
- [ ] Mass check (right-click the component, Properties): each bar should read about 138 g (plate only: 170 x 30 x 10 mm = 51 cm3 at 2.7 g/cm3). A big difference means a wrong parameter.

## Phase 4: PLA parts (spacers and rough shapes)

- [ ] Add the user parameters `spA_c` (`-( LC_L / 2 ) + ( boss_L / 2 )`) and `spB_c` (`( LC_L / 2 ) - ( boss_L / 2 )`), unit mm. They should read -51.00 and 51.00.
- [ ] `Spacer_A` and `Spacer_B` (one component each): centre rectangle `boss_L` by `LC_W` positioned at `spA_c` / `spB_c` along the length and centred on the width axis; thickness `gap`, between the plate and the load cell at each end (A on the left, above the cell; B on the right, below it). Assign PLA for appearance. Holes come later, after the load cell is measured.
- [ ] Ridges on the outer face of bar B: two ridges around the grip zone centre, `ridge_h` tall, with about 22 mm of flat platform between them (the calibration platform). Width and exact shape are placeholders.
- [ ] Cable clamp: a small PLA block on bar A in the 0 to 20 mm spare end zone on the cable side (not over the load cell). The cable is assumed to be 5 mm in diameter (not measured; the distributor writes 4 mm), which is larger than the 2.5 mm gap, so the cable must not pass through the gap. Clamp hole about the cable diameter, outer size about 7.4 to 9 mm, bend radius TBD. Placeholder size only.
- [ ] Put these in their own component(s) so they can be exported and printed separately.

## Phase 5: checks

- [ ] Inspect > Interference: select all bodies. Expected result: no interference (the spacers touch the plate and load cell faces, which is not an overlap).
- [ ] Inspect > Measure: gap between bar A and the load cell at the free end is 2.5 mm; gap between bar B and the load cell at the fixed end is 2.5 mm; overall thickness matches `total_t`.
- [ ] Inspect > Section Analysis through the length: compare with the cross-section drawing (A on top, B at the bottom, spacers on opposite ends).
- [ ] Parameter test: change `gap` to 3 mm, confirm everything updates and `total_t` reads 53 mm, then change it back to 2.5.
- [ ] Save with a version description.

What this does not check: whether the bars bend into the load cell under load. Rely on `../desain-housing-load-cell.md`, HL9 (aluminium 10 mm with 28 mm spacers leaves at least 2.26 mm of clearance before the load cell flexing and cover height are known).

## Phase 6: while you wait for the load cell

- [ ] Build the cardboard mock-up at about 52 mm total thickness and check hand comfort.
- [ ] Confirm with the TA that a hybrid grip (metal plates plus printed parts) is acceptable.
- [ ] The plate thickness is decided (10 mm). Plain plates can be ordered; hold the drilling until the holes are measured. Keywords and the seller checklist are in `../desain-housing-load-cell.md`, HL11.
- [ ] Optional: print a 30 x 7 mm PLA strip and do the cantilever test in 14.9 to check the beam model.

## Phase 7: when the load cell arrives

Measure with the caliper (Section 15 table in `catatan-proyek-hgd.md`, rows C1 to C8) and write the results down before touching the model:
- [ ] Hole diameter, pitch along the length and across the width, and distance from the end of the body (C2 to C4). The vendor drawing gives 12 / 106 / 15 mm; confirm.
- [ ] Load cell width (28 or 30 mm) and the length of the end blocks (about 27 to 28 mm is only an estimate).
- [ ] Cable diameter (5 mm is an assumption) and cable length (about 28 cm in the listing, 0.42 m in the GJ Impex datasheet).
- [ ] Whether the holes are through or blind (C6). If blind, the bolting scheme changes; stop and review.
- [ ] Which side is along the load (arrow, C5) and which end the cable leaves (C7).
- [ ] Height of the white strain-gauge cover above the metal (and whether the other side has one).
- [ ] Bolt thread size.

Then:
- [ ] Update the parameters (replace the TBD rows; split `hole_pitch` into length and width pitch if they differ).
- [ ] Check that the 28 mm spacer covers the hole pattern (use the `spA_c` / `spB_c` positions). If it does not, change `boss_L` and recompute the lever arm.
- [ ] Add holes (plain through holes in the metal plates, no counterbore; M6 x 25 bolts with washers) only after this.
- [ ] Send a short correction file with the measured values to Cowork so it redraws the schematics (it should not receive the CSV).
- [ ] Send the plate drawings to the workshop or drilling service.

## Printing rules to remember (from the 3D printing module)

- PLA only; slot booked in advance; print only after the slice review is approved.
- Clearance 0.2 to 0.3 mm per side; M3 through-hole modelled at 3.4 mm; wall at least 1.2 mm (use about 2 mm where loaded); overhang at most 45 degrees; chamfer 0.3 to 0.5 mm on bottom edges.
- Print the spacers flat (28 x 28 mm face down) so no support is needed. Print a small test coupon (spacer and hole pattern) before the full part.
- The metal bars are not printed. Only the spacers, ridges, platform, cable clamp, and the electronics housing are.

## Decision gates

1. Before ordering plates: cardboard comfort check at about 52 mm and the TA answer on the hybrid grip (thickness 10 mm is decided).
2. Before drilling: all hole measurements recorded and through/blind known.
3. Before printing: slice review approved.
4. Before calibration: follow the dumbbell protocol (`../../kalibrasi/protokol-kalibrasi-hgd.md`, up to 42 kg; 42 to 70 kgf is extrapolation) and decide how plate A is supported, because the bolt heads and washers on its outer face stick out (`../desain-housing-load-cell.md`, HL10).

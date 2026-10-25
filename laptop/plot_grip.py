"""Grafik gaya genggam (HGS) per percobaan dari hgd_genggam_YYYYMMDD.csv.

Satu gambar per percobaan (tiga gambar per sesi), dua panel:
- atas: seluruh genggaman, tiap sampel HX711 sebagai titik; onset, akhir upaya, ambang 1 kg,
  jendela ref, dan puncak; di bawahnya strip keadaan LED D1 (menyala = plateau), dihitung ulang
  dari sampel CSV;
- bawah: perbesaran jendela ref: kelima sampelnya dengan nilainya, rentang maks-min terhadap
  batas plateau 0,5 kg, dan ref = rata-rata kelima sampel.

Aturan yang digambar sama dengan firmware (fsm.cpp, handleGrip; config.h):
- jendela = 5 sampel terakhir sesudah onset (sampel onset sendiri tidak ikut);
- plateau = kelima sampel di atas 1 kg DAN maks - min <= 0,5 kg (LED D1 menyala);
- ref = rata-rata jendela plateau tertinggi; bila tidak pernah ada plateau, jendela tertinggi.
Skrip menghitung ulang jendela ref dari sampel dan memberi peringatan bila hasilnya berbeda
dari jendela yang dicatat firmware (penanda M).

Cara pakai di laptop (butuh pandas dan matplotlib):
    python laptop/plot_grip.py ..\\data\\hgd_genggam_20261025.csv
    python laptop/plot_grip.py ..\\data\\hgd_genggam_20261025.csv --boot 5068 --sesi 3

Cara pakai di Google Colab (pandas dan matplotlib sudah ada):
    1. Sel 1: tempel seluruh isi berkas ini, jalankan (tidak ada keluaran).
    2. Sel 2:
           from google.colab import files
           up = files.upload()                    # pilih hgd_genggam_YYYYMMDD.csv
           pngs = plot_trials(list(up)[0])       # atau plot_trials(..., boot="5068", sesi=3)
           for p in pngs: files.download(p)

Tanpa boot/sesi: dipakai sesi lengkap (tiga percobaan dengan ref) yang paling akhir di berkas.
Nilai kg memakai skala firmware; sebelum kalibrasi itu skala nominal.
"""

import argparse
import os
import sys

import matplotlib
matplotlib.use("Agg")  # tanpa jendela: langsung ke berkas PNG (juga aman di Colab)
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter, MultipleLocator
import pandas as pd

# Sama dengan config.h firmware.
ONSET_KG = 1.0      # THRESHOLD_ONSET_KG
STABLE_KG = 0.5     # THRESHOLD_STABLE_KG: batas maks - min jendela plateau
BUF_SIZE = 5        # sampel per jendela

PRE_ONSET_S = 1.0   # detik sebelum onset yang ditampilkan

# Warna: data hitam; jendela ref biru dan puncak oranye (slot 1 dan 2 palet kategorikal
# tervalidasi); daerah LED menyala biru sangat muda. Teks selalu hitam/abu, bukan warna seri.
INK = "#0b0b0b"
INK_2 = "#52514e"
TRACE = "#3d3d3a"
WIN = "#2a78d6"
PEAK = "#eb6834"
LED_ON = "#1baf7a"   # slot 3 (aqua): strip LED D1
WIN_FILL = "#b7d3f6"
GRID_MAJOR = "#cfcec9"
GRID_MINOR = "#ebeae6"


def comma(x, _pos=None):
    """Angka sumbu dengan koma desimal (gaya Indonesia)."""
    return f"{x:g}".replace(".", ",")


def kg2(x):
    """Dua desimal dengan koma, mis. 3,70."""
    return f"{x:.2f}".replace(".", ",")


def load(path):
    # boot dibaca sebagai teks: "5068" dan "29B9" sama-sama ID heksadesimal.
    df = pd.read_csv(path, dtype={"boot": str})
    need = {"boot", "sesi", "percobaan", "upaya", "jenis", "ms", "t_onset_s", "kg", "fase", "ref_kg"}
    if not need <= set(df.columns):
        raise SystemExit(f"{os.path.basename(path)} bukan berkas hgd_genggam_YYYYMMDD.csv "
                         "(kolom boot/sesi/percobaan/... tidak ada).")
    df = df[df["sesi"].notna()].copy()
    df["sesi"] = df["sesi"].astype(int)
    return df


def pick_trials(df, boot, sesi):
    """Penanda M terakhir yang punya ref untuk tiap percobaan di sesi ini."""
    m = df[(df["jenis"] == "M") & (df["boot"] == boot) & (df["sesi"] == sesi) & df["ref_kg"].notna()]
    m = m.sort_values("upaya")
    return {int(p): row for p, row in m.groupby("percobaan").tail(1).set_index("percobaan").iterrows()}


def latest_complete(df):
    """(boot, sesi) terakhir di berkas yang ketiga percobaannya punya ref."""
    m = df[(df["jenis"] == "M") & df["ref_kg"].notna()]
    for (boot, sesi), g in reversed(list(m.groupby(["boot", "sesi"], sort=False))):
        if {1, 2, 3} <= set(g["percobaan"].astype(int)):
            return boot, int(sesi)
    raise SystemExit("Tidak ada sesi dengan tiga percobaan lengkap di berkas ini.")


def windows(g2):
    """Hitung ulang semua jendela 5 sampel fase 2, seperti handleGrip() di firmware.

    g2: sampel fase 2 urut waktu. Mengembalikan daftar dict per sampel ke-i (i >= 4):
    indeks awal, akhir, rata-rata, rentang, dan apakah plateau.
    """
    kg = g2["kg"].tolist()
    out = []
    for i in range(BUF_SIZE - 1, len(kg)):
        w = kg[i - BUF_SIZE + 1:i + 1]
        above = min(w) > ONSET_KG
        spread = max(w) - min(w)
        out.append({"i0": i - BUF_SIZE + 1, "i1": i, "avg": sum(w) / BUF_SIZE, "spread": spread,
                    # 1e-9: 3,16 - 2,66 di Python = 0,5000000000000004, padahal maksudnya tepat 0,50
                    "grip": above, "plateau": above and spread <= STABLE_KG + 1e-9})
    return out


def best_window(ws):
    """Jendela ref menurut firmware: plateau tertinggi, atau jendela tertinggi bila tak ada plateau."""
    plat = [w for w in ws if w["plateau"]]
    pool, kind = (plat, "plateau") if plat else ([w for w in ws if w["grip"]], "tanpa plateau")
    if not pool:
        return None, kind
    best = pool[0]
    for w in pool[1:]:
        if w["avg"] > best["avg"]:  # ">" seperti firmware: bila sama, jendela pertama tetap
            best = w
    return best, kind


def style_axes(ax, xmajor, xminor, ymajor, yminor):
    for side in ("top", "right", "bottom", "left"):
        ax.spines[side].set_visible(True)
        ax.spines[side].set_color(INK)
        ax.spines[side].set_linewidth(0.9)
    ax.xaxis.set_major_locator(MultipleLocator(xmajor))
    ax.xaxis.set_minor_locator(MultipleLocator(xminor))
    ax.yaxis.set_major_locator(MultipleLocator(ymajor))
    ax.yaxis.set_minor_locator(MultipleLocator(yminor))
    ax.xaxis.set_major_formatter(FuncFormatter(comma))
    ax.yaxis.set_major_formatter(FuncFormatter(comma))
    ax.tick_params(which="both", direction="in", top=True, right=True, colors=INK, labelsize=10.5)
    ax.tick_params(which="major", length=6, width=0.9)
    ax.tick_params(which="minor", length=3, width=0.7)
    ax.grid(which="major", color=GRID_MAJOR, linewidth=0.8, zorder=0)
    ax.grid(which="minor", color=GRID_MINOR, linewidth=0.5, zorder=0)
    ax.set_axisbelow(True)


def plot_trial(df, boot, sesi, p, mk, out):
    g = df[(df["jenis"] == "G") & (df["boot"] == boot) & (df["sesi"] == sesi)
           & (df["percobaan"] == p) & (df["upaya"] == mk["upaya"])].sort_values("ms")
    onset = float(mk["onset_ms"])
    end_s = (float(mk["akhir_ms"]) - onset) / 1000
    ref, peak = float(mk["ref_kg"]), float(mk["puncak_kg"])
    w0 = (float(mk["jendela_awal_ms"]) - onset) / 1000
    w1 = (float(mk["jendela_akhir_ms"]) - onset) / 1000
    t = (g["ms"] - onset) / 1000

    g2 = g[g["fase"] == 2].reset_index(drop=True)
    t2 = (g2["ms"] - onset) / 1000
    ws = windows(g2)
    best, kind = best_window(ws)

    # Cek: jendela hitung ulang = jendela yang dicatat firmware?
    note = ""
    if best is not None:
        bw0, bw1 = float(t2[best["i0"]]), float(t2[best["i1"]])
        if abs(bw0 - w0) > 0.001 or abs(bw1 - w1) > 0.001 or abs(best["avg"] - ref) > 0.006:
            note = "Peringatan: jendela hitung ulang berbeda dari catatan firmware (kg di CSV dibulatkan 0,01)."
            print(f"[plot] percobaan {p}: {note}")

    # Sampel di dalam jendela ref yang dicatat.
    in_win = (t >= w0 - 1e-6) & (t <= w1 + 1e-6)
    wk = g.loc[in_win, "kg"].tolist()
    wt = t[in_win].tolist()
    spread = (max(wk) - min(wk)) if wk else float("nan")

    fig = plt.figure(figsize=(10, 10), dpi=200)
    fig.patch.set_facecolor("white")
    outer = fig.add_gridspec(2, 1, height_ratios=[3.6, 2.2], hspace=0.32)
    top = outer[0].subgridspec(2, 1, height_ratios=[3, 0.42], hspace=0.06)
    ax = fig.add_subplot(top[0])
    al = fig.add_subplot(top[1], sharex=ax)
    az = fig.add_subplot(outer[1])

    # ---------- Panel atas: seluruh genggaman ----------
    # LED D1 menyala: dari sampel saat plateau terpenuhi sampai sampel berikutnya.
    led = []  # (awal, akhir) tiap selang LED menyala: dari sampel plateau sampai sampel berikutnya
    for w in ws:
        if w["plateau"]:
            a = float(t2[w["i1"]])
            b = float(t2[w["i1"] + 1]) if w["i1"] + 1 < len(t2) else end_s
            led.append((a, b))
    ax.axvspan(w0, w1, color=WIN_FILL, alpha=0.7, lw=0, zorder=0.6, label="jendela ref (5 sampel)")
    ax.axhline(ONSET_KG, color=INK_2, lw=1, ls=(0, (5, 3)), zorder=1, label="ambang onset 1 kg")
    ax.axvline(0, color=INK, lw=0.9, ls=(0, (1, 2)), zorder=1)
    ax.axvline(end_s, color=INK, lw=0.9, ls=(0, (1, 2)), zorder=1)
    ax.plot(t, g["kg"], color=TRACE, lw=1.1, zorder=2)
    ax.plot(t, g["kg"], "o", ms=3.2, color=TRACE, zorder=3, label="sampel HX711")
    ax.plot(wt, wk, "o", ms=5.5, color=WIN, mec="white", mew=0.8, zorder=4)
    ax.plot([w0, w1], [ref, ref], color=WIN, lw=2, zorder=4, label=f"ref = {kg2(ref)} kg")
    ip = g["kg"].idxmax()
    ax.plot([(g.loc[ip, "ms"] - onset) / 1000], [g.loc[ip, "kg"]], "^", ms=8, color=PEAK,
            mec="white", mew=0.8, zorder=5, label=f"puncak = {kg2(peak)} kg")

    ymax = max(g["kg"].max(), ref) + 0.5
    ax.set_xlim(-PRE_ONSET_S, max(end_s, t.max()) + 0.15)
    ax.set_ylim(min(-0.25, g["kg"].min() - 0.1), ymax)
    style_axes(ax, 0.5, 0.1, 0.5, 0.1)
    lbl = {"fontsize": 9.5, "color": INK, "va": "top",
           "bbox": {"facecolor": "white", "edgecolor": "none", "pad": 1.5}}
    ax.text(0.03, ymax - 0.08, "onset (t = 0)", ha="left", **lbl)
    ax.text(end_s - 0.03, ymax - 0.08, f"akhir upaya ({mk['keluar']})", ha="right", **lbl)
    ax.tick_params(labelbottom=False)

    # Strip LED D1: menyala/mati per sampel, sumbu waktu sama dengan panel atas.
    al.axvspan(w0, w1, color=WIN_FILL, alpha=0.7, lw=0, zorder=0.5)
    for a, b in led:
        al.fill_between([a, b], 0, 1, color=LED_ON, lw=0, step="post", zorder=2)
    al.axvline(0, color=INK, lw=0.9, ls=(0, (1, 2)), zorder=1)
    al.axvline(end_s, color=INK, lw=0.9, ls=(0, (1, 2)), zorder=1)
    al.set_ylim(0, 1)
    style_axes(al, 0.5, 0.1, 1, 1)
    al.set_yticks([0.5])
    al.set_yticklabels(["LED D1"])
    al.tick_params(axis="y", which="both", length=0)
    al.grid(axis="y", visible=False)
    al.set_xlabel("Waktu sejak onset (s)", fontsize=11.5, color=INK)
    ax.plot([], [], color=LED_ON, lw=6, label="LED D1 menyala (plateau,\ndihitung ulang dari CSV)")
    ax.set_ylabel("Gaya (kg)", fontsize=11.5, color=INK)
    leg = ax.legend(loc="upper left", bbox_to_anchor=(1.01, 1.0), frameon=False, fontsize=9.5,
                    borderaxespad=0)
    for txt in leg.get_texts():
        txt.set_color(INK)

    dt = t.diff().median()
    ax.set_title(f"Percobaan {p} - sesi {sesi}, boot {boot}", loc="left", fontsize=13, color=INK, pad=24)
    ax.text(0, 1.02, f"{len(g)} sampel, selang median {dt * 1000:.0f} ms "
            f"({comma(round(1 / dt, 1))} sampel/s). Skala nominal, belum terkalibrasi. "
            "Kg di CSV dibulatkan 0,01, jadi jendela tepat di batas 0,50 kg bisa berbeda dari LED nyata.",
            transform=ax.transAxes, fontsize=9.5, color=INK_2)

    # ---------- Panel bawah: perbesaran jendela ref ----------
    pad_s = 0.3
    zmask = (t >= w0 - pad_s) & (t <= w1 + pad_s)
    zt, zk = t[zmask], g.loc[zmask, "kg"]
    az.axvspan(w0, w1, color=WIN_FILL, alpha=0.7, lw=0, zorder=0.5)
    if wk:
        az.axhspan(min(wk), max(wk), color=WIN, alpha=0.08, lw=0, zorder=0.6)
        for yv in (min(wk), max(wk)):
            az.axhline(yv, color=WIN, lw=0.8, ls=(0, (2, 2)), zorder=1)
    az.plot(zt, zk, color=TRACE, lw=1.1, zorder=2)
    az.plot(zt, zk, "o", ms=4, color=TRACE, zorder=3)
    az.plot(wt, wk, "o", ms=7, color=WIN, mec="white", mew=1, zorder=4)
    az.axhline(ref, color=WIN, lw=1.8, zorder=3.5)
    for x, y in zip(wt, wk):
        az.annotate(kg2(y), (x, y), xytext=(0, 9), textcoords="offset points", ha="center",
                    fontsize=9.5, color=INK, zorder=6)
    lo = min(zk.min(), ref - STABLE_KG / 2) - 0.15
    hi = max(zk.max(), ref + STABLE_KG / 2) + 0.2
    az.set_xlim(w0 - pad_s, w1 + pad_s)
    az.set_ylim(lo, hi)
    style_axes(az, 0.1, 0.05, 0.1, 0.05)
    az.set_xlabel("Waktu sejak onset (s)", fontsize=11.5, color=INK)
    az.set_ylabel("Gaya (kg)", fontsize=11.5, color=INK)
    verdict = "≤" if spread <= STABLE_KG else ">"
    az.set_title(f"Jendela ref ({kind}): t = {comma(round(w0, 3))} sampai {comma(round(w1, 3))} s",
                 loc="left", fontsize=11.5, color=INK, pad=8)
    info = (f"ref = rata-rata 5 sampel = {kg2(ref)} kg\n"
            f"rentang maks - min = {kg2(spread)} kg {verdict} {kg2(STABLE_KG)} kg (batas plateau)\n"
            f"semua sampel > {comma(ONSET_KG)} kg: {'ya' if wk and min(wk) > ONSET_KG else 'tidak'}")
    az.text(1.02, 1.0, info, transform=az.transAxes, va="top", ha="left", fontsize=9.5, color=INK,
            linespacing=1.5)
    if note:
        az.text(1.02, 0.35, note, transform=az.transAxes, va="top", ha="left", fontsize=9,
                color=PEAK, wrap=True)

    fig.savefig(out, facecolor="white", bbox_inches="tight")
    plt.close(fig)
    print(f"[plot] {out}")
    return out


def plot_trials(path, boot=None, sesi=None, outdir=None):
    """Tiga gambar (satu per percobaan). Mengembalikan daftar nama berkas PNG."""
    df = load(path)
    if boot is None or sesi is None:
        boot, sesi = latest_complete(df)
    boot, sesi = str(boot), int(sesi)
    trials = pick_trials(df, boot, sesi)
    if not trials:
        raise SystemExit(f"Sesi {sesi} (boot {boot}) tidak punya percobaan dengan ref.")
    if outdir is None:
        outdir = os.path.dirname(os.path.abspath(path))
    outs = []
    for p in sorted(trials):
        out = os.path.join(outdir, f"grafik_genggam_{boot}_sesi{sesi}_p{p}.png")
        outs.append(plot_trial(df, boot, sesi, p, trials[p], out))
    return outs


# Di Colab/Jupyter bagian ini dilewati: argumen notebook bukan nama CSV; panggil plot_trials() di sel lain.
if __name__ == "__main__" and "ipykernel" not in sys.modules:
    ap = argparse.ArgumentParser(description="Grafik gaya genggam per percobaan dari hgd_genggam_*.csv")
    ap.add_argument("csv", help="berkas hgd_genggam_YYYYMMDD.csv")
    ap.add_argument("--boot", help="ID boot, mis. 5068 (lihat kolom boot)")
    ap.add_argument("--sesi", type=int, help="nomor sesi di boot itu")
    ap.add_argument("--outdir", help="folder PNG (bawaan: folder CSV)")
    a = ap.parse_args()
    plot_trials(a.csv, a.boot, a.sesi, a.outdir)

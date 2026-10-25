"""Grafik tiga percobaan satu sesi dari hgd_genggam_YYYYMMDD.csv, dua gambar:

1. berurutan: tiga genggaman sesuai urutan waktu, satu panel per percobaan dengan sumbu gaya
   yang sama; jeda istirahat di antaranya ditulis (dari jam laptop, ketelitian ±1 s), tidak
   digambar sesuai skala, supaya bentuk tiap genggaman tetap terbaca;
2. bertumpuk: ketiga genggaman pada satu sumbu, disejajarkan di onset (t = 0).

Tiap sampel HX711 digambar sebagai titik; lima sampel jendela ref tiap percobaan diperbesar.
Grafik per percobaan yang lebih rinci (strip LED, perbesaran jendela) ada di plot_grip.py.

Cara pakai di laptop (butuh pandas dan matplotlib):
    python laptop/plot_session.py ..\\data\\hgd_genggam_20261025.csv
    python laptop/plot_session.py ..\\data\\hgd_genggam_20261025.csv --boot 5068 --sesi 3

Cara pakai di Google Colab (pandas dan matplotlib sudah ada):
    1. Sel 1: tempel seluruh isi berkas ini, jalankan (tidak ada keluaran).
    2. Sel 2:
           from google.colab import files
           up = files.upload()                    # pilih hgd_genggam_YYYYMMDD.csv
           pngs = plot_overview(list(up)[0])      # atau plot_overview(..., boot="5068", sesi=3)
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

ONSET_KG = 1.0      # THRESHOLD_ONSET_KG di config.h
PRE_ONSET_S = 1.0   # detik sebelum onset yang ditampilkan

# Warna percobaan 1, 2, 3: tiga slot pertama palet kategorikal tervalidasi, urutan tetap.
SERIES = ["#2a78d6", "#eb6834", "#1baf7a"]
INK = "#0b0b0b"
INK_2 = "#52514e"
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
    need = {"timestamp", "boot", "sesi", "percobaan", "upaya", "jenis", "ms", "kg", "ref_kg"}
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


def style_axes(ax, xmajor, xminor, ymajor, yminor):
    for side in ("top", "right", "bottom", "left"):
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
    ax.grid(which="major", color=GRID_MAJOR, linewidth=0.8)
    ax.grid(which="minor", color=GRID_MINOR, linewidth=0.5)
    ax.set_axisbelow(True)


def trial_data(df, boot, sesi, trials):
    """Per percobaan: waktu sejak onset, kg, jendela ref, dan perkiraan jam onset/akhir (laptop)."""
    out = {}
    for p, mk in sorted(trials.items()):
        g = df[(df["jenis"] == "G") & (df["boot"] == boot) & (df["sesi"] == sesi)
               & (df["percobaan"] == p) & (df["upaya"] == mk["upaya"])].sort_values("ms")
        onset = float(mk["onset_ms"])
        t = ((g["ms"] - onset) / 1000).to_numpy()
        kg = g["kg"].to_numpy()
        keep = t >= -PRE_ONSET_S
        w0 = (float(mk["jendela_awal_ms"]) - onset) / 1000
        w1 = (float(mk["jendela_akhir_ms"]) - onset) / 1000
        end_s = (float(mk["akhir_ms"]) - onset) / 1000
        # Jam laptop saat penanda M ditulis (≈ akhir upaya, resolusi 1 s); onset = akhir - durasi.
        t_end = pd.Timestamp(mk["timestamp"])
        out[p] = {"t": t[keep], "kg": kg[keep], "w0": w0, "w1": w1, "end_s": end_s,
                  "ref": float(mk["ref_kg"]), "peak": float(mk["puncak_kg"]),
                  "exit": mk["keluar"], "clock_end": t_end,
                  "clock_onset": t_end - pd.Timedelta(seconds=end_s)}
    return out


def draw_trial(ax, d, color, label=None):
    """Satu genggaman: garis + titik tiap sampel, lima sampel jendela ref diperbesar."""
    ax.plot(d["t"], d["kg"], color=color, lw=1.4, zorder=3, label=label)
    ax.plot(d["t"], d["kg"], "o", ms=2.8, color=color, zorder=3)
    inw = (d["t"] >= d["w0"] - 1e-6) & (d["t"] <= d["w1"] + 1e-6)
    ax.plot(d["t"][inw], d["kg"][inw], "o", ms=6.5, color=color, mec="white", mew=0.9, zorder=4)
    ax.plot([d["w0"], d["w1"]], [d["ref"], d["ref"]], color=INK, lw=1.6, zorder=5)


def subtitle(fig, text):
    fig.text(0.01, 0.955, text, fontsize=9.5, color=INK_2, ha="left")


def plot_series(data, boot, sesi, out):
    ps = sorted(data)
    widths = [max(d["end_s"], d["t"].max()) + PRE_ONSET_S + 0.2 for d in (data[p] for p in ps)]
    fig, axes = plt.subplots(1, len(ps), figsize=(13, 5.6), dpi=200, sharey=True,
                             gridspec_kw={"width_ratios": widths, "wspace": 0.18})
    fig.patch.set_facecolor("white")
    ymax = max(max(d["kg"].max(), d["ref"]) for d in data.values()) + 0.5
    for i, (ax, p) in enumerate(zip(axes, ps)):
        d = data[p]
        color = SERIES[(p - 1) % len(SERIES)]
        ax.axhline(ONSET_KG, color=INK_2, lw=1, ls=(0, (5, 3)), zorder=1)
        ax.axvline(0, color=INK, lw=0.8, ls=(0, (1, 2)), zorder=1)
        ax.axvline(d["end_s"], color=INK, lw=0.8, ls=(0, (1, 2)), zorder=1)
        draw_trial(ax, d, color)
        ax.set_xlim(-PRE_ONSET_S, max(d["end_s"], d["t"].max()) + 0.2)
        ax.set_ylim(-0.3, ymax)
        style_axes(ax, 1, 0.2, 0.5, 0.1)
        if i > 0:
            ax.tick_params(labelleft=False)
        ax.set_title(f"Percobaan {p}: ref {kg2(d['ref'])} kg, puncak {kg2(d['peak'])} kg\n"
                     f"onset {d['clock_onset']:%H:%M:%S}, akhir {d['exit']}",
                     fontsize=10.5, color=INK, loc="left")
        ax.text(d["w1"] + 0.08, d["ref"], kg2(d["ref"]), va="center", fontsize=9.5, color=INK, zorder=6)
    # Jeda istirahat antarpanel (jam laptop, ±1 s), ditulis di celah.
    for i in range(len(ps) - 1):
        a, b = data[ps[i]], data[ps[i + 1]]
        gap = (b["clock_onset"] - a["clock_end"]).total_seconds()
        x0 = axes[i].get_position().x1
        x1 = axes[i + 1].get_position().x0
        fig.text((x0 + x1) / 2, axes[i].get_position().y0 + 0.5 * axes[i].get_position().height,
                 f"jeda\n≈ {gap:.0f} s", ha="center", va="center", fontsize=9, color=INK_2,
                 rotation=90)
    axes[0].set_ylabel("Gaya (kg)", fontsize=11.5, color=INK)
    fig.supxlabel("Waktu sejak onset tiap percobaan (s); jeda istirahat tidak sesuai skala",
                  fontsize=11, color=INK, y=0.01)
    fig.suptitle(f"Sesi {sesi}, boot {boot}: tiga percobaan berurutan", x=0.01, ha="left",
                 fontsize=13, color=INK, y=1.02)
    subtitle(fig, "Titik = sampel HX711 (≈ 85 ms); titik besar = 5 sampel jendela ref; garis hitam = ref; "
                  "garis putus = ambang onset 1 kg. Jeda dari jam laptop (±1 s). Skala nominal, belum terkalibrasi.")
    fig.savefig(out, facecolor="white", bbox_inches="tight")
    plt.close(fig)
    print(f"[plot] {out}")
    return out


def plot_overlap(data, boot, sesi, out):
    fig, ax = plt.subplots(figsize=(11, 6.2), dpi=200)
    fig.patch.set_facecolor("white")
    ax.axhline(ONSET_KG, color=INK_2, lw=1, ls=(0, (5, 3)), zorder=1, label="ambang onset 1 kg")
    ax.axvline(0, color=INK, lw=0.8, ls=(0, (1, 2)), zorder=1)
    refs = []
    for p in sorted(data):
        d = data[p]
        refs.append(d["ref"])
        color = SERIES[(p - 1) % len(SERIES)]
        draw_trial(ax, d, color, label=f"Percobaan {p}: ref {kg2(d['ref'])} kg (puncak {kg2(d['peak'])})")
        ax.text(d["w1"] + 0.06, d["ref"], kg2(d["ref"]), va="center", fontsize=9.5, color=INK, zorder=6)
    ax.plot([], [], color=INK, lw=1.6, label="ref (rata-rata 5 sampel jendela)")
    xmax = max(max(d["end_s"], d["t"].max()) for d in data.values()) + 0.2
    ax.set_xlim(-PRE_ONSET_S, xmax)
    ax.set_ylim(-0.3, max(max(d["kg"].max(), d["ref"]) for d in data.values()) + 0.5)
    style_axes(ax, 0.5, 0.1, 0.5, 0.1)
    ax.set_xlabel("Waktu sejak onset (s)", fontsize=11.5, color=INK)
    ax.set_ylabel("Gaya (kg)", fontsize=11.5, color=INK)
    title = f"Sesi {sesi}, boot {boot}: tiga percobaan disejajarkan di onset"
    if len(refs) == 3:
        title += f" (puncak {kg2(max(refs))} kg, rerata {kg2(sum(refs) / 3)} kg)"
    ax.set_title(title, loc="left", fontsize=13, color=INK, pad=24)
    ax.text(0, 1.02, "Titik = sampel HX711 (≈ 85 ms); titik besar = 5 sampel jendela ref. "
            "Skala nominal, belum terkalibrasi.", transform=ax.transAxes, fontsize=9.5, color=INK_2)
    leg = ax.legend(loc="upper left", bbox_to_anchor=(1.01, 1.0), frameon=False, fontsize=9.5,
                    borderaxespad=0)
    for txt in leg.get_texts():
        txt.set_color(INK)
    fig.savefig(out, facecolor="white", bbox_inches="tight")
    plt.close(fig)
    print(f"[plot] {out}")
    return out


def plot_overview(path, boot=None, sesi=None, outdir=None):
    """Dua gambar (berurutan, bertumpuk). Mengembalikan daftar nama berkas PNG."""
    df = load(path)
    if boot is None or sesi is None:
        boot, sesi = latest_complete(df)
    boot, sesi = str(boot), int(sesi)
    trials = pick_trials(df, boot, sesi)
    if not trials:
        raise SystemExit(f"Sesi {sesi} (boot {boot}) tidak punya percobaan dengan ref.")
    if outdir is None:
        outdir = os.path.dirname(os.path.abspath(path))
    data = trial_data(df, boot, sesi, trials)
    base = os.path.join(outdir, f"grafik_sesi_{boot}_sesi{sesi}")
    return [plot_series(data, boot, sesi, base + "_berurutan.png"),
            plot_overlap(data, boot, sesi, base + "_bertumpuk.png")]


# Di Colab/Jupyter bagian ini dilewati: argumen notebook bukan nama CSV; panggil plot_overview() di sel lain.
if __name__ == "__main__" and "ipykernel" not in sys.modules:
    ap = argparse.ArgumentParser(description="Grafik tiga percobaan satu sesi dari hgd_genggam_*.csv")
    ap.add_argument("csv", help="berkas hgd_genggam_YYYYMMDD.csv")
    ap.add_argument("--boot", help="ID boot, mis. 5068 (lihat kolom boot)")
    ap.add_argument("--sesi", type=int, help="nomor sesi di boot itu")
    ap.add_argument("--outdir", help="folder PNG (bawaan: folder CSV)")
    a = ap.parse_args()
    plot_overview(a.csv, a.boot, a.sesi, a.outdir)

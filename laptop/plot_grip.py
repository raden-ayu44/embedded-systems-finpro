"""Grafik gaya genggam (HGS) satu sesi dari hgd_genggam_YYYYMMDD.csv.

Satu gambar per sesi: gaya (kg) terhadap waktu sejak onset untuk tiga percobaan,
dengan ambang onset 1 kg dan jendela stabil terkuat (ref) tiap percobaan.

Cara pakai di laptop (butuh pandas dan matplotlib):
    python laptop/plot_grip.py ..\\data\\hgd_genggam_20261025.csv
    python laptop/plot_grip.py ..\\data\\hgd_genggam_20261025.csv --boot 5068 --sesi 3

Cara pakai di Google Colab (pandas dan matplotlib sudah ada):
    1. Sel 1: tempel seluruh isi berkas ini, jalankan.
    2. Sel 2:
           from google.colab import files
           up = files.upload()                   # pilih hgd_genggam_YYYYMMDD.csv
           png = plot_session(list(up)[0])       # atau plot_session(..., boot="5068", sesi=3)
           files.download(png)

Tanpa boot/sesi: dipakai sesi lengkap (tiga percobaan dengan ref) yang paling akhir di berkas.
Nilai kg memakai skala firmware; sebelum kalibrasi itu skala nominal.
"""

import argparse
import os

import matplotlib
matplotlib.use("Agg")  # tanpa jendela: langsung ke berkas PNG (juga aman di Colab)
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import pandas as pd

ONSET_KG = 1.0  # ambang onset firmware (THRESHOLD_ONSET_KG di config.h)
PRE_ONSET_S = 1.0  # detik sebelum onset yang ditampilkan (masa menunggu sebelumnya datar di 0)

# Warna: tiga slot pertama palet kategorikal yang sudah divalidasi (biru, oranye, aqua),
# urutan tetap: percobaan 1, 2, 3. Teks memakai warna teks, bukan warna seri.
SERIES = ["#2a78d6", "#eb6834", "#1baf7a"]
SURFACE = "#fcfcfb"
TEXT = "#0b0b0b"
TEXT_2 = "#52514e"
GRID = "#e4e3df"


def comma(x, _pos=None):
    """Angka dengan koma desimal (gaya Indonesia) untuk sumbu."""
    return f"{x:g}".replace(".", ",")


def kg2(x):
    """Dua desimal dengan koma, mis. 3,70."""
    return f"{x:.2f}".replace(".", ",")


def load(path):
    # boot dibaca sebagai teks: "5068" dan "29B9" sama-sama ID heksadesimal.
    df = pd.read_csv(path, dtype={"boot": str})
    df = df[df["sesi"].notna()].copy()
    df["sesi"] = df["sesi"].astype(int)
    return df


def pick_trials(df, boot, sesi):
    """Penanda M terakhir yang punya ref untuk tiap percobaan 1-3 di sesi ini."""
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


def plot_session(path, boot=None, sesi=None, out=None):
    df = load(path)
    if boot is None or sesi is None:
        boot, sesi = latest_complete(df)
    boot, sesi = str(boot), int(sesi)
    trials = pick_trials(df, boot, sesi)
    if not trials:
        raise SystemExit(f"Sesi {sesi} (boot {boot}) tidak punya percobaan dengan ref.")

    plt.rcParams.update({"font.size": 11, "axes.edgecolor": TEXT_2, "axes.labelcolor": TEXT,
                         "xtick.color": TEXT_2, "ytick.color": TEXT_2})
    fig, ax = plt.subplots(figsize=(10, 5.6), dpi=200)
    fig.patch.set_facecolor(SURFACE)
    ax.set_facecolor(SURFACE)

    refs = []
    for p in sorted(trials):
        mk = trials[p]
        color = SERIES[(p - 1) % len(SERIES)]
        g = df[(df["jenis"] == "G") & (df["boot"] == boot) & (df["sesi"] == sesi)
               & (df["percobaan"] == p) & (df["upaya"] == mk["upaya"])].sort_values("ms")
        ref, peak = float(mk["ref_kg"]), float(mk["puncak_kg"])
        refs.append(ref)
        label = f"Percobaan {p}: ref {kg2(ref)} kg (puncak {kg2(peak)})"
        ax.plot(g["t_onset_s"], g["kg"], color=color, linewidth=2, label=label, zorder=3)
        # Jendela stabil terkuat: garis tebal setinggi ref di rentang waktunya.
        onset = float(mk["onset_ms"])
        x0 = (float(mk["jendela_awal_ms"]) - onset) / 1000
        x1 = (float(mk["jendela_akhir_ms"]) - onset) / 1000
        ax.plot([x0, x1], [ref, ref], color=color, linewidth=6, solid_capstyle="round", zorder=4)
        ax.plot([x0, x1], [ref, ref], color=SURFACE, linewidth=8.5, solid_capstyle="round", zorder=3.9)
        ax.annotate(kg2(ref), xy=(x1, ref), xytext=(8, 0), textcoords="offset points",
                    va="center", color=TEXT, fontsize=10, zorder=5)

    # Ambang onset dan titik nol waktu: garis bantu yang samar.
    ax.axhline(ONSET_KG, color=TEXT_2, linewidth=1, linestyle=(0, (4, 3)), zorder=2)
    ax.annotate("ambang onset 1 kg", xy=(0, ONSET_KG), xycoords=("axes fraction", "data"),
                xytext=(4, 4), textcoords="offset points", color=TEXT_2, fontsize=9)
    ax.axvline(0, color=GRID, linewidth=1, zorder=1)

    ax.grid(axis="y", color=GRID, linewidth=0.8, zorder=0)
    for side in ("top", "right"):
        ax.spines[side].set_visible(False)
    ax.set_ylim(bottom=min(0, ax.get_ylim()[0]))
    ax.set_xlim(left=-PRE_ONSET_S)
    ax.xaxis.set_major_formatter(FuncFormatter(comma))
    ax.yaxis.set_major_formatter(FuncFormatter(comma))
    ax.set_xlabel("Waktu sejak onset (s)")
    ax.set_ylabel("Gaya (kg)")

    peak_ref = max(refs)
    mean_ref = sum(refs) / len(refs)
    title = f"Sesi {sesi} (boot {boot}): puncak {kg2(peak_ref)} kg"
    if len(refs) == 3:
        title += f", rerata {kg2(mean_ref)} kg"
    ax.set_title(title, loc="left", color=TEXT, fontsize=13, pad=22)
    ax.text(0, 1.02, "Garis tebal: jendela stabil terkuat (ref). Skala nominal, belum terkalibrasi.",
            transform=ax.transAxes, color=TEXT_2, fontsize=9)
    # Legenda di bawah grafik, satu baris, supaya tidak menutupi kurva.
    leg = ax.legend(loc="upper center", bbox_to_anchor=(0.5, -0.14), ncol=3, frameon=False, fontsize=9.5)
    for t in leg.get_texts():
        t.set_color(TEXT)

    if out is None:
        out = os.path.join(os.path.dirname(os.path.abspath(path)), f"grafik_genggam_{boot}_sesi{sesi}.png")
    fig.tight_layout()
    fig.savefig(out, facecolor=SURFACE)
    plt.close(fig)
    print(f"[plot] {out}")
    return out


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description="Grafik gaya genggam satu sesi dari hgd_genggam_*.csv")
    ap.add_argument("csv", help="berkas hgd_genggam_YYYYMMDD.csv")
    ap.add_argument("--boot", help="ID boot, mis. 5068 (lihat kolom boot)")
    ap.add_argument("--sesi", type=int, help="nomor sesi di boot itu")
    ap.add_argument("--out", help="nama berkas PNG (bawaan: di samping CSV)")
    a, _ = ap.parse_known_args()  # parse_known_args: argumen tambahan Colab/Jupyter diabaikan
    plot_session(a.csv, a.boot, a.sesi, a.out)

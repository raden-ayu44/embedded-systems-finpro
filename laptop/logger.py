"""
logger.py - pencatat sesi HGD di laptop (embedded system, Kelompok 01)
---------------------------------------------------------------------
Peran (alur-program-hgd.md Bagian 4.8, 5.22, 9):
- membaca baris dari port serial ESP32 (115200 baud);
- baris "HGD,..." divalidasi (awalan, jumlah kolom, checksum XOR); valid -> ditulis ke CSV harian
  hgd_YYYYMMDD.csv, flush(), lalu dibalas "ACK,<seq>"; tidak valid -> "NAK,<seq>" tanpa menulis;
- baris lain (teks debug, [FAULT], [GAGAL SIMPAN], ...) dicetak ke konsol, tidak ke CSV;
- tombol yang diketik di konsol ini (t, p, x, s, u, r) diteruskan ke ESP32, karena Serial Monitor
  tidak bisa dibuka selama logger.py memegang port.

Kebutuhan: Python 3 dan pyserial (diuji dengan Python 3.11 dan pyserial 3.5):
    pip install pyserial
Cara pakai:
    python logger.py COM3            (Windows; di Linux mis. /dev/ttyUSB0)
    python logger.py COM3 --dir data (CSV ditulis ke folder "data")
Berhenti: Ctrl+C.
Alat uji: --kirim u (uji ACK 100 baris, U9/E5), --kirim r (baris rusak -> NAK, U10).

Catatan:
- Port dibuka dengan RTS dan DTR mati, jadi membuka (atau membuka ulang) logger.py TIDAK me-reset
  ESP32 dan sesi di RAM tidak hilang. (Pada papan DevKit, RTS terhubung ke pin EN = reset.)
- Bila ACK hilang dan ESP32 mengirim ulang baris yang sama, baris itu dibalas ACK lagi tetapi tidak
  ditulis dua kali.
- Byte sampah di depan "HGD," (kadang muncul di jalur USB) dilewati; checksum hanya menghitung
  dari "HGD," sampai sebelum "*", jadi isi baris tetap terjaga.
"""

import argparse
import csv
import datetime
import os
import sys
import time

import serial  # pyserial

try:
    import msvcrt  # Windows: baca tombol tanpa menunggu
except ImportError:  # sistem lain: penerusan tombol tidak tersedia
    msvcrt = None

BAUD = 115200
CSV_HEADER = ["timestamp", "sesi", "jenis", "percobaan", "gaya_kg", "rerata_kg", "catatan"]
KEYS_TO_FORWARD = "tpxsur"  # perintah debug firmware (Bagian 5.21)
EVENT_NAMES = {  # kode baris E (Bagian 9.2)
    "1": "HX711_TIMEOUT",
    "2": "ADC_SATURASI",
    "3": "DI_LUAR_RENTANG",
    "4": "READING_INVALID",
    "5": "FORCE_RESET",
}
VALID_TYPES = "TSEU"  # U = baris uji ACK (perintah u/r), di-ACK tetapi tidak ditulis ke CSV
RECENT_MAX = 32       # jumlah baris terakhir yang diingat untuk menolak duplikat


def checksum(text):
    """XOR semua byte teks, sebagai dua digit heksadesimal huruf besar."""
    x = 0
    for ch in text.encode("ascii", "replace"):
        x ^= ch
    return "%02X" % x


def parse_line(line):
    """Kembalikan (fields, seq, error). fields = daftar 7 kolom bila valid, error = None.
    Bila tidak valid: fields = None, error = alasan, seq = nomor seq bila masih terbaca."""
    start = line.rfind("HGD,")
    if start < 0:
        return None, None, "bukan baris HGD"
    line = line[start:]  # lewati sampah di depan
    if "*" not in line:
        return None, None, "tanpa checksum"
    body, cs = line.rsplit("*", 1)
    fields = body.split(",")
    seq = fields[2] if len(fields) > 2 and fields[2].isdigit() else None
    if len(fields) != 7:
        return None, seq, "jumlah kolom %d, harus 7" % len(fields)
    if fields[1] not in VALID_TYPES or len(fields[1]) != 1:
        return None, seq, "jenis tidak dikenal"
    if seq is None:
        return None, None, "seq tidak terbaca"
    if checksum(body) != cs.strip().upper():
        return None, seq, "checksum salah"
    return fields, seq, None


def csv_row(fields):
    """Ubah 7 kolom baris HGD menjadi satu baris CSV (Bagian 9.3). None untuk baris uji U."""
    kind, sesi, trial, v1, v2 = fields[1], fields[3], fields[4], fields[5], fields[6]
    stamp = datetime.datetime.now().isoformat(timespec="seconds")
    if kind == "T":
        note = "DEMO" if v2 == "1" else ""
        return [stamp, sesi, "T", trial, v1, "", note]
    if kind == "S":
        # percobaan di baris S = jumlah percobaan demo (0 = semua data nyata); di CSV ditulis 0
        note = ("DEMO (%s percobaan)" % trial) if trial != "0" else ""
        return [stamp, sesi, "S", "0", v1, v2, note]
    if kind == "E":
        # Bagian 9.3: gaya_kg = kode, rerata_kg = detik sejak sesi mulai, catatan = nama galat
        return [stamp, sesi, "E", trial, v1, v2, EVENT_NAMES.get(v1, "KODE_" + v1)]
    return None  # U: baris uji


class CsvLog:
    """Satu berkas per hari, header sekali, flush() tiap baris."""

    def __init__(self, folder):
        self.folder = folder
        self.path = None
        self.file = None
        self.writer = None

    def write(self, row):
        path = os.path.join(self.folder, "hgd_%s.csv" % datetime.date.today().strftime("%Y%m%d"))
        if path != self.path:  # hari baru (atau pertama kali): buka berkas yang sesuai
            if self.file:
                self.file.close()
            is_new = not os.path.exists(path) or os.path.getsize(path) == 0
            self.file = open(path, "a", newline="", encoding="utf-8")
            self.writer = csv.writer(self.file)
            if is_new:
                self.writer.writerow(CSV_HEADER)
            self.path = path
        self.writer.writerow(row)
        self.file.flush()  # data aman walau laptop mati mendadak

    def close(self):
        if self.file:
            self.file.close()


def handle_hgd(line, port, log, recent, stats):
    fields, seq, error = parse_line(line)
    if error:
        stats["nak"] += 1
        if seq is not None:
            port.write(("NAK,%s\n" % seq).encode("ascii"))
            print("[logger] NAK,%s (%s): %s" % (seq, error, line))
        else:
            print("[logger] baris rusak tanpa seq, diabaikan (%s): %s" % (error, line))
        return
    port.write(("ACK,%s\n" % seq).encode("ascii"))  # balas dulu: ESP32 menunggu
    key = ",".join(fields)
    if key in recent:  # kiriman ulang karena ACK sebelumnya hilang: jangan tulis dua kali
        print("[logger] ACK,%s (duplikat, tidak ditulis ulang)" % seq)
        return
    recent.append(key)
    del recent[:-RECENT_MAX]
    row = csv_row(fields)
    if row is None:
        stats["test"] += 1
        print("[logger] ACK,%s (baris uji U, tidak ditulis ke CSV)" % seq)
        return
    log.write(row)
    stats["written"] += 1
    print("[logger] ACK,%s  %s" % (seq, ",".join(row[1:])))


def forward_keys(port):
    """Teruskan tombol yang diketik di konsol ke ESP32 (Windows)."""
    if msvcrt is None:
        return
    while msvcrt.kbhit():
        ch = msvcrt.getwch()
        if ch in KEYS_TO_FORWARD:
            port.write(ch.encode("ascii"))
            print("[logger] kirim '%s' ke ESP32" % ch)


def main():
    ap = argparse.ArgumentParser(description="Pencatat sesi HGD (CSV + ACK)")
    ap.add_argument("port", help="port serial ESP32, mis. COM3")
    ap.add_argument("--dir", default=".", help="folder berkas CSV (bawaan: folder kerja)")
    ap.add_argument("--kirim", default="",
                    help="alat uji: huruf perintah yang dikirim 3 s setelah mulai, mis. u (uji ACK)")
    args = ap.parse_args()

    # Karakter aneh dari jalur USB tidak boleh membuat print() berhenti.
    # line_buffering: tiap baris langsung tampil, juga bila keluaran diarahkan ke berkas.
    sys.stdout.reconfigure(errors="backslashreplace", line_buffering=True)
    os.makedirs(args.dir, exist_ok=True)

    port = serial.Serial()
    port.port = args.port
    port.baudrate = BAUD
    port.timeout = 0
    port.rts = False  # RTS/DTR mati sebelum dibuka: ESP32 tidak di-reset
    port.dtr = False
    port.open()

    log = CsvLog(args.dir)
    recent = []
    stats = {"written": 0, "nak": 0, "test": 0}
    buf = b""
    t_start = time.time()
    to_send = args.kirim
    print("[logger] port %s terbuka (%d baud), CSV di %s" % (args.port, BAUD, os.path.abspath(args.dir)))
    if msvcrt:
        print("[logger] tombol diteruskan ke ESP32: %s; berhenti: Ctrl+C" % ", ".join(KEYS_TO_FORWARD))
    try:
        while True:
            data = port.read(port.in_waiting or 1)
            if data:
                buf += data
                while b"\n" in buf:
                    raw, buf = buf.split(b"\n", 1)
                    line = raw.decode("ascii", "backslashreplace").strip()
                    if not line:
                        continue
                    if "HGD," in line:
                        handle_hgd(line, port, log, recent, stats)
                    else:
                        print(line)  # teks debug ESP32: hanya ke konsol
            else:
                time.sleep(0.001)  # tidak ada data: istirahat 1 ms supaya CPU tidak 100 %
            forward_keys(port)
            if to_send and time.time() - t_start > 3.0:  # --kirim: satu huruf per detik
                port.write(to_send[0].encode("ascii"))
                print("[logger] kirim '%s' ke ESP32 (--kirim)" % to_send[0])
                to_send = to_send[1:]
                t_start = time.time() - 2.0
    except KeyboardInterrupt:
        pass
    finally:
        log.close()
        port.close()
        print("[logger] berhenti: %d baris ditulis, %d NAK, %d baris uji" %
              (stats["written"], stats["nak"], stats["test"]))


if __name__ == "__main__":
    main()

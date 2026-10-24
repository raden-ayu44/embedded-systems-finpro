r"""
logger.py - pencatat sesi HGD di laptop (embedded system, Kelompok 01)
---------------------------------------------------------------------
Peran (alur-program-hgd.md Bagian 4.8, 5.22, 9):
- membaca baris dari port serial ESP32 (115200 baud);
- baris "HGD,..." divalidasi (awalan, jumlah kolom, checksum XOR); valid -> ditulis ke CSV,
  flush + fsync, lalu dibalas "ACK,<seq>"; tidak valid atau gagal ditulis -> "NAK,<seq>";
- langkah 10c, tiga berkas CSV per hari:
    hgd_YYYYMMDD.csv          baris T (percobaan) dan S (ringkasan sesi), kolom sama seperti langkah 10;
    hgd_galat_YYYYMMDD.csv    baris E (galat 1-4, reset total 5) dengan nama kodenya;
    hgd_genggam_YYYYMMDD.csv  tiap sampel selama GENGGAM (baris G, tanpa ACK) dan satu penanda M per
                              upaya genggam. Baris G disimpan di memori per upaya; saat penanda M
                              datang, semua baris G upaya itu + baris M ditulis sekaligus, lalu ACK.
                              Jadi ACK penanda berarti "rekaman upaya ini sudah di disk".
- baris lain (teks debug, [FAULT], [GAGAL SIMPAN], ...) dicetak ke konsol, tidak ke CSV;
- tombol yang diketik di konsol ini (t, p, x, s, u, r) diteruskan ke ESP32, karena Serial Monitor
  tidak bisa dibuka selama logger.py memegang port.

Kebutuhan: Python 3 dan pyserial (diuji dengan Python 3.11 dan pyserial 3.5):
    pip install pyserial
Cara pakai (dari folder repo):
    python laptop/logger.py COM3 --dir ..\data   (Windows; CSV di folder "data" di samping repo)
    python laptop/logger.py /dev/ttyUSB0 --dir ../data   (Linux)
Tanpa --dir, CSV ditulis ke folder kerja. Jangan tulis CSV ke dalam repo (data pengukuran).
Jangan membuka CSV hari ini di Excel selama logger.py berjalan: Excel mengunci berkas, baris tidak
bisa ditulis, logger.py membalas NAK. Untuk hgd_YYYYMMDD.csv sesi berakhir di GAGAL SIMPAN; untuk
berkas genggaman ESP32 hanya membuang penanda itu (data sesi tidak terpengaruh); logger.py menyimpan
penanda dan baris G-nya di memori dan menulisnya terlambat (catatan "ditulis terlambat") setelah
berkas ditutup. NAK tetap berarti "tidak tertulis sekarang"; penulisan terlambat hanya usaha terbaik.
Berhenti: Ctrl+C (rekaman genggaman yang tertunda dicoba ditulis sekali lagi; bila tetap gagal,
jumlah upaya dan baris yang hilang dicetak).
Alat uji: --kirim u (uji ACK 100 baris, U9/E5), --kirim r (baris rusak -> NAK, U10).

Catatan:
- Port dibuka dengan RTS dan DTR mati, jadi membuka (atau membuka ulang) logger.py TIDAK me-reset
  ESP32 dan sesi di RAM tidak hilang. (Pada papan DevKit, RTS terhubung ke pin EN = reset.)
- Bila ACK hilang dan ESP32 mengirim ulang baris yang sama, baris itu dibalas ACK lagi tetapi tidak
  ditulis dua kali. Langkah 10c: tiap baris membawa nomor boot ESP32, jadi baris yang sama persis
  SETELAH reset ESP32 (seq dan sesi mulai lagi dari 1) tidak dianggap duplikat.
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
KEYS_TO_FORWARD = "tpxsur"  # perintah debug firmware (Bagian 5.21)

# Jumlah kolom tiap jenis baris, termasuk "HGD", jenis, seq dan boot (Bagian 9.2).
FIELD_COUNT = {"T": 8, "S": 8, "E": 8, "U": 8, "G": 9, "M": 17}

# Kolom CSV (Bagian 9.3).
DAILY_HEADER = ["timestamp", "sesi", "jenis", "percobaan", "gaya_kg", "rerata_kg", "catatan"]
FAULT_HEADER = ["timestamp", "boot", "sesi", "percobaan", "kode", "nama", "detik_sesi"]
GRIP_HEADER = ["timestamp", "boot", "sesi", "percobaan", "upaya", "jenis", "ms", "t_onset_s",
               "kg", "fase", "demo", "keluar", "onset_ms", "akhir_ms", "jendela_awal_ms",
               "jendela_akhir_ms", "ref_kg", "puncak_kg", "galat", "n", "catatan"]

EVENT_NAMES = {  # kode baris E dan kolom galat penanda M (Bagian 9.2)
    "1": "HX711_TIMEOUT",
    "2": "ADC_SATURASI",
    "3": "DI_LUAR_RENTANG",
    "4": "READING_INVALID",
    "5": "FORCE_RESET",
}
EXIT_NAMES = {  # kolom keluar penanda M = GripExit di firmware (types.h)
    "1": "EARLY_RELEASE",
    "2": "RELEASE",
    "3": "TIMEOUT",
    "4": "FAULT",
    "5": "FORCE_RESET",
    "6": "ZERO_GUARD",
}
RECENT_MAX = 32       # jumlah baris terakhir yang diingat untuk menolak duplikat
ORPHAN_AFTER_S = 10   # baris G tanpa penanda selama ini (tidak ada sampel baru) ditulis sendiri
RETRY_S = 5           # selang coba tulis ulang rekaman yang tertunda (berkas terkunci)
# Batas rekaman tertunda di memori. 20000 baris G ~ 29 menit genggaman pada 11,6 sampel/s; di
# atasnya upaya tertua dibuang dengan peringatan, supaya logger.py tidak kehabisan memori bila
# berkas genggaman lama dibiarkan terkunci.
MAX_HELD_ATTEMPTS = 50
MAX_HELD_ROWS = 20000


def checksum(text):
    """XOR semua byte teks, sebagai dua digit heksadesimal huruf besar."""
    x = 0
    for ch in text.encode("ascii", "replace"):
        x ^= ch
    return "%02X" % x


def now_stamp():
    return datetime.datetime.now().isoformat(timespec="seconds")


def parse_line(line):
    """Kembalikan (fields, seq, error). fields = daftar kolom bila valid, error = None.
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
    kind = fields[1] if len(fields) > 1 else ""
    if kind not in FIELD_COUNT:
        return None, seq, "jenis tidak dikenal"
    if len(fields) != FIELD_COUNT[kind]:
        return None, seq, "jumlah kolom %d, harus %d" % (len(fields), FIELD_COUNT[kind])
    if seq is None:
        return None, None, "seq tidak terbaca"
    if checksum(body) != cs.strip().upper():
        return None, seq, "checksum salah"
    return fields, seq, None


def daily_row(fields):
    """Baris T atau S -> satu baris hgd_YYYYMMDD.csv (kolom sama seperti langkah 10)."""
    kind, sesi, trial, v1, v2 = fields[1], fields[4], fields[5], fields[6], fields[7]
    if kind == "T":
        note = "DEMO" if v2 == "1" else ""
        return [now_stamp(), sesi, "T", trial, v1, "", note]
    # S: kolom percobaan = jumlah percobaan demo (0 = semua data nyata); di CSV ditulis 0
    note = ("DEMO (%s percobaan)" % trial) if trial != "0" else ""
    return [now_stamp(), sesi, "S", "0", v1, v2, note]


def fault_row(fields):
    """Baris E -> satu baris hgd_galat_YYYYMMDD.csv."""
    boot, sesi, trial, code, secs = fields[3], fields[4], fields[5], fields[6], fields[7]
    return [now_stamp(), boot, sesi, trial, code, EVENT_NAMES.get(code, "KODE_" + code), secs]


class CsvLog:
    """Satu berkas per hari per jenis (awalan), header sekali. Tiap kali menulis: buka, tulis,
    flush, fsync, tutup. Berkas tidak dibiarkan terbuka, supaya berkas yang dikunci program lain
    (mis. dibuka di Excel di Windows) selalu terdeteksi sebagai galat tulis -> NAK, bukan ACK palsu."""

    def __init__(self, folder, prefix, header):
        self.folder = folder
        self.prefix = prefix
        self.header = header

    def path(self):
        day = datetime.date.today().strftime("%Y%m%d")
        return os.path.join(self.folder, "%s%s.csv" % (self.prefix, day))

    def write(self, rows):
        """Tulis beberapa baris sekaligus; melempar OSError (mis. PermissionError) bila gagal."""
        path = self.path()
        is_new = not os.path.exists(path) or os.path.getsize(path) == 0
        with open(path, "a", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            if is_new:
                writer.writerow(self.header)
            writer.writerows(rows)
            f.flush()
            os.fsync(f.fileno())  # data benar-benar di disk walau laptop mati mendadak


class GripBuffer:
    """Rekaman genggaman yang belum tertulis (langkah 10c).
    - attempts: baris G per upaya (kunci = boot + nomor upaya), menunggu penanda M upaya itu.
      Baris G tidak di-ACK, jadi disimpan di memori dulu; baru ditulis bersama penandanya.
    - late: penanda M yang sudah diterima tetapi gagal ditulis (berkas terkunci) dan sudah dibalas
      NAK. Kunci = boot + seq, jadi kiriman ulang penanda yang sama tidak tersimpan dua kali.
      NAK tetap berarti "tidak tertulis SEKARANG"; ESP32 membuang penanda itu setelah kirim ulang.
      Penulisan terlambat di sini hanya usaha terbaik (best effort): bila logger.py berhenti atau
      batas memori tercapai sebelum berkas bebas, rekaman itu hilang (jumlahnya dicetak).
    - done: penanda (boot + seq) yang sudah tertulis, supaya kiriman ulang yang datang sesudah
      penulisan terlambat hanya di-ACK, tidak ditulis lagi."""

    def __init__(self, log):
        self.log = log
        self.attempts = {}   # (boot, upaya) -> {"rows": [(stamp, fields)], "last": waktu terima}
        self.late = {}       # (boot, seq) -> fields penanda M
        self.done = []       # (boot, seq) penanda yang sudah tertulis, RECENT_MAX terakhir
        self.next_retry = 0.0
        self.lost_attempts = 0
        self.lost_rows = 0
        self.late_written = 0  # penanda yang ditulis terlambat (untuk ringkasan saat berhenti)

    def held_rows(self):
        return sum(len(e["rows"]) for e in self.attempts.values())

    def add_sample(self, fields):
        key = (fields[3], fields[4])
        entry = self.attempts.setdefault(key, {"rows": [], "last": 0.0})
        entry["rows"].append((now_stamp(), fields))
        entry["last"] = time.time()
        self.enforce_cap()

    def hold_marker(self, m):
        """Penanda yang gagal ditulis disimpan untuk ditulis terlambat (kunci boot + seq)."""
        self.late[(m[3], m[2])] = m
        self.enforce_cap()

    def enforce_cap(self):
        """Batas memori: di atas MAX_HELD_ATTEMPTS upaya atau MAX_HELD_ROWS baris G, upaya tertua
        (beserta penandanya) dibuang dengan peringatan. dict Python menjaga urutan masuk."""
        while self.attempts and (len(self.attempts) > MAX_HELD_ATTEMPTS
                                 or self.held_rows() > MAX_HELD_ROWS):
            key = next(iter(self.attempts))
            rows = len(self.attempts.pop(key)["rows"])
            for lk in [k for k, m in self.late.items() if (m[3], m[6]) == key]:
                del self.late[lk]
            self.lost_attempts += 1
            self.lost_rows += rows
            print("[logger] PERINGATAN: batas memori, upaya %s (%d baris G) dibuang" % (key[1], rows))
        while len(self.late) > MAX_HELD_ATTEMPTS:  # penanda tanpa baris G (mis. LOG_RAW 0)
            del self.late[next(iter(self.late))]
            self.lost_attempts += 1
            print("[logger] PERINGATAN: batas memori, satu penanda tertunda dibuang")

    def write_marker(self, m, late=False):
        """Tulis semua baris G upaya ini + baris M. Melempar OSError bila gagal (baris G tetap di
        memori). Mengembalikan (jumlah baris G, catatan); jumlah None = penanda ini sudah pernah
        tertulis (kiriman ulang), tidak ditulis lagi."""
        boot, seq, sesi, trial, upaya = m[3], m[2], m[4], m[5], m[6]
        exit_code, onset, end, w0, w1, ref, peak, demo, fault, n = m[7:17]
        if (boot, seq) in self.done:
            self.late.pop((boot, seq), None)
            return None, "sudah tertulis"
        key = (boot, upaya)
        samples = self.attempts.get(key, {"rows": []})["rows"]
        onset_ms = int(onset)
        rows = []
        for stamp, g in samples:
            ms = int(g[5])
            # Waktu nol grafik = onset (keputusan 10c); sebelum onset waktunya negatif.
            t_onset = "%.3f" % ((ms - onset_ms) / 1000.0) if onset_ms >= 0 else ""
            rows.append([stamp, boot, sesi, trial, upaya, "G", g[5], t_onset, g[6], g[7], g[8]]
                        + [""] * 10)
        notes = []
        if late:
            notes.append("ditulis terlambat")
        if int(n) != len(samples):  # baris G hilang di jalur USB atau logger.py mulai di tengah upaya
            notes.append("n=%s, diterima %d" % (n, len(samples)))
        note = "; ".join(notes)
        rows.append([now_stamp(), boot, sesi, trial, upaya, "M", "", "", "", "", demo,
                     EXIT_NAMES.get(exit_code, "KODE_" + exit_code), onset, end, w0, w1, ref, peak,
                     EVENT_NAMES.get(fault, "") if fault != "0" else "", n, note])
        self.log.write(rows)
        self.attempts.pop(key, None)
        self.late.pop((boot, seq), None)
        self.done.append((boot, seq))
        del self.done[:-RECENT_MAX]
        return len(samples), note

    def write_orphan(self, key):
        """Baris G yang penandanya tidak pernah diterima (ESP32 reset, logger.py mulai di tengah
        upaya) ditulis sendiri dengan catatan "tanpa penanda". Melempar OSError bila gagal."""
        entry = self.attempts[key]
        rows = [[stamp, g[3], "", "", g[4], "G", g[5], "", g[6], g[7], g[8]] + [""] * 9
                + ["tanpa penanda"] for stamp, g in entry["rows"]]
        self.log.write(rows)
        del self.attempts[key]
        return len(rows)

    def retry(self, final=False):
        """Satu putaran penulisan terlambat, paling sering tiap RETRY_S (selalu bila final):
        penanda tertunda beserta baris G-nya, lalu baris G tanpa penanda yang sudah ORPHAN_AFTER_S
        tidak bertambah. Satu baris peringatan per putaran yang gagal. final = saat Ctrl+C."""
        now = time.time()
        if not final and now < self.next_retry:
            return
        self.next_retry = now + RETRY_S
        failed_attempts, failed_rows, error = 0, 0, None
        for m in list(self.late.values()):
            try:
                count, note = self.write_marker(m, late=True)
                if count is not None:
                    self.late_written += 1
                    print("[logger] upaya %s ditulis terlambat: %d baris G + penanda%s" %
                          (m[6], count, " (" + note + ")" if note else ""))
            except OSError as err:
                failed_attempts += 1
                failed_rows += len(self.attempts.get((m[3], m[6]), {"rows": []})["rows"])
                error = err
        waiting = {(m[3], m[6]) for m in self.late.values()}
        for key in list(self.attempts):
            if key in waiting:
                continue  # baris G ini menunggu penandanya (sudah dihitung di atas)
            if not final and now - self.attempts[key]["last"] < ORPHAN_AFTER_S:
                continue  # upaya mungkin masih berjalan
            try:
                count = self.write_orphan(key)
                print("[logger] %d baris G upaya %s tanpa penanda ditulis" % (count, key[1]))
            except OSError as err:
                failed_attempts += 1
                failed_rows += len(self.attempts[key]["rows"])
                error = err
        if failed_attempts and not final:
            print("[logger] PERINGATAN: %d upaya (%d baris G) belum bisa ditulis (%s), dicoba lagi "
                  "tiap %d s" % (failed_attempts, failed_rows, error, RETRY_S))
        if failed_attempts and final:
            self.lost_attempts += failed_attempts
            self.lost_rows += failed_rows
            print("[logger] HILANG: %d upaya (%d baris G) tidak bisa ditulis (%s)" %
                  (failed_attempts, failed_rows, error))


def handle_hgd(line, port, logs, grips, recent, stats):
    fields, seq, error = parse_line(line)
    kind = fields[1] if fields else None
    if error:
        if seq == "0":  # baris G rusak: tidak ada ACK/NAK untuk baris G, cukup dibuang
            stats["g_bad"] += 1
            print("[logger] baris G rusak dibuang (%s)" % error)
            return
        stats["nak"] += 1
        if seq is not None:
            port.write(("NAK,%s\n" % seq).encode("ascii"))
            print("[logger] NAK,%s (%s): %s" % (seq, error, line))
        else:
            print("[logger] baris rusak tanpa seq, diabaikan (%s): %s" % (error, line))
        return
    if kind == "G":  # tanpa ACK: simpan di memori sampai penanda M upayanya datang
        grips.add_sample(fields)
        stats["g"] += 1
        return
    # ACK berarti "sudah tertulis" (Bagian 4.8): tulis + flush DULU, baru ACK.
    # Kunci duplikat = seluruh baris, termasuk nomor boot (langkah 10c).
    key = ",".join(fields)
    if key in recent:  # kiriman ulang karena ACK sebelumnya hilang: sudah tertulis, ACK lagi saja
        port.write(("ACK,%s\n" % seq).encode("ascii"))
        print("[logger] ACK,%s (duplikat, tidak ditulis ulang)" % seq)
        return
    if kind == "U":  # baris uji: tidak ditulis ke CSV
        port.write(("ACK,%s\n" % seq).encode("ascii"))
        stats["test"] += 1
        print("[logger] ACK,%s (baris uji U, tidak ditulis ke CSV)" % seq)
        return
    try:
        if kind in "TS":
            target = logs["daily"]
            row = daily_row(fields)
            target.write([row])
            shown = ",".join(row[1:])
        elif kind == "E":
            target = logs["fault"]
            row = fault_row(fields)
            target.write([row])
            shown = "galat %s %s" % (row[4], row[5])
        else:  # M
            target = logs["grip"]
            try:
                count, note = grips.write_marker(fields)
            except OSError:
                grips.hold_marker(fields)  # simpan untuk ditulis terlambat; tetap NAK di bawah
                raise
            if count is None:  # kiriman ulang penanda yang sudah ditulis terlambat
                port.write(("ACK,%s\n" % seq).encode("ascii"))
                print("[logger] ACK,%s (penanda sudah tertulis, tidak ditulis ulang)" % seq)
                return
            shown = "upaya %s: %d baris G + penanda (%s)%s" % (
                fields[6], count, EXIT_NAMES.get(fields[7], fields[7]), ", " + note if note else "")
            stats["m"] += 1
    except OSError as err:
        # Mis. PermissionError: berkas CSV sedang dibuka di Excel. Tidak tertulis -> NAK, jadi ESP32
        # mengirim ulang; untuk T/S, bila tetap gagal, ESP32 masuk GAGAL SIMPAN. logger.py tetap berjalan.
        stats["nak"] += 1
        port.write(("NAK,%s\n" % seq).encode("ascii"))
        print("[logger] NAK,%s: GAGAL MENULIS %s (%s). Tutup berkas itu (mis. di Excel)." %
              (seq, target.path(), err))
        return
    recent.append(key)
    del recent[:-RECENT_MAX]
    port.write(("ACK,%s\n" % seq).encode("ascii"))
    stats["written"] += 1
    print("[logger] ACK,%s  %s" % (seq, shown))


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

    logs = {
        "daily": CsvLog(args.dir, "hgd_", DAILY_HEADER),
        "fault": CsvLog(args.dir, "hgd_galat_", FAULT_HEADER),
        "grip": CsvLog(args.dir, "hgd_genggam_", GRIP_HEADER),
    }
    grips = GripBuffer(logs["grip"])
    recent = []
    stats = {"written": 0, "nak": 0, "test": 0, "g": 0, "g_bad": 0, "m": 0}
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
                        handle_hgd(line, port, logs, grips, recent, stats)
                    else:
                        print(line)  # teks debug ESP32: hanya ke konsol
            else:
                time.sleep(0.001)  # tidak ada data: istirahat 1 ms supaya CPU tidak 100 %
            forward_keys(port)
            grips.retry()  # rekaman tertunda (paling sering tiap RETRY_S)
            if to_send and time.time() - t_start > 3.0:  # --kirim: satu huruf per detik
                port.write(to_send[0].encode("ascii"))
                print("[logger] kirim '%s' ke ESP32 (--kirim)" % to_send[0])
                to_send = to_send[1:]
                t_start = time.time() - 2.0
    except KeyboardInterrupt:
        pass
    finally:
        port.close()
        grips.retry(final=True)  # percobaan terakhir menulis rekaman yang tertunda
        # Penanda yang ditulis terlambat tidak di-ACK (ESP32 sudah membuangnya), jadi tidak masuk
        # hitungan "baris ditulis"; dihitung terpisah di "penanda".
        print("[logger] berhenti: %d baris ditulis, %d NAK, %d baris uji, %d baris G (%d rusak), "
              "%d penanda (%d terlambat); rekaman hilang: %d upaya, %d baris G" %
              (stats["written"], stats["nak"], stats["test"], stats["g"], stats["g_bad"],
               stats["m"] + grips.late_written, grips.late_written,
               grips.lost_attempts, grips.lost_rows))


if __name__ == "__main__":
    main()

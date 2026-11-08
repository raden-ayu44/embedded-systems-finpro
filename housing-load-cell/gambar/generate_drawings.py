"""Gambar housing grip load cell, dibuat dari CSV parameter Fusion.

Cara pakai (dari folder repo):
    python housing-load-cell/gambar/generate_drawings.py

Skrip membaca housing-load-cell/fusion/fusion_parameters_housing_grip.csv dan menimpa enam SVG:
potongan memanjang, exploded isometrik, dua tampak klem kabel, tampak atas, dan setup kalibrasi
(kalibrasi/gambar/kalibrasi_setup.svg). Jalankan ulang setiap kali CSV berubah, lalu periksa
gambarnya di browser sebelum commit. Hanya memakai pustaka standar Python 3.

Yang tidak diambil dari CSV (asumsi; ubah konstantanya di bawah bila sudah diukur):
- kepala flange bolt M6 menurut DIN 6921: flange Ø14,2 tebal 1,5, heksagon kunci 10;
- lubang binokular dan penutup strain gauge load cell (skematik);
- balok PLA, papan, dumbbell, dan pemandu di setup kalibrasi (dumbbell skematik);
- jarak eksplode di gambar isometrik dan tata letak label.
"""
import csv, io, math, os, sys

# Folder repo: argumen pertama, atau dua tingkat di atas folder skrip ini.
REPO = sys.argv[1] if len(sys.argv) > 1 else os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

# Semua ukuran dalam mm, dibaca dari CSV (kolom Value). Titik asal di tengah load cell, muka bawah load cell di z 0.
rows = list(csv.DictReader(io.open(os.path.join(REPO, 'housing-load-cell', 'fusion', 'fusion_parameters_housing_grip.csv'), encoding='utf-8')))
P = {r['Name']: float(r['Value'].replace(' mm', '')) for r in rows}
LCL, LCW, LCH = P['LC_L'], P['LC_W'], P['LC_H']
HX, HY, GAP, BOSS = P['hole_x'], P['hole_y'], P['gap'], P['boss_L']
LIPT, LIPH, INW, ENDCL = P['sp_lip_t'], P['sp_lip_h'], P['sp_in_W'], P['sp_end_cl']
SPL, SPW = P['spacer_L'], P['spacer_W']
BARL, BARW, BART = P['bar_L'], P['bar_W'], P['bar_t']
CX, CZ, CD = P['cable_x'], P['cable_z'], P['cable_d']
WALLT, WALLZ = P['clamp_wall_t'], P['wall_bottom_z']
NW, NTOP, PADL, PADT = P['notch_w'], P['notch_top_z'], P['pad_L'], P['pad_t']
TUNW, TUNH, TUNF, TUNY = P['tun_w'], P['tun_h'], P['tun_floor'], P['tun_y']
TIEW, TIET = P['tie_w'], P['tie_t']
HSP, HD, BHE = P['hole_sp_d'], P['hole_d'], P['bar_hole_edge']
RL, RW, RH, RSP = P['ridge_L'], P['ridge_w'], P['ridge_h'], P['ridge_sp']
WSH, HEADP = P['washer_t'], P['head_protrusion']
MATT, SLOTL, SLOTW = P['mat_t'], P['mat_slot_L'], P['mat_slot_W']
TOT = P['total_t']
FL_D, FL_T, HEX_AF = 14.2, 1.5, 10.0          # kepala flange DIN 6921 M6 (asumsi, belum diukur)
HEXH = HEADP - WSH - FL_T                       # tinggi heksagon di atas flange
E0 = -LCL / 2
SA0, SA1 = E0 - ENDCL - LIPT, E0 + BOSS         # Spacer A arah x
SB0, SB1 = LCL / 2 - BOSS, LCL / 2 + ENDCL + LIPT
ZA0, ZA1 = LCH + GAP, LCH + GAP + BART          # pelat A
ZB1, ZB0 = -GAP, -GAP - BART                    # pelat B
XC = E0 + CX                                    # sumbu kabel
GZ = P['grip_zone']                             # zona genggam
fmt = lambda v: ('%g' % round(v, 2)).replace('.', ',')

STYLE = '''<style>
text{font-family:"Segoe UI",Arial,sans-serif;fill:#17212b}
.t1{font-size:19px;font-weight:700}.t2{font-size:12.5px;fill:#5b6774}.lb{font-size:12.5px}.sm{font-size:11px}.mu{fill:#5b6774}
.dm{font:12px Consolas,monospace;fill:#41505f}.warn{fill:#b42318}
.al{fill:#dfe3e8;stroke:#3d4a57;stroke-width:1.2}.alh{fill:url(#hAl);stroke:#3d4a57;stroke-width:1.2}
.lc{fill:#dbe9f6;stroke:#2c3e50;stroke-width:1.2}.lch{fill:url(#hLc);stroke:#2c3e50;stroke-width:1.2}
.pla{fill:#fbe6d8;stroke:#d9661f;stroke-width:1.3}.plah{fill:url(#hPla);stroke:#d9661f;stroke-width:1.3}
.bolt{fill:#c3ccd6;stroke:#2c3e50;stroke-width:1.1}.wsh{fill:#eef1f4;stroke:#2c3e50;stroke-width:1}
.cab{fill:#222}.cabs{fill:none;stroke:#222;stroke-linecap:round}.tie{fill:#2f7d5b}.ties{fill:none;stroke:#2f7d5b;stroke-width:3;stroke-linejoin:round}
.hid{fill:none;stroke:#5b6774;stroke-width:1;stroke-dasharray:5 4}.hidp{fill:none;stroke:#d9661f;stroke-width:1.1;stroke-dasharray:5 4}
.free{fill:none;stroke:#6b7785;stroke-width:1.2;stroke-dasharray:4 3}.void{fill:#ffffff;stroke:#3d4a57;stroke-width:1}
.dim{stroke:#5b6774;stroke-width:1;fill:none}.ext{stroke:#9aa7b4;stroke-width:.8}.ax{stroke:#d9661f;stroke-width:1;stroke-dasharray:1 4;stroke-linecap:round}
.ld{stroke:#5b6774;stroke-width:.9;fill:none}.co{fill:#ffffff;stroke:#17212b;stroke-width:1.2}.cot{font-size:11px;font-weight:700;text-anchor:middle}
.F{stroke:#b42318;stroke-width:2.4;fill:none}.Fh{fill:#b42318}.wood{fill:#ead7b0;stroke:#8a6d3b;stroke-width:1.2}.rub{fill:#5c5f63;stroke:#2b2d30;stroke-width:1}
.table{fill:#c9cdd2;stroke:#6b7785}.gz{fill:url(#grip);stroke:#3d8b37;stroke-width:1.5;opacity:.85}.gzt{fill:#2d6a27;font-size:14px;font-weight:700}.lcdash{fill:none;stroke:#c0392b;stroke-width:1.2;stroke-dasharray:8 5}.slot{fill:none;stroke:#d35400;stroke-width:1.6;stroke-dasharray:2 3}.dmg{font:12px Consolas,monospace;fill:#3d8b37}.dmr{font:12px Consolas,monospace;fill:#c0392b}.db{fill:#4a5562;stroke:#17212b;stroke-width:1.2}.wbox{fill:#fdf2e9;stroke:#d35400}.sticker{fill:#f2c94c;stroke:#8a6d3b}
</style>
<defs>
<marker id="ar" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse"><path d="M0 0L10 5L0 10z" fill="#5b6774"/></marker>
<pattern id="hAl" width="7" height="7" patternUnits="userSpaceOnUse" patternTransform="rotate(45)"><rect width="7" height="7" fill="#dfe3e8"/><line x1="0" y1="0" x2="0" y2="7" stroke="#8792a0" stroke-width="1"/></pattern>
<pattern id="hLc" width="7" height="7" patternUnits="userSpaceOnUse" patternTransform="rotate(-45)"><rect width="7" height="7" fill="#dbe9f6"/><line x1="0" y1="0" x2="0" y2="7" stroke="#7a9bbb" stroke-width="1"/></pattern>
<pattern id="grip" width="8" height="8" patternUnits="userSpaceOnUse" patternTransform="rotate(45)"><rect width="8" height="8" fill="#e6f2e3"/><line x1="0" y1="0" x2="0" y2="8" stroke="#9ccc94" stroke-width="2"/></pattern>
<pattern id="hPla" width="6" height="6" patternUnits="userSpaceOnUse" patternTransform="rotate(45)"><rect width="6" height="6" fill="#fbe6d8"/><line x1="0" y1="0" x2="0" y2="6" stroke="#d9661f" stroke-width="1"/></pattern>
</defs>'''

class Canvas:
    """Kanvas dengan pemetaan mm -> px (sumbu tegak naik)."""
    def __init__(s, W, H, sc, ox, oy):
        s.W, s.H, s.sc, s.ox, s.oy, s.o = W, H, sc, ox, oy, []
    def X(s, x): return s.ox + x * s.sc
    def Y(s, z): return s.oy - z * s.sc
    def add(s, t): s.o.append(t)
    def rect(s, x0, z0, x1, z1, c, extra=''):
        x0, x1 = min(x0, x1), max(x0, x1); z0, z1 = min(z0, z1), max(z0, z1)
        s.add('<rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" class="%s"%s/>' % (s.X(x0), s.Y(z1), (x1 - x0) * s.sc, (z1 - z0) * s.sc, c, extra))
    def poly(s, pts, c):
        s.add('<path d="M%s Z" class="%s"/>' % (' L'.join('%.1f,%.1f' % (s.X(x), s.Y(z)) for x, z in pts), c))
    def circ(s, x, z, r, c):
        s.add('<circle cx="%.1f" cy="%.1f" r="%.1f" class="%s"/>' % (s.X(x), s.Y(z), r * s.sc, c))
    def line(s, x0, z0, x1, z1, c):
        s.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="%s"/>' % (s.X(x0), s.Y(z0), s.X(x1), s.Y(z1), c))
    def text(s, px, py, t, c='lb', a='start'):
        s.add('<text x="%.1f" y="%.1f" class="%s" text-anchor="%s">%s</text>' % (px, py, c, a, t))
    def hdim(s, x0, x1, py, t, ext_from=None, below=False):
        for x in (x0, x1):
            if ext_from is not None: s.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="ext"/>' % (s.X(x), ext_from, s.X(x), py + (4 if py > ext_from else -4)))
        s.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="dim" marker-start="url(#ar)" marker-end="url(#ar)"/>' % (s.X(x0), py, s.X(x1), py))
        s.text((s.X(x0) + s.X(x1)) / 2, py + (14 if below else -5), t, 'dm', 'middle')
    def vdim(s, z0, z1, px, t, ext_from=None, left=False):
        for z in (z0, z1):
            if ext_from is not None: s.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="ext"/>' % (ext_from, s.Y(z), px + (4 if px > ext_from else -4), s.Y(z)))
        s.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="dim" marker-start="url(#ar)" marker-end="url(#ar)"/>' % (px, s.Y(z0), px, s.Y(z1)))
        s.text(px + (-6 if left else 6), (s.Y(z0) + s.Y(z1)) / 2 + 4, t, 'dm', 'end' if left else 'start')
    def call(s, n, px, py, tx=None, ty=None):
        """Nomor callout di (tx, ty) dengan garis ke titik (px, py) dalam px."""
        if tx is None: tx, ty = px, py
        else: s.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="ld"/><circle cx="%.1f" cy="%.1f" r="2" fill="#17212b"/>' % (px, py, tx, ty, px, py))
        s.add('<circle cx="%.1f" cy="%.1f" r="10" class="co"/><text x="%.1f" y="%.1f" class="cot">%d</text>' % (tx, ty, tx, ty + 4, n))
    def svg(s, title, sub, label):
        head = ('<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" viewBox="0 0 %d %d" role="img" aria-label="%s">' % (s.W, s.H, s.W, s.H, label) + STYLE +
                '<rect width="%d" height="%d" fill="#ffffff"/>' % (s.W, s.H) +
                '<text x="24" y="34" class="t1">%s</text><text x="24" y="54" class="t2">%s</text>' % (title, sub))
        return head + '\n'.join(s.o) + '</svg>\n'

def draw_legend(d, x, y, items, cols=2, colw=520, lh=19):
    per = math.ceil(len(items) / cols)
    for i, (n, t) in enumerate(items):
        cx, cy = x + (i // per) * colw, y + (i % per) * lh
        if n != '': d.add('<circle cx="%.1f" cy="%.1f" r="8.5" class="co"/><text x="%.1f" y="%.1f" class="cot" style="font-size:10px">%s</text>' % (cx, cy - 4, cx, cy - 0.5, n))
        d.text(cx + 15, cy, t, 'sm')

def hexagon_side(d, x, z0, z1, af):
    """Kepala heksagon dilihat dari samping (lebar sudut ke sudut)."""
    w = af / math.cos(math.pi / 6)
    d.rect(x - w / 2, z0, x + w / 2, z1, 'bolt')
    d.line(x - af / 2 / math.cos(math.pi / 6) / 2, z0, x - af / 2 / math.cos(math.pi / 6) / 2, z1, 'ld')
    d.line(x + af / 2 / math.cos(math.pi / 6) / 2, z0, x + af / 2 / math.cos(math.pi / 6) / 2, z1, 'ld')

def bolt_side(d, x, zface, up, cut=False):
    """Flange bolt + ring dari muka luar pelat. up=True: kepala di atas (z naik)."""
    sg = 1 if up else -1
    zw = zface + sg * WSH
    d.rect(x - 6, zface, x + 6, zw, 'wsh')
    d.rect(x - FL_D / 2, zw, x + FL_D / 2, zw + sg * FL_T, 'bolt')
    hexagon_side(d, x, zw + sg * FL_T, zw + sg * (FL_T + HEXH), HEX_AF)
    tip = zw - sg * P['bolt_L']
    d.rect(x - 3, zw, x + 3, tip, 'bolt')
    return tip

# ------------------------------------------------------------------ 1. potongan memanjang
def section_view():
    sc = 5.0
    d = Canvas(1180, 880, sc, 560, 330)
    # pelat A dan B (potongan di baris lubang y = -7,5), dengan lubang
    for (z0, z1, hx) in ((ZA0, ZA1, -HX), (ZB0, ZB1, HX)):
        d.rect(-BARL / 2, z0, hx - HD / 2, z1, 'alh'); d.rect(hx + HD / 2, z0, BARL / 2, z1, 'alh')
    # load cell, terpotong di sumbu lubang: lubang ulir tembus
    for x0, x1 in ((E0, -HX - 2.5), (-HX + 2.5, HX - 2.5), (HX + 2.5, -E0)):
        d.rect(x0, 0, x1, LCH, 'lch')
    for hx in (-HX, HX):
        d.rect(hx - 2.5, 0, hx + 2.5, LCH, 'void')
        for z in range(1, int(LCH)):
            d.line(hx - 3, z, hx - 2.5, z + 0.4, 'ld'); d.line(hx + 2.5, z, hx + 3, z + 0.4, 'ld')
    # lubang binokular (skematik) dan penutup strain gauge
    for cx in (-11, 11):
        d.circ(cx, LCH / 2, 6, 'void')
    d.rect(-11, LCH / 2 - 2, 11, LCH / 2 + 2, 'void'); d.line(-11, LCH / 2 - 2, 11, LCH / 2 - 2, 'lc'); d.line(-11, LCH / 2 + 2, 11, LCH / 2 + 2, 'lc')
    d.add('<rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" fill="#ffffff" stroke="none"/>' % (d.X(-5.2), d.Y(LCH / 2 + 2) + 0.5, 10.4 * sc, 4 * sc - 1))
    d.rect(-15, LCH, 15, LCH + 1.0, 'hid')
    d.rect(-15, -1.0, 15, 0, 'hid')
    # Spacer A dan B (potongan)
    d.rect(SA0, LCH, -HX - HSP / 2, ZA0, 'plah'); d.rect(-HX + HSP / 2, LCH, SA1, ZA0, 'plah')
    d.rect(SA0, LCH - LIPH, E0 - ENDCL, LCH, 'plah')
    d.rect(SB0, ZB1, HX - HSP / 2, 0, 'plah'); d.rect(HX + HSP / 2, ZB1, SB1, 0, 'plah')
    d.rect(-E0 + ENDCL, 0, SB1, LIPH, 'plah')
    # dinding klem dan bantalan di depan bidang potong (putus-putus)
    nx0, nx1 = XC - NW / 2, XC + NW / 2
    d.add('<path d="M%.1f,%.1f L%.1f,%.1f L%.1f,%.1f L%.1f,%.1f A%.1f,%.1f 0 0 1 %.1f,%.1f L%.1f,%.1f L%.1f,%.1f L%.1f,%.1f Z" class="hidp"/>' % (
        d.X(SA0), d.Y(LCH), d.X(SA0), d.Y(WALLZ), d.X(nx0), d.Y(WALLZ), d.X(nx0), d.Y(NTOP - NW / 2), NW / 2 * sc, NW / 2 * sc, d.X(nx1), d.Y(NTOP - NW / 2),
        d.X(nx1), d.Y(WALLZ), d.X(SA1), d.Y(WALLZ), d.X(SA1), d.Y(LCH)))
    d.rect(XC - PADL / 2, NTOP, XC + PADL / 2, ZA0, 'hidp')
    d.circ(XC, CZ, CD / 2, 'cab'); d.add('<circle cx="%.1f" cy="%.1f" r="2.2" fill="#ffffff"/>' % (d.X(XC), d.Y(CZ)))
    # baut
    tA = bolt_side(d, -HX, ZA1, True); tB = bolt_side(d, HX, ZB0, False)
    # ridge (bebas)
    for x0 in (-RSP / 2 - RW, RSP / 2):
        d.rect(x0, ZB0, x0 + RW, ZB0 - RH, 'free')
    # gaya
    for x in (-20, 0, 20):
        d.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="F"/><path d="M%.1f,%.1f l-5,-10 l10,0 z" class="Fh"/>' % (d.X(x), d.Y(ZA1) - 46, d.X(x), d.Y(ZA1) - 10, d.X(x), d.Y(ZA1) - 2))
        d.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="F"/><path d="M%.1f,%.1f l-5,10 l10,0 z" class="Fh"/>' % (d.X(x), d.Y(ZB0 - RH) + 46, d.X(x), d.Y(ZB0 - RH) + 10, d.X(x), d.Y(ZB0 - RH) + 2))
    d.text(d.X(26), d.Y(ZB0 - RH) + 36, 'jari menekan B (F)', 'lb warn')
    # ukuran atas
    yA = d.Y(ZA1)
    d.hdim(-BARL / 2, BARL / 2, 92, 'pelat A dan B ' + fmt(BARL) + ' × ' + fmt(BARW) + ' × ' + fmt(BART) + ' (Al 6061-T6)', yA)
    d.hdim(E0, -E0, 118, 'load cell ' + fmt(LCL) + ' × ' + fmt(LCW) + ' × ' + fmt(LCH), d.Y(LCH))
    d.hdim(-P['grip_zone'] / 2, P['grip_zone'] / 2, 144, 'zona genggam ' + fmt(P['grip_zone']), yA)
    # ukuran bawah
    yb = d.Y(ZB0 - RH) + 70
    d.hdim(E0, -HX, yb, fmt(P['hole_edge']), d.Y(0), below=True); d.hdim(-HX, HX, yb, fmt(P['hole_pitch']) + ' antar pasangan lubang', d.Y(0), below=True); d.hdim(HX, -E0, yb, fmt(P['hole_edge']), d.Y(0), below=True)
    d.hdim(E0, SA1, yb + 30, 'spacer ' + fmt(BOSS), None, below=True); d.hdim(SA1, SB0, yb + 30, 'bentang bebas ' + fmt(SB0 - SA1), None, below=True); d.hdim(SB0, -E0, yb + 30, fmt(BOSS), None, below=True)
    d.hdim(SA1, 0, yb + 60, 'a = ' + fmt(-SA1), None, below=True); d.hdim(SA1, -E0, yb + 90, 'x = ' + fmt(-E0 - SA1) + ' (ujung jauh load cell dari tepi dalam Spacer A)', None, below=True)
    d.hdim(-BARL / 2, -HX, yb + 120, fmt(BHE), d.Y(ZB0), below=True); d.hdim(HX, BARL / 2, yb + 120, fmt(BHE), d.Y(ZB0), below=True)
    # ukuran tegak di kanan
    px = d.X(BARL / 2) + 30
    d.vdim(ZA0, ZA1, px, 'A ' + fmt(BART), d.X(BARL / 2)); d.vdim(LCH, ZA0, px, 'celah ' + fmt(GAP), d.X(SB1))
    d.vdim(0, LCH, px, 'LC ' + fmt(LCH), d.X(-E0)); d.vdim(ZB1, 0, px, 'celah ' + fmt(GAP), d.X(-E0)); d.vdim(ZB0, ZB1, px, 'B ' + fmt(BART), d.X(BARL / 2))
    d.vdim(ZB0 - RH, ZB0, px, 'ridge ' + fmt(RH) + ' (bebas)', d.X(RSP / 2 + RW))
    d.vdim(ZB0 - RH, ZA1, px + 105, 'total ' + fmt(TOT) + ' (tanpa kepala baut)', d.X(BARL / 2) + 4)
    d.vdim(ZB1, WALLZ, d.X(SA0) - 26, fmt(WALLZ + GAP), d.X(SA0), left=True)
    # callout
    d.call(1, d.X(-30), d.Y(ZA1 - BART / 2)); d.call(2, d.X(-30), d.Y(ZB0 + BART / 2)); d.call(3, d.X(30), d.Y(LCH * 0.3))
    d.call(4, d.X(-45), d.Y(ZA0 - GAP / 2), d.X(-38), d.Y(ZA0) - 40); d.call(5, d.X(45), d.Y(-GAP / 2), d.X(38), d.Y(ZB0) + 34)
    d.call(6, d.X(-HX + 7), d.Y(ZA1 + HEADP - 2), d.X(-HX + 22), d.Y(ZA1 + HEADP) - 4); d.call(7, d.X(HX + 7), d.Y(ZB0 - HEADP + 2), d.X(HX + 22), d.Y(ZB0 - HEADP) + 6)
    d.call(8, d.X(30), d.Y(LCH + GAP / 2), d.X(48), d.Y(ZA1) - 22); d.call(9, d.X(11), d.Y(LCH / 2), d.X(0), d.Y(LCH / 2) + 0)
    d.call(10, d.X(-RSP / 2 - RW / 2), d.Y(ZB0 - RH), d.X(-34), d.Y(ZB0 - RH) + 30); d.call(11, d.X(0), d.Y(ZB0 - 0.5), d.X(-6), d.Y(ZB0 - RH) + 20)
    d.call(12, d.X(XC - 2), d.Y(CZ - 1), d.X(XC - 32), d.Y(CZ - 6)); d.call(13, d.X(XC - PADL / 2), d.Y(NTOP + 4), d.X(SA0 - 26), d.Y(NTOP + 8))
    d.call(15, d.X(-BARL / 2 + 1), d.Y((ZB1 + ZA0) / 2), d.X(-BARL / 2) - 24, d.Y((ZB1 + ZA0) / 2) + 26); d.call(16, d.X(-E0 - 20), d.Y(LCH * 0.7))
    d.call(17, d.X(-12), d.Y(LCH + 0.5), d.X(-24), d.Y(LCH + GAP) - 24); d.call(18, d.X(-HX - HD / 2), d.Y(ZA0 + BART * 0.5), d.X(-HX - 26), d.Y(ZA1) - 36)
    d.text(d.X(-BARL / 2), d.Y(ZB0 - RH) + 44, '◀ ujung tetap (sisi kabel)', 'sm mu')
    d.text(d.X(BARL / 2), d.Y(ZB0 - RH) + 44, 'ujung bebas ▶', 'sm mu', 'end')
    draw_legend(d, 34, 690, [
        (1, 'Pelat A — telapak, tetap; Al 6061-T6 ' + fmt(BARL) + ' × ' + fmt(BARW) + ' × ' + fmt(BART)), (2, 'Pelat B — jari, bergerak; sama dengan A'),
        (3, 'Load cell CZL 601 80 kg, ' + fmt(LCL) + ' × ' + fmt(LCW) + ' × ' + fmt(LCH)), (4, 'Spacer A — PLA tebal ' + fmt(GAP) + ', kontak ' + fmt(BOSS) + ' × ' + fmt(LCW) + ' (ujung tetap)'),
        (5, 'Spacer B — PLA, sama, tanpa dinding klem (ujung bebas)'), (6, 'Flange bolt M6 × 25 A2-70 + ring ' + fmt(WSH) + ' (sisi A)'),
        (7, 'Flange bolt M6 × 25 A2-70 + ring (sisi B)'), (8, 'Celah bebas-sentuh ' + fmt(GAP) + ' = tebal spacer'),
        (9, 'Area elemen ukur (lubang binokular, skematik)'), (10, 'Ridge PLA (bentuk bebas; ' + fmt(RL) + ' × ' + fmt(RW) + ' × ' + fmt(RH) + ', jarak ' + fmt(RSP) + ')'),
        (11, 'Bidang tumpu kalibrasi = permukaan pelat B'), (12, 'Kabel Ø' + fmt(CD) + ' di depan bidang potong (lubang Ø5)'),
        (13, 'Klem: takik ' + fmt(NW) + ' + bantalan zip tie (putus-putus, di depan)'), (15, 'Celah ujung A–B, tanpa sambungan'),
        (16, 'Load cell = satu-satunya jalur gaya A → B'), (17, 'Penutup strain gauge, tinggi 1,0 (asumsi, belum diukur)'),
        (18, 'Lubang pelat Ø' + fmt(HD) + ' / spacer Ø' + fmt(HSP) + ' (model)'), ('—', 'Panah merah: telapak menekan A dari atas, jari menekan B dari bawah (F)'), ('—', 'Baut tidak diarsir; ujung baut ' + fmt(LCH - (P['bolt_L'] - WSH - BART - GAP)) + ' dari muka seberang')])
    return d.svg('Potongan Memanjang v16 — pelat 140 × 40 × 8, spacer ' + fmt(GAP),
                 'dihasilkan dari CSV parameter · skala 1 mm = 5 px · potongan di baris lubang y = −7,5, dilihat dari sisi kabel · A (telapak) di atas, B (jari) di bawah',
                 'Potongan memanjang housing grip v16')

# ------------------------------------------------------------------ 2. exploded isometrik
C30, S30 = math.cos(math.pi / 6), math.sin(math.pi / 6)
class Iso:
    def __init__(s, sc, ox, oy): s.sc, s.ox, s.oy, s.o = sc, ox, oy, []
    def P(s, x, y, z):  # pengamat di +x, -y, +z: tampak muka atas, muka -y (sisi kabel), muka +x
        return (s.ox + (x + y) * C30 * s.sc, s.oy + ((x - y) * S30 - z) * s.sc)
    def face(s, pts, c):
        s.o.append('<path d="M%s Z" class="%s"/>' % (' L'.join('%.1f,%.1f' % s.P(*p) for p in pts), c))
    def box(s, x0, x1, y0, y1, z0, z1, c):
        s.face([(x0, y0, z0), (x1, y0, z0), (x1, y0, z1), (x0, y0, z1)], c)
        s.face([(x1, y0, z0), (x1, y1, z0), (x1, y1, z1), (x1, y0, z1)], c)
        s.face([(x0, y0, z1), (x1, y0, z1), (x1, y1, z1), (x0, y1, z1)], c)
    def ell(s, x, y, z, r, c):
        cx, cy = s.P(x, y, z)
        s.o.append('<ellipse cx="%.1f" cy="%.1f" rx="%.1f" ry="%.1f" class="%s"/>' % (cx, cy, r * math.sqrt(2) * C30 * s.sc, r * math.sqrt(2) * S30 * s.sc, c))
    def cyl(s, x, y, z0, z1, r, c):
        a, b = s.P(x, y, z0), s.P(x, y, z1)
        rx, ry = r * math.sqrt(2) * C30 * s.sc, r * math.sqrt(2) * S30 * s.sc
        lo, hi = (a, b) if a[1] > b[1] else (b, a)
        s.o.append('<path d="M%.1f,%.1f L%.1f,%.1f A%.1f,%.1f 0 0 0 %.1f,%.1f L%.1f,%.1f A%.1f,%.1f 0 0 1 %.1f,%.1f Z" class="%s"/>' % (
            hi[0] - rx, hi[1], lo[0] - rx, lo[1], rx, ry, lo[0] + rx, lo[1], hi[0] + rx, hi[1], rx, ry, hi[0] - rx, hi[1], c))
        s.o.append('<ellipse cx="%.1f" cy="%.1f" rx="%.1f" ry="%.1f" class="%s"/>' % (hi[0], hi[1], rx, ry, c))
    def line(s, p, q, c):
        a, b = s.P(*p), s.P(*q)
        s.o.append('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="%s"/>' % (a[0], a[1], b[0], b[1], c))

def exploded_view():
    d = Canvas(1180, 1100, 1, 0, 0)
    iso = Iso(2.55, 330, 600)
    EA, ESA, ESB, EB = 60, 30, -30, -60          # jarak eksplode (mm, arah z)
    EBA, EBB = 110, -110
    def bolt_iso(x, y, zb, up):
        sg = 1 if up else -1
        zw = zb + sg * WSH; zf = zw + sg * FL_T; zh = zf + sg * HEXH; tip = zw - sg * P['bolt_L']
        if up:
            iso.cyl(x, y, tip, zw, 3, 'bolt'); iso.cyl(x, y, zb, zw, 6, 'wsh'); iso.cyl(x, y, zw, zf, FL_D / 2, 'bolt'); iso.cyl(x, y, zf, zh, HEX_AF / 2 / C30, 'bolt')
        else:
            iso.cyl(x, y, zh, zf, HEX_AF / 2 / C30, 'bolt'); iso.cyl(x, y, zf, zw, FL_D / 2, 'bolt'); iso.cyl(x, y, zw, zb, 6, 'wsh'); iso.cyl(x, y, zw, tip, 3, 'bolt')
    # sumbu lubang (garis titik)
    for x, z0, z1 in ((-HX, ZB0 + EB, ZA1 + EBA + HEADP), (HX, ZB0 + EBB - HEADP, ZA1 + EA)):
        for y in (-HY, HY):
            iso.line((x, y, z0), (x, y, z1), 'ax')
    # dari bawah ke atas
    for y in (HY, -HY): bolt_iso(HX, y, ZB0 + EBB, False)
    for x0 in (-RSP / 2 - RW, RSP / 2):
        iso.box(x0, x0 + RW, -RL / 2, RL / 2, ZB0 + EB - RH, ZB0 + EB, 'free')
    iso.box(-BARL / 2, BARL / 2, -BARW / 2, BARW / 2, ZB0 + EB, ZB1 + EB, 'al')
    for y in (-HY, HY): iso.ell(HX, y, ZB1 + EB, HD / 2, 'void')
    # Spacer B
    z0, z1 = ZB1 + ESB, ESB
    iso.box(SB0, SB1, -SPW / 2, SPW / 2, z0, z1, 'pla')
    iso.box(SB0, SB1, -SPW / 2, -INW / 2, z1, z1 + LIPH, 'pla'); iso.box(-E0 + ENDCL, SB1, -SPW / 2, SPW / 2, z1, z1 + LIPH, 'pla')
    iso.box(SB0, SB1, INW / 2, SPW / 2, z1, z1 + LIPH, 'pla')
    for y in (-HY, HY): iso.ell(HX, y, z1, HSP / 2 * 0.87, 'void')
    # load cell
    iso.box(E0, -E0, -LCW / 2, LCW / 2, 0, LCH, 'lc')
    iso.box(-15, 15, -8, 8, LCH, LCH + 1.0, 'hid')
    for x in (-HX, HX):
        for y in (-HY, HY): iso.ell(x, y, LCH, 2.5, 'void')
    for cx in (-11, 11):
        p = iso.P(cx, -LCW / 2, LCH / 2); iso.o.append('<ellipse cx="%.1f" cy="%.1f" rx="%.1f" ry="%.1f" class="void"/>' % (p[0], p[1], 6 * C30 * iso.sc, 6 * iso.sc))
    a, b = iso.P(-11, -LCW / 2, LCH / 2 + 2), iso.P(11, -LCW / 2, LCH / 2 - 2)
    iso.face([(-11, -LCW / 2, LCH / 2 - 2), (11, -LCW / 2, LCH / 2 - 2), (11, -LCW / 2, LCH / 2 + 2), (-11, -LCW / 2, LCH / 2 + 2)], 'void')
    # stiker panah beban di muka ujung bebas, menunjuk ke arah B
    iso.face([(-E0, -6, 6), (-E0, 6, 6), (-E0, 6, 17), (-E0, -6, 17)], 'sticker')
    p, q = iso.P(-E0, 0, 15.5), iso.P(-E0, 0, 8)
    iso.o.append('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" stroke="#17212b" stroke-width="2" marker-end="url(#ar)"/>' % (p[0], p[1], q[0], q[1]))
    # kabel: keluar menyamping (-y) lewat takik, lalu turun ke arah B
    pts = [iso.P(XC, -LCW / 2, CZ), iso.P(XC, -SPW / 2 - PADT - 4, CZ), iso.P(XC, -SPW / 2 - PADT - 14, CZ - 10), iso.P(XC, -SPW / 2 - PADT - 16, CZ - 60)]
    iso.o.append('<path d="M%.1f,%.1f L%.1f,%.1f Q%.1f,%.1f %.1f,%.1f" class="cabs" stroke-width="%.1f"/>' % (pts[0] + pts[1] + pts[2] + pts[3] + (CD * iso.sc * 0.9,)))
    # Spacer A (di atas load cell, dieksplode ke atas)
    z0, z1 = LCH + ESA, ZA0 + ESA
    iso.box(SA0, SA1, -SPW / 2, -INW / 2, WALLZ + ESA, z0, 'pla')   # dinding klem
    nx0 = XC - NW / 2
    iso.face([(nx0, -SPW / 2, WALLZ + ESA), (nx0 + NW, -SPW / 2, WALLZ + ESA), (nx0 + NW, -SPW / 2, NTOP - NW / 2 + ESA), (XC, -SPW / 2, NTOP + ESA), (nx0, -SPW / 2, NTOP - NW / 2 + ESA)], 'void')
    iso.box(SA0, E0 - ENDCL, -SPW / 2, SPW / 2, LCH - LIPH + ESA, z0, 'pla'); iso.box(SA0, SA1, INW / 2, SPW / 2, LCH - LIPH + ESA, z0, 'pla')
    iso.box(XC - PADL / 2, XC + PADL / 2, -SPW / 2 - PADT, -SPW / 2, NTOP + ESA, z1, 'pla')
    iso.face([(XC + PADL / 2, -SPW / 2 - TUNY - TUNW / 2, NTOP + TUNF + ESA), (XC + PADL / 2, -SPW / 2 - TUNY + TUNW / 2, NTOP + TUNF + ESA),
              (XC + PADL / 2, -SPW / 2 - TUNY + TUNW / 2, NTOP + TUNF + TUNH + ESA), (XC + PADL / 2, -SPW / 2 - TUNY - TUNW / 2, NTOP + TUNF + TUNH + ESA)], 'void')
    iso.box(SA0, SA1, -SPW / 2, SPW / 2, z0, z1, 'pla')
    for y in (-HY, HY): iso.ell(-HX, y, z1, HSP / 2 * 0.87, 'void')
    # pelat A dan baut A
    iso.box(-BARL / 2, BARL / 2, -BARW / 2, BARW / 2, ZA0 + EA, ZA1 + EA, 'al')
    for y in (-HY, HY): iso.ell(-HX, y, ZA1 + EA, HD / 2, 'void')
    for y in (HY, -HY): bolt_iso(-HX, y, ZA1 + EBA, True)
    # gaya
    for (x, z, up) in ((0, ZA1 + EA, True), (0, ZB0 + EB - RH, False)):
        a = iso.P(x, -BARW / 2 - 4, z + (40 if up else -40)); b = iso.P(x, -BARW / 2 - 4, z + (4 if up else -4))
        iso.o.append('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="F"/><path d="M%.1f,%.1f l-5,%d l10,0 z" class="Fh"/>' % (a[0], a[1], b[0], b[1] - (8 if up else -8), b[0], b[1], -10 if up else 10))
    d.o.extend(iso.o)
    # callout (dari titik pada benda ke label di kiri atau kanan)
    def callout_iso(n, pt, side, dy=0):
        px, py = iso.P(*pt)
        tx = 60 if side == 'L' else 720
        d.call(n, px, py, tx, py + dy)
    callout_iso(1, (40, -BARW / 2, ZA1 + EA - 2), 'R'); callout_iso(6, (-HX, -HY - 7, ZA1 + EBA + HEADP - 1), 'L'); callout_iso(18, (-HX + 3, HY, ZA1 + EA), 'R', -14)
    callout_iso(4, (SA1 - 4, -SPW / 2, ZA0 + ESA - 1), 'R'); callout_iso(13, (XC, -SPW / 2 - PADT, NTOP + ESA + 6), 'L'); callout_iso(12, (XC, -SPW / 2 - PADT - 15, CZ - 30), 'L')
    callout_iso(14, (XC, -SPW / 2 - PADT - 16, CZ - 55), 'L', 10)
    callout_iso(3, (20, -LCW / 2, 4), 'R'); callout_iso(9, (11, -LCW / 2, LCH / 2), 'R', 8); callout_iso(17, (8, -8, LCH + 1), 'R', -20); callout_iso(16, (-30, -LCW / 2, 14), 'L'); callout_iso(19, (-E0, 0, 12), 'R', 6)
    callout_iso(5, (SB1 - 3, -SPW / 2, ESB - 1), 'R'); callout_iso(2, (-40, -BARW / 2, ZB0 + EB + 2), 'L'); callout_iso(10, (-RSP / 2 - RW, -RL / 2, ZB0 + EB - RH), 'L', 12)
    callout_iso(11, (0, -BARW / 2 + 6, ZB0 + EB), 'L', 30); callout_iso(7, (HX, -HY - 7, ZB0 + EBB - HEADP + 1), 'R')
    d.text(380, 1080, 'telapak menekan A (F) dari atas · jari menekan B (F) dari bawah', 'lb warn', 'middle')
    draw_legend(d, 790, 110, [
        (1, 'Pelat A — telapak, tetap'), ('', 'Al 6061-T6 ' + fmt(BARL) + ' × ' + fmt(BARW) + ' × ' + fmt(BART) + ', 2 lubang Ø' + fmt(HD)), (2, 'Pelat B — jari, bergerak (sama)'),
        (3, 'Load cell CZL 601 80 kg'), ('', fmt(LCL) + ' × ' + fmt(LCW) + ' × ' + fmt(LCH) + ', 4 lubang M6 tembus'), (4, 'Spacer A — PLA tebal ' + fmt(GAP)),
        ('', 'luar ' + fmt(SPL) + ' × ' + fmt(SPW) + ', lubang Ø' + fmt(HSP) + ' (model)'), (5, 'Spacer B — PLA, tanpa dinding klem'), (6, 'Flange bolt M6 × 25 A2-70 + ring'),
        ('', '(dari atas, ke ujung tetap)'), (7, 'Flange bolt M6 × 25 + ring'), ('', '(dari bawah, ke ujung bebas)'), (9, 'Area elemen ukur (binokular)'),
        (10, 'Ridge PLA (bentuk bebas)'), (11, 'Bidang tumpu = permukaan pelat B'), (12, 'Kabel Ø' + fmt(CD) + ' keluar menyamping'),
        (13, 'Klem: takik ' + fmt(NW) + ' + bantalan'), ('', 'dengan terowongan zip tie 2,5 × 100'), (14, 'Kabel menuju housing elektronik,'), ('', 'ke arah pelat B (menjauhi telapak)'),
        (16, 'Load cell = satu-satunya'), ('', 'penghubung A ↔ B'), (17, 'Penutup strain gauge (tinggi'), ('', '1,0 asumsi, belum diukur)'),
        (18, 'Lubang pelat Ø' + fmt(HD) + ', hanya satu ujung'), (19, 'Stiker panah beban, ke arah B')], cols=1, lh=21)
    d.text(790, 700, 'Tebal total ' + fmt(TOT) + ' = ' + fmt(BART) + ' + ' + fmt(GAP) + ' + ' + fmt(LCH) + ' + ' + fmt(GAP) + ' + ' + fmt(BART) + ' + ridge ' + fmt(RH), 'sm')
    d.text(790, 720, 'Jarak antar bagian = jarak eksplode, bukan ukuran.', 'sm mu')
    d.text(790, 740, 'Kepala flange diasumsikan DIN 6921 (Ø14,2, kunci 10).', 'sm mu')
    d.text(790, 760, 'Stop overload belum dirancang.', 'sm mu')
    return d.svg('Exploded Isometrik v16 — pelat 140 × 40 × 8, spacer ' + fmt(GAP),
                 'dihasilkan dari CSV parameter · proyeksi isometrik berskala (2,55 px/mm), jarak eksplode tidak berskala · urutan dari atas: baut A → pelat A → Spacer A → load cell → Spacer B → pelat B → baut B',
                 'Exploded isometrik housing grip v16')

# ------------------------------------------------------------------ 3. klem kabel tampak depan
def clamp_front_view():
    sc = 10.0
    d = Canvas(1000, 840, sc, 870, 500)          # x: -72 .. -30 ; z: -14 .. 36
    xr = -33
    # load cell (paling belakang, y = -14)
    d.rect(E0, 0, xr, LCH, 'lc')
    d.circ(XC, CZ, 2.5, 'void')
    # dinding klem (y = -17) dengan takik
    nx0, nx1 = XC - NW / 2, XC + NW / 2
    d.add('<path d="M%.1f,%.1f L%.1f,%.1f L%.1f,%.1f L%.1f,%.1f A%.1f,%.1f 0 0 1 %.1f,%.1f L%.1f,%.1f L%.1f,%.1f L%.1f,%.1f Z" class="pla"/>' % (
        d.X(SA0), d.Y(ZA0), d.X(SA0), d.Y(WALLZ), d.X(nx0), d.Y(WALLZ), d.X(nx0), d.Y(NTOP - NW / 2), NW / 2 * sc, NW / 2 * sc, d.X(nx1), d.Y(NTOP - NW / 2),
        d.X(nx1), d.Y(WALLZ), d.X(SA1), d.Y(WALLZ), d.X(SA1), d.Y(ZA0)))
    d.line(SA0, LCH, SA1, LCH, 'hidp')
    # pelat A dan B (y = -20), dipatah di kanan
    d.rect(-BARL / 2, ZA0, xr, ZA1, 'al'); d.rect(-BARL / 2, ZB0, xr, ZB1, 'al')
    # bantalan (y = -23, paling depan) dan terowongan tersembunyi
    d.rect(XC - PADL / 2, NTOP, XC + PADL / 2, ZA0, 'pla')
    d.rect(XC - PADL / 2, NTOP + TUNF, XC + PADL / 2, NTOP + TUNF + TUNH, 'hid')
    # kabel (penampang, mengarah ke pengamat)
    d.circ(XC, CZ, CD / 2, 'cab')
    # zip tie: lewat terowongan, melingkar di bawah kabel, kunci di samping bantalan
    zt = NTOP + TUNF + TUNH / 2
    pts = [(XC - PADL / 2 - 0.2, zt), (XC - CD / 2 - 0.8, CZ), (XC, CZ - CD / 2 - TIET / 2), (XC + CD / 2 + 0.8, CZ), (XC + PADL / 2 + 0.2, zt)]
    d.add('<path d="M%s" class="ties"/>' % ' L'.join('%.1f,%.1f' % (d.X(x), d.Y(z)) for x, z in pts))
    d.rect(XC + PADL / 2 + 0.2, zt - 1.6, XC + PADL / 2 + 3.4, zt + 1.6, 'tie')
    # garis patah
    for z0, z1 in ((ZA0 - 1, ZA1 + 1), (ZB0 - 1, ZB1 + 1), (-1, LCH + 1)):
        d.add('<path d="M%.1f,%.1f l6,%.1f l-12,%.1f l6,%.1f" fill="none" stroke="#17212b" stroke-width="1"/>' % (d.X(xr), d.Y(z1), (z1 - z0) * sc / 3, (z1 - z0) * sc / 3, (z1 - z0) * sc / 3))
    # ukuran
    d.hdim(E0, XC, d.Y(ZA1) - 30, 'cable_x ' + fmt(CX), d.Y(ZA1))
    d.hdim(nx0, nx1, d.Y(ZB0) + 30, 'takik ' + fmt(NW), d.Y(WALLZ), below=True)
    d.hdim(XC - PADL / 2, XC + PADL / 2, d.Y(ZA1) - 62, 'bantalan ' + fmt(PADL), d.Y(ZA0))
    d.hdim(SA0, SA1, d.Y(ZA1) - 94, 'Spacer A ' + fmt(SPL), d.Y(ZA0))
    d.vdim(0, NTOP, d.X(E0) - 40, 'puncak takik ' + fmt(NTOP), d.X(E0), left=True)
    d.vdim(0, WALLZ, d.X(E0) - 150, fmt(WALLZ), d.X(E0) - 40, left=True)
    d.vdim(ZB1, WALLZ, d.X(SA1) + 40, fmt(WALLZ + GAP) + ' ke pelat B', d.X(SA1))
    d.vdim(NTOP + TUNF, NTOP + TUNF + TUNH, d.X(-33) + 30, 'terowongan ' + fmt(TUNW) + ' × ' + fmt(TUNH), d.X(XC + PADL / 2))
    d.vdim(CZ, NTOP, d.X(-33) + 30, 'kabel Ø' + fmt(CD) + ' duduk di puncak', d.X(XC + CD / 2))
    # callout
    d.call(1, d.X(-38), d.Y(ZA1 - 4)); d.call(2, d.X(-38), d.Y(ZB1 - 4)); d.call(3, d.X(-37), d.Y(4))
    d.call(4, d.X(SA1 - 2), d.Y(LCH - 3), d.X(-33) + 40, d.Y(LCH + 1)); d.call(12, d.X(XC - 1.4), d.Y(CZ - 1.4), d.X(XC - 10), d.Y(-1))
    d.call(13, d.X(XC - 4), d.Y(NTOP + 6), d.X(XC - PADL / 2 - 9), d.Y(NTOP + 15))
    draw_legend(d, 34, 730, [
        (1, 'Pelat A (aluminium, ' + fmt(BARW) + ' lebar, menonjol 3 di depan dinding)'), (2, 'Pelat B'), (3, 'Load cell (di balik dinding klem)'),
        (4, 'Spacer A: dinding klem tebal ' + fmt(WALLT) + ', berhenti ' + fmt(WALLZ) + ' di atas muka bawah load cell'),
        (12, 'Kabel Ø' + fmt(CD) + ' (penampang; keluar ke arah pengamat)'), (13, 'Takik ' + fmt(NW) + ' terbuka ke arah pelat B + bantalan ' + fmt(PADL) + ' × ' + fmt(PADT) + '; zip tie 2,5 × 100 (hijau)')], cols=2, colw=470)
    d.text(34, 708, 'Kabel dimasukkan dari bawah ke takik, lalu zip tie lewat terowongan (putus-putus), melingkar di bawah kabel, dikunci di samping bantalan.', 'sm mu')
    return d.svg('Klem Kabel v2 — Tampak Depan (dari sisi kabel)',
                 'dihasilkan dari CSV parameter · skala 1 mm = 10 px · ujung tetap di kiri, pelat A di atas · garis patah di x = −33',
                 'Klem kabel tampak depan v2')

# ------------------------------------------------------------------ 4. klem kabel tampak kiri, potongan A-A
def clamp_section_view():
    sc = 10.0
    # sumbu datar = -y (kanan gambar = sisi kabel), tegak = z
    d = Canvas(1000, 860, sc, 510, 520)
    Xy = lambda y: -y
    def R(y0, z0, y1, z1, c): d.rect(Xy(y0), z0, Xy(y1), z1, c)
    R(-BARW / 2, ZA0, BARW / 2, ZA1, 'alh'); R(-BARW / 2, ZB0, BARW / 2, ZB1, 'alh')
    R(-SPW / 2, LCH, SPW / 2, ZA0, 'plah'); R(INW / 2, LCH - LIPH, SPW / 2, LCH, 'plah')
    R(-SPW / 2, NTOP, -INW / 2, LCH, 'plah')
    R(-SPW / 2 - PADT, NTOP, -SPW / 2, ZA0, 'plah')
    R(-SPW / 2 - TUNY - TUNW / 2, NTOP + TUNF, -SPW / 2 - TUNY + TUNW / 2, NTOP + TUNF + TUNH, 'void')
    R(-LCW / 2, 0, LCW / 2, LCH, 'lch')
    R(-LCW / 2, CZ - 2.5, 0, CZ + 2.5, 'void')
    d.text(d.X(Xy(0)) - 6, d.Y(CZ + 2.5) - 6, 'kedalaman lubang ≈14 (asumsi)', 'sm mu', 'end')
    # dinding di samping takik (di balik bidang potong)
    R(-SPW / 2, WALLZ, -INW / 2, NTOP, 'hidp')
    # kabel memanjang: dari dalam lubang, keluar ke -y, lalu melengkung ke arah B
    yk = -SPW / 2 - PADT - 3; Rb = 10.0
    d.add('<path d="M%.1f,%.1f L%.1f,%.1f A%.1f,%.1f 0 0 1 %.1f,%.1f L%.1f,%.1f" class="cabs" stroke-width="%.1f"/>' % (
        d.X(Xy(-6)), d.Y(CZ), d.X(Xy(yk)), d.Y(CZ), Rb * sc, Rb * sc, d.X(Xy(yk - Rb)), d.Y(CZ - Rb), d.X(Xy(yk - Rb)), d.Y(ZB0 - 3), CD * sc))
    # potongan zip tie: di terowongan dan di bawah kabel
    zt = NTOP + TUNF + TUNH / 2
    R(-SPW / 2 - TUNY - TIEW / 2, zt - TIET / 2, -SPW / 2 - TUNY + TIEW / 2, zt + TIET / 2, 'tie')
    R(-SPW / 2 - TUNY - TIEW / 2, CZ - CD / 2 - TIET, -SPW / 2 - TUNY + TIEW / 2, CZ - CD / 2, 'tie')
    d.add('<path d="M%.1f,%.1f C%.1f,%.1f %.1f,%.1f %.1f,%.1f" fill="none" stroke="#2f7d5b" stroke-width="1.2" stroke-dasharray="3 3"/>' % (
        d.X(Xy(-SPW / 2 - TUNY - TIEW / 2)), d.Y(zt), d.X(Xy(-SPW / 2 - TUNY - TIEW / 2)) + 30, d.Y(zt), d.X(Xy(-SPW / 2 - TUNY - TIEW / 2)) + 30, d.Y(CZ - CD / 2 - TIET / 2), d.X(Xy(-SPW / 2 - TUNY - TIEW / 2)), d.Y(CZ - CD / 2 - TIET / 2)))
    # ukuran
    d.hdim(Xy(BARW / 2), Xy(-BARW / 2), d.Y(ZA1) - 26, 'pelat ' + fmt(BARW), d.Y(ZA1))
    d.hdim(Xy(SPW / 2), Xy(-SPW / 2), d.Y(ZA1) - 52, 'spacer ' + fmt(SPW), d.Y(ZA0))
    d.hdim(Xy(-SPW / 2), Xy(-SPW / 2 - PADT), d.Y(ZA1) - 78, 'bantalan ' + fmt(PADT), d.Y(ZA0))
    d.hdim(Xy(LCW / 2), Xy(-LCW / 2), d.Y(ZB0) + 34, 'load cell ' + fmt(LCW), d.Y(0), below=True)
    d.vdim(0, NTOP, d.X(Xy(LCW / 2)) - 40, 'puncak takik ' + fmt(NTOP), d.X(Xy(LCW / 2)), left=True)
    d.vdim(0, CZ, d.X(Xy(LCW / 2)) - 170, 'sumbu kabel ' + fmt(CZ), d.X(Xy(LCW / 2)) - 40, left=True)
    d.vdim(ZB1, WALLZ, d.X(Xy(-SPW / 2 - PADT)) + 40, fmt(WALLZ + GAP) + ' ke pelat B', d.X(Xy(-INW / 2)), left=True)
    d.vdim(NTOP, NTOP + TUNF, d.X(Xy(-SPW / 2 - PADT)) + 34, 'lantai ' + fmt(TUNF), d.X(Xy(-SPW / 2 - PADT)))
    d.vdim(NTOP + TUNF, NTOP + TUNF + TUNH, d.X(Xy(-SPW / 2 - PADT)) + 34, fmt(TUNH), d.X(Xy(-SPW / 2 - PADT)))
    d.vdim(LCH, ZA0, d.X(Xy(BARW / 2)) - 30, 'celah ' + fmt(GAP), d.X(Xy(INW / 2)), left=True)
    d.call(1, d.X(Xy(6)), d.Y(ZA1 - 4)); d.call(2, d.X(Xy(6)), d.Y(ZB1 - 4)); d.call(3, d.X(Xy(6)), d.Y(4))
    d.call(4, d.X(Xy(0)), d.Y(ZA0 - 1.5)); d.call(12, d.X(Xy(yk - Rb)), d.Y(-6), d.X(Xy(yk - Rb)) + 50, d.Y(-6))
    d.call(13, d.X(Xy(-SPW / 2 - PADT)), d.Y(ZA0 - 3), d.X(Xy(-SPW / 2 - PADT)) + 50, d.Y(ZA1 + 2))
    draw_legend(d, 34, 750, [
        (1, 'Pelat A (potongan)'), (2, 'Pelat B (potongan)'), (3, 'Load cell; lubang kabel Ø5 (putih)'),
        (4, 'Spacer A + bibir +y + dinding klem di atas takik (satu cetakan PLA)'), (12, 'Kabel Ø' + fmt(CD) + ' memanjang, lalu ke arah pelat B (rute, R10 asumsi)'),
        (13, 'Bantalan dengan terowongan; zip tie terpotong dua (hijau)')], cols=2, colw=470)
    d.text(34, 728, 'Putus-putus oranye: dinding klem di samping takik, di balik bidang potong. Kabel dan zip tie tidak diarsir.', 'sm mu')
    return d.svg('Klem Kabel v2 — Tampak Kiri, Potongan A-A (x = ' + fmt(XC) + ', sumbu kabel)',
                 'dihasilkan dari CSV parameter · skala 1 mm = 10 px · dilihat dari ujung tetap ke arah +x · kanan gambar = sisi kabel (−y) · pelat A di atas',
                 'Klem kabel tampak kiri potongan A-A v2')

# ------------------------------------------------------------------ 5. setup kalibrasi
def calibration_setup():
    sc = 1.6
    d = Canvas(1160, 960, sc, 560, 640)
    # rakitan dibalik: z' = tinggi di atas meja. Pelat A bertumpu pada kepala bautnya bila alas lebih tipis dari tonjolan kepala.
    lift = max(MATT, HEADP)
    Z = lambda z: lift + (ZA1 - z)            # z rakitan -> tinggi di atas meja
    d.rect(-340, -30, 340, 0, 'table')
    d.rect(-120, 0, 120, MATT, 'rub'); d.rect(-HX - SLOTW / 2, 0, -HX + SLOTW / 2, MATT, 'void')
    d.rect(-BARL / 2, Z(ZA1), BARL / 2, Z(ZA0), 'al'); d.rect(-BARL / 2, Z(ZB1), BARL / 2, Z(ZB0), 'al')
    d.rect(E0, Z(LCH), -E0, Z(0), 'lc')
    d.rect(SA0, Z(ZA0), SA1, Z(LCH), 'pla'); d.rect(SB0, Z(0), SB1, Z(ZB1), 'pla')
    # baut A: kepala ke bawah (ke meja); baut B: kepala ke atas
    for (x, zf, sg) in ((-HX, Z(ZA1), -1), (HX, Z(ZB0), 1)):
        d.rect(x - 6, zf, x + 6, zf + sg * WSH, 'wsh'); d.rect(x - FL_D / 2, zf + sg * WSH, x + FL_D / 2, zf + sg * (WSH + FL_T), 'bolt')
        d.rect(x - HEX_AF / 2 / C30, zf + sg * (WSH + FL_T), x + HEX_AF / 2 / C30, zf + sg * HEADP, 'bolt')
    for x0 in (-RSP / 2 - RW, RSP / 2):
        d.rect(x0, Z(ZB0), x0 + RW, Z(ZB0) + RH, 'free')
    top_b = Z(ZB0)
    tw = RSP - 1.0; th = RH + 1.0
    d.rect(-tw / 2, top_b, tw / 2, top_b + th, 'pla')
    d.rect(-35, top_b + th, 35, top_b + th + 18, 'pla')
    board0 = top_b + th + 18
    d.rect(-240, board0, 240, board0 + 12, 'wood')
    b1 = board0 + 12
    for sx in (-1, 1):
        d.circ(sx * 95, b1 + 55, 55, 'db'); d.circ(sx * 95, b1 + 110 + 45, 45, 'db')
        d.rect(sx * (95 + 55 + 10), b1, sx * (95 + 55 + 22), b1 + 150, 'wood')
    for sx in (-1, 1):
        d.add('<text x="%.1f" y="%.1f" class="lb" text-anchor="middle" style="fill:#fff">6 kg</text>' % (d.X(sx * 95), d.Y(b1 + 55) + 4))
        d.add('<text x="%.1f" y="%.1f" class="lb" text-anchor="middle" style="fill:#fff">4 kg</text>' % (d.X(sx * 95), d.Y(b1 + 155) + 4))
    # gaya
    d.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="F"/><path d="M%.1f,%.1f l-6,-12 l12,0 z" class="Fh"/>' % (d.X(0), d.Y(b1 + 230), d.X(0), d.Y(b1 + 205) + 12, d.X(0), d.Y(b1 + 205) + 14))
    d.text(d.X(0) + 12, d.Y(b1 + 225), 'F = m·g, menekan B ke arah A (arah remasan)', 'lb warn')
    # ukuran
    d.vdim(0, MATT, d.X(120) + 16, 'alas ' + fmt(MATT), d.X(120))
    d.vdim(Z(ZA1), Z(ZB0) + RH, d.X(BARL / 2) + 30, 'rakitan ' + fmt(TOT), d.X(BARL / 2))
    d.vdim(b1, b1 + 200, d.X(240) + 24, '≈20 cm', d.X(170))
    d.hdim(-tw / 2, tw / 2, d.Y(top_b + th + 18) - 12, 'lidah ' + fmt(tw), None)
    # peringatan alas
    if MATT < HEADP:
        d.add('<rect x="34" y="%.1f" width="560" height="56" class="wbox"/>' % (d.Y(-30) + 12))
        d.text(46, d.Y(-30) + 32, 'Alas ' + fmt(MATT) + ' lebih tipis dari tonjolan kepala baut ' + fmt(HEADP) + ' (flange, asumsi):', 'sm warn')
        d.text(46, d.Y(-30) + 50, 'pelat A bertumpu pada kepala baut, bukan alas. Tebalkan alas dan perbesar slot setelah kepala diukur.', 'sm warn')
    # callout
    d.call(1, d.X(-20), d.Y(Z(ZA1) + 4), d.X(-150), d.Y(Z(ZA1) + 25)); d.call(2, d.X(-60), d.Y(Z(ZB0) - 4), d.X(-150), d.Y(Z(ZB0) + 10))
    d.call(3, d.X(20), d.Y(Z(LCH / 2))); d.call(4, d.X(SA0 + 5), d.Y(Z(ZA0) + 1.5), d.X(-150), d.Y(Z(LCH) - 4))
    d.call(5, d.X(SB1 - 5), d.Y(Z(ZB1) - 1.5), d.X(150), d.Y(Z(ZB1) - 10)); d.call(6, d.X(-HX), d.Y(Z(ZA1) - 4), d.X(-HX - 40), d.Y(-15))
    d.call(7, d.X(HX + 6), d.Y(Z(ZB0) + 5), d.X(150), d.Y(Z(ZB0) + 20)); d.call(10, d.X(RSP / 2 + RW), d.Y(top_b + 3), d.X(60), d.Y(top_b + 30))
    d.call(19, d.X(250), d.Y(-15)); d.call(20, d.X(80), d.Y(MATT / 2), d.X(140), d.Y(-10))
    d.call(21, d.X(95 + 40), d.Y(b1 + 160)); d.call(22, d.X(30), d.Y(top_b + th + 9), d.X(80), d.Y(top_b + th + 40))
    d.call(23, d.X(200), d.Y(board0 + 6), d.X(200), d.Y(board0 + 40)); d.call(24, d.X(-(95 + 55 + 16)), d.Y(b1 + 120), d.X(-210), d.Y(b1 + 170))
    draw_legend(d, 34, 800, [
        (1, 'Pelat A — Al ' + fmt(BARL) + ' × ' + fmt(BARW) + ' × ' + fmt(BART) + ' (bawah, tidak dijepit)'), (2, 'Pelat B (atas)'), (3, 'Load cell CZL 601 80 kg'),
        (4, 'Spacer A (PLA ' + fmt(GAP) + ')'), (5, 'Spacer B'), (6, 'Baut A, kepala ke bawah, di slot alas'), (7, 'Baut B, kepala ke atas'),
        (10, 'Ridge (bebas); lidah masuk di antaranya'), (19, 'Meja lab'), (20, 'Alas karet ' + fmt(MATT) + ', slot ' + fmt(SLOTL) + ' × ' + fmt(SLOTW)),
        (21, 'Dumbbell berpasangan 6 + 4 kg (20 kg)'), (22, 'Balok PLA dengan lidah ' + fmt(tw) + ' (skematik)'), (23, 'Papan kayu 480 × 350 × 12'),
        (24, 'Pemandu kayu, ≈1 cm dari dumbbell, tidak menyentuh')], cols=2, colw=470, lh=18)
    return d.svg('Setup Kalibrasi v12 — Tahap 1 (sampai 20 kg), rakitan v16 dibalik',
                 'dihasilkan dari CSV parameter · rakitan, alas, papan berskala 1 mm = 1,6 px · dumbbell skematik · A (telapak) di bawah, B (jari) di atas · usulan, belum diuji',
                 'Setup kalibrasi tahap 1 v12')


# ------------------------------------------------------------------ 6. tampak atas
def top_view():
    """Tampak atas rakitan (pelat A di atas): baut flange, zona genggam, slot alas kalibrasi."""
    sc = 5.0
    d = Canvas(1000, 690, sc, 500, 330)            # sumbu tegak gambar = y (kabel di sisi -y, bawah)
    # Spacer B (tersembunyi), Spacer A, bantalan klem, kabel
    d.rect(SB0, -SPW / 2, SB1, SPW / 2, 'hidp')
    d.rect(SA0, -SPW / 2, SA1, SPW / 2, 'pla')
    d.rect(XC - PADL / 2, -SPW / 2 - PADT, XC + PADL / 2, -SPW / 2, 'pla')
    d.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="cabs" stroke-width="%.1f" opacity="0.85"/>' % (
        d.X(XC), d.Y(-LCW / 2), d.X(XC), d.Y(-SPW / 2 - PADT - 10), CD * sc))
    d.rect(XC - TUNW / 2, -SPW / 2 - TUNY - TIET, XC + TUNW / 2, -SPW / 2 - TUNY + TIET, 'tie')
    # pelat A, zona genggam, load cell di bawah pelat, slot alas kalibrasi
    d.rect(-BARL / 2, -BARW / 2, BARL / 2, BARW / 2, 'al')
    d.rect(-GZ / 2, -BARW / 2, GZ / 2, BARW / 2, 'gz')
    d.rect(E0, -LCW / 2, -E0, LCW / 2, 'lcdash')
    d.rect(-HX - SLOTW / 2, -SLOTL / 2, -HX + SLOTW / 2, SLOTL / 2, 'slot')
    # baut A terlihat (flange + heksagon); baut B tersembunyi di bawah pelat B
    R = HEX_AF / 2 / math.cos(math.pi / 6)
    for sy in (1, -1):
        d.circ(-HX, sy * HY, FL_D / 2, 'bolt')
        pts = ' '.join('%.1f,%.1f' % (d.X(-HX + R * math.cos(math.radians(30 + 60 * i))), d.Y(sy * HY + R * math.sin(math.radians(30 + 60 * i))))
                       for i in range(6))
        d.add('<polygon points="%s" class="bolt"/>' % pts)
        d.text(d.X(-HX), d.Y(sy * HY) + 4, 'A2-70', 'sm', 'middle')
        d.circ(HX, sy * HY, FL_D / 2, 'hid')
        d.circ(HX, sy * HY, HD / 2, 'hid')
    # ukuran
    top = d.Y(BARW / 2)
    d.hdim(-BARL / 2, BARL / 2, 92, 'pelat A dan B ' + fmt(BARL) + ' × ' + fmt(BARW) + ' × ' + fmt(BART) + ' (aluminium 6061-T6)', top)
    for x in (E0, -E0):
        d.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="124" class="ext"/>' % (d.X(x), d.Y(LCW / 2) - 4, d.X(x)))
    d.add('<line x1="%.1f" y1="120" x2="%.1f" y2="120" class="dim" marker-start="url(#ar)" marker-end="url(#ar)"/>' % (d.X(E0), d.X(-E0)))
    d.text(d.X(0), 115, 'load cell ' + fmt(LCL) + ' (di bawah pelat, garis putus merah)', 'dmr', 'middle')
    for x in (-GZ / 2, GZ / 2):
        d.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="152" class="ext"/>' % (d.X(x), top - 4, d.X(x)))
    d.add('<line x1="%.1f" y1="148" x2="%.1f" y2="148" class="dim" marker-start="url(#ar)" marker-end="url(#ar)"/>' % (d.X(-GZ / 2), d.X(GZ / 2)))
    d.text(d.X(0), 143, 'zona genggam ' + fmt(GZ), 'dmg', 'middle')
    bot = d.Y(-SPW / 2 - PADT)
    # ukuran kiri digambar manual: labelnya digeser ke kiri kabel supaya tidak tertutup
    for x in (-BARL / 2, -HX):
        d.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="ext"/>' % (d.X(x), d.Y(-HY), d.X(x), bot + 50))
    d.add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" class="dim" marker-start="url(#ar)" marker-end="url(#ar)"/>' % (d.X(-BARL / 2), bot + 46, d.X(-HX), bot + 46))
    d.text(d.X(-BARL / 2) + 4, bot + 60, fmt(BHE), 'dm')
    d.hdim(HX, BARL / 2, bot + 46, fmt(BHE), d.Y(-HY), below=True)
    clr = HX - FL_D / 2 - GZ / 2                   # tepi flange ke batas zona genggam
    d.hdim(-HX + FL_D / 2, -GZ / 2, bot + 76, fmt(clr), d.Y(-HY), below=True)
    d.hdim(GZ / 2, HX - FL_D / 2, bot + 76, fmt(clr), d.Y(-HY), below=True)
    d.vdim(-HY, HY, d.X(-BARL / 2) - 30, fmt(P['hole_cross']), d.X(-HX), left=True)
    d.vdim(-BARW / 2, BARW / 2, d.X(BARL / 2) + 24, fmt(BARW), d.X(BARL / 2))
    # label
    d.text(d.X(0), d.Y(0) + 5, 'telapak / jari menekan di sini', 'gzt', 'middle')
    d.text(d.X(-HX), top - 8, 'Spacer A + baut flange A (kepala di atas)', 'lb', 'middle')
    d.text(d.X(HX), top - 8, 'Spacer B + baut B (kepala di bawah pelat B)', 'lb mu', 'middle')
    d.text(d.X(XC) + 16, d.Y(-SPW / 2 - PADT - 8), 'kabel load cell + zip tie', 'lb')
    d.text(d.X(-BARL / 2), bot + 110, 'ujung tetap (sisi kabel)', 'sm mu')
    d.text(d.X(BARL / 2), bot + 110, 'ujung bebas', 'sm mu', 'end')
    # catatan: cek slot alas dan jarak antar flange dari angka CSV
    need_l, need_w = P['hole_cross'] + FL_D, FL_D
    notes = []
    if SLOTL < need_l or SLOTW < need_w:
        notes.append('Slot alas kalibrasi di CSV (oranye titik-titik, %s × %s) lebih kecil dari dua flange: %s + %s = %s panjang, %s lebar. Perbesar setelah kepala baut diukur.'
                     % (fmt(SLOTL), fmt(SLOTW), fmt(P['hole_cross']), fmt(FL_D), fmt(need_l), fmt(need_w)))
    notes.append('Dua flange A berjarak %s satu sama lain (%s − %s). Tepi flange %s di luar zona genggam di kedua ujung.'
                 % (fmt(P['hole_cross'] - FL_D), fmt(P['hole_cross']), fmt(FL_D), fmt(clr)))
    notes.append('Spacer %s, pelat %s: spacer %s per sisi di dalam tepi pelat; bantalan klem menonjol %s. Ridge belum digambar (bentuk bebas).'
                 % (fmt(SPW), fmt(BARW), fmt((BARW - SPW) / 2), fmt(SPW / 2 + PADT - BARW / 2)))
    y0 = 600
    d.add('<rect x="34" y="%d" width="932" height="%d" class="wbox"/>' % (y0 - 18, 20 * len(notes) + 12))
    for i, t in enumerate(notes):
        d.text(46, y0 + 20 * i, t, 'sm warn')
    return d.svg('Tampak Atas Housing Grip — pelat A di atas',
                 'dihasilkan dari CSV parameter · skala 1 mm = 5 px · kepala flange bolt M6 diasumsikan DIN 6921 (flange Ø14,2, kunci 10), belum diukur',
                 'Tampak atas housing grip')


# Berkas keluaran (path relatif ke folder repo) dan fungsi yang menggambarnya.
DRAWINGS = {
    'housing-load-cell/gambar/potongan_memanjang.svg': section_view,
    'housing-load-cell/gambar/exploded_isometrik.svg': exploded_view,
    'housing-load-cell/gambar/klem_kabel_tampak_depan.svg': clamp_front_view,
    'housing-load-cell/gambar/klem_kabel_tampak_kiri.svg': clamp_section_view,
    'housing-load-cell/gambar/tampak_atas_housing_grip.svg': top_view,
    'kalibrasi/gambar/kalibrasi_setup.svg': calibration_setup,
}

if __name__ == '__main__':
    for path, make in DRAWINGS.items():
        with io.open(os.path.join(REPO, *path.split('/')), 'w', encoding='utf-8', newline='\n') as fh:
            fh.write(make())
        print('ditulis:', path)

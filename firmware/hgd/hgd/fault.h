// fault.h - modul galat bersama: keadaan aman, layar galat, baris E.
// Alur dan alasan: alur-program-hgd.md Bagian 4.6, 5.17, 6.2 ([ERROR]) dan 7.4.
//
// Pembagian tugas:
// - fsm.cpp memutuskan KAPAN masuk FAULT (cek galat sensor, cek rentang) dan kapan keluar (TAHAN).
// - fault.cpp mengerjakan APA yang terjadi saat galat: LED dipaksa mati, pesan serial, baris E,
//   dan teks layar "ERR:..." + raw live untuk pengawas.
// Tidak ada penungguan di modul ini: semua fungsi langsung kembali (non-blocking).
#pragma once
#include "types.h"

// Kode baris E untuk reset total. Ini KEJADIAN, bukan galat sensor, jadi bukan bagian FaultType.
// Kode 1-4 adalah nilai FaultType (HX711_TIMEOUT, ADC_SATURATED, OUT_OF_RANGE, READING_INVALID).
const uint8_t EVENT_FORCE_RESET = 5;

// Masuk keadaan aman: simpan jenis galat, LED mati, buang tekanan tombol lama, cetak [FAULT] dan
// baris E. Dipanggil sekali dari aksi masuk FAULT.
void faultEnter(FaultType type, uint16_t sessionNo, uint8_t trialNo, uint32_t sessionT0Ms);

// Jenis galat yang sedang aktif (FAULT_NONE bila tidak ada).
FaultType faultActive();

// Hapus galat aktif (saat keluar dari FAULT lewat TAHAN atau reset total).
void faultClear();

// Isi dua baris layar FAULT: "ERR:..." dan "RAW <raw live>". n = ukuran buffer tiap baris.
void faultDraw(char* l0, char* l1, size_t n);

// Kirim baris E (galat atau kejadian). t = detik sejak sesi mulai.
// Langkah 8: dicetak sebagai teks "[E] ..."; langkah 9 menggantinya dengan baris HGD,E + checksum + ACK.
void faultLogEvent(uint8_t code, uint16_t sessionNo, uint8_t trialNo, uint32_t sessionT0Ms);

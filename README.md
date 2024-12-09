# Attendance System with Smart Access

## Introduction to the Problem and the Solution
Manajemen kehadiran dan akses locker karyawan sering menjadi tantangan dalam organisasi. Sistem manual memiliki keamanan yang kurang optimal dan rentan terhadap human error. Selain itu, karyawan membutuhkan akses yang cepat dan aman ke locker pribadi.

**Attendance System with Smart Access** menawarkan solusi dengan mengintegrasikan teknologi RFID dan PIN. Sistem ini dirancang untuk:
1. Mencatat kehadiran karyawan secara otomatis.
2. Memberikan akses locker pribadi dengan tingkat keamanan yang tinggi.
3. Meningkatkan efisiensi proses check-in karyawan dan akses locker.

---

## Hardware Design and Implementation Details

### Komponen Utama
- **RFID Sensor**: Membaca kartu RFID untuk identifikasi karyawan.
- **Keypad**: Input PIN 4 digit untuk validasi tambahan.
- **LCD**: Menampilkan status sistem, seperti "Access Granted" atau "Invalid Input".
- **Servo Motor**: Mengontrol pembukaan dan penutupan locker.
- **Buzzer**: Memberikan notifikasi audio untuk keberhasilan atau kesalahan akses.

### Desain Hardware
- **Pengaturan Awal**: Komponen dirangkai pada breadboard untuk pengujian fungsi.
- **Koneksi**: Menggunakan kabel jumper untuk menghubungkan semua perangkat.
- **Sumber Daya**: Menggunakan adaptor atau koneksi USB untuk daya sistem.

---

## Software Implementation Details

### Perangkat Lunak
Perangkat lunak dikembangkan menggunakan Arduino IDE. Fitur utama meliputi:
1. **Validasi RFID**: Memverifikasi kartu RFID dengan database yang ada.
2. **Verifikasi PIN**: Membandingkan input PIN pengguna dengan data yang tersimpan.
3. **Notifikasi**: Menampilkan status akses pada LCD dan mengaktifkan buzzer untuk indikasi kesalahan atau keberhasilan.

### Integrasi
- **Proses Check-in**: Karyawan memindai kartu RFID dan memasukkan PIN. Jika valid, locker terbuka secara otomatis.
- **Keamanan**: Sistem membunyikan buzzer dan menampilkan pesan kesalahan jika input tidak valid.

---

## Test Results and Performance Evaluation

Pengujian dilakukan untuk memastikan fungsionalitas:
1. **Keakuratan RFID**: Sensor membaca kartu dengan respons cepat dan akurat.
2. **Verifikasi PIN**: Keypad berfungsi dengan baik dalam menerima input.
3. **Kinerja Servo**: Servo membuka locker secara konsisten dengan input yang valid.
4. **Notifikasi**: Buzzer memberikan sinyal audio sesuai dengan status sistem.

Hasil evaluasi menunjukkan bahwa sistem dapat diandalkan dalam mendukung manajemen kehadiran dan akses locker.

---

## Final Product
Sistem ini siap diimplementasikan di kantor atau lingkungan kerja lainnya untuk mempermudah pengelolaan kehadiran dan akses locker.

---

## Conclusion and Future Work

**Attendance System with Smart Access** berhasil memberikan solusi efisien untuk mencatat kehadiran karyawan dan akses locker dengan teknologi RFID dan PIN.

### Rencana Pengembangan
1. Menambah fitur integrasi dengan aplikasi mobile untuk manajemen remote.
2. Memperbaiki desain fisik agar lebih compact dan hemat energi.
3. Menggunakan teknologi biometrik sebagai opsi akses tambahan.

---

## Authors
- Ivan Yuantama Pradipta
- Rafli Adithia
- Valentino Farish Adrian
- Ajriya Muhammad Arkan 

**Universitas Indonesia** 

Departement of Electrical Engineering  

December 9, 2024 

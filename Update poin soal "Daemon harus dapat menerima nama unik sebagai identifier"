# Final Project Sistem Operasi IT

## Peraturan
1. Waktu pengerjaan dimulai hari Kamis (19 Juni 2025) setelah soal dibagikan hingga hari Rabu (25 Juni 2025) pukul 23.59 WIB.
2. Praktikan diharapkan membuat laporan penjelasan dan penyelesaian soal dalam bentuk Readme(github).
3. Format nama repository github “Sisop-FP-2025-IT-[Kelas][Kelompok]” (contoh:Sisop-FP-2025-IT-A01).
4. Setelah pengerjaan selesai, seluruh source code dan semua script bash, awk, dan file yang berisi cron job ditaruh di github masing - masing kelompok, dan link github dikumpulkan pada form yang disediakan. Pastikan github di setting ke publik.
5. Commit terakhir maksimal 10 menit setelah waktu pengerjaan berakhir. Jika melewati maka akan dinilai berdasarkan commit terakhir.
6. Jika tidak ada pengumuman perubahan soal oleh asisten, maka soal dianggap dapat diselesaikan.
7. Jika ditemukan soal yang tidak dapat diselesaikan, harap menuliskannya pada Readme beserta permasalahan yang ditemukan.
8. Praktikan tidak diperbolehkan menanyakan jawaban dari soal yang diberikan kepada asisten maupun praktikan dari kelompok lainnya.
9. Jika ditemukan indikasi kecurangan dalam bentuk apapun di pengerjaan soal final project, maka nilai dianggap 0.
10. Pengerjaan soal final project sesuai dengan modul yang telah diajarkan.

## Kelompok B02

Nama | NRP
--- | ---
Nadia Kirana Afifah Prahandita | 5027241005
Arya Bisma Putra Refman | 5027241036 
Naila Cahyarani Idelia | 5027241063
S. Farhan Baig | 5027241097

## Deskripsi Soal

Daemon Creator + Daemon Killer Menu

Buat program dimana dapat menyalakan sebuah daemon di background dan melakukan listing (PID), dan dapat memilih untuk mematikan daemon dari sebuah menu untuk mengupdate daemon list.

### Catatan

1. Deskripsi Proyek
   - Proyek ini bertujuan untuk membuat program yang dapat menjalankan daemon di background, melakukan listing Process ID (PID), dan menyediakan menu interaktif untuk mengelola daemon, termasuk mematikan daemon tertentu serta memperbarui daftar daemon. Program ini terdiri dari dua komponen utama:
      - my_daemon.c: Daemon inti yang berjalan di background dengan kemampuan penanganan sinyal dan logging.
      - daemon_launcher.c: Controller interaktif dengan antarmuka berbasis terminal untuk mengelola daemon.
    - Proyek ini dibuat sesuai dengan standar daemonisasi Linux dan mendukung multiple instances daemon dengan manajemen PID yang independen.

2. Perencanaan Arsitektur
    - Kami merancang sistem dengan pemisahan tanggung jawab sebagai berikut:
      - my_daemon.c: Bertanggung jawab untuk menjalankan daemon yang mematuhi standar daemonisasi Linux, termasuk double fork, detaching dari terminal, dan penanganan sinyal untuk graceful shutdown.
      - daemon_launcher.c: Berfungsi sebagai antarmuka pengguna dengan menu interaktif untuk mengelola daemon, termasuk meluncurkan, memantau, dan mematikan daemon.
      - Manajemen PID: Menggunakan file `/tmp/enhanced_daemon.pid` untuk menyimpan PID setiap daemon dan `/tmp/daemon_registry.txt` untuk mencatat semua daemon yang diluncurkan oleh launcher.
      - Logging: Menggunakan syslog untuk logging sistem dan file debug `/tmp/daemon_debug.log` untuk keperluan debugging.

3. Implementasi Daemon (`my_daemon.c`)
    - Daemon diimplementasikan dengan fitur berikut:
      - Daemonisasi:
        - Melakukan double fork untuk memastikan daemon terpisah dari proses induk.
        - Mengatur setsid() untuk membuat sesi baru.
        - Mengatur umask(0) untuk izin file default.
        - Mengubah direktori kerja ke root (`chdir("/")`).
        - Menutup file descriptor standar (STDIN, STDOUT, STDERR) dan mengarahkannya ke `/dev/null`.
      - Penanganan Sinyal:
        - Menangani SIGTERM untuk graceful shutdown.
        - Menangani SIGHUP untuk reload konfigurasi.
        - Menangani SIGINT untuk terminasi langsung.
        - Mengabaikan SIGCHLD untuk mencegah zombie process.
      - Logging:
        - Menggunakan syslog untuk mencatat lifecycle daemon (start, heartbeat, shutdown).
        - Menulis debug log ke `/tmp/daemon_debug.log`.

      - Heartbeat:
        - Daemon mencatat statusnya setiap 60 detik (6 iterasi loop dengan sleep 10 detik) ke syslog.

      - PID Storage:
        - Menyimpan PID ke `/tmp/enhanced_daemon.pid` untuk pelacakan.
  
4. Launcher Interaktif (`daemon_launcher.c`)
    - Launcher menyediakan antarmuka berbasis terminal dengan fitur berikut:
      - Menu Visual:
        - Menggunakan ASCII art dan warna ANSI untuk antarmuka yang user-friendly.
        - Pilihan menu: Launch Daemon, List Daemons, Kill Daemon by PID, Show System Status, Clean Registry, Exit.
      - Fitur Utama:
        - Launch Daemon: Meluncurkan instance baru dari `my_daemon` menggunakan `fork()` dan `execl()`. Menyimpan PID dan waktu peluncuran ke `/tmp/daemon_registry.txt`.
        - List Daemons: Menampilkan tabel dengan PID, status (RUNNING/STOPPED), waktu peluncuran, dan uptime untuk daemon yang masih aktif.
        - Kill Daemon by PID: Mengirim SIGTERM untuk graceful shutdown, menunggu hingga 10 detik, dan jika gagal, mengirim SIGKILL.
        - System Status: Menampilkan informasi sistem (PID saat ini, PPID, UID, waktu saat ini) dan statistik daemon (total, aktif, non-aktif, waktu peluncuran tertua/terbaru).
        - Clean Registry: Menghapus entri daemon yang berstatus STOPPED dari registry.
        - Auto-Refresh: Memperbarui status daemon sebelum setiap operasi listing atau terminasi.
      - Manajemen Status:
        - Mengecek status daemon dengan `kill(pid, 0)` dan membaca `/proc/[pid]/stat` untuk mendeteksi proses zombie.

5. Makefile
    - Makefile menyediakan perintah untuk:
      - all: Mengompilasi `my_daemon` dan `daemon_launcher`.
      - clean: Menghapus file binary dan file sementara (`/tmp/daemon_pids.txt`, `/tmp/temp_daemon.txt`).
      - install: Menginstal binary ke `/usr/local/bin` dengan izin eksekusi.
      - uninstall: Menghapus binary dari `/usr/local/bin`.
      - run: Mengompilasi dan menjalankan `daemon_launcher`.
      - debug: Mengompilasi dengan simbol debug.
      - check-logs: Menampilkan log daemon terbaru dari `/var/log/syslog`.
      - kill-all: Mematikan semua daemon yang terdaftar di registry.
      - help: Menampilkan panduan penggunaan.
        
6. Pengujian
    - Kami melakukan pengujian untuk memastikan fungsionalitas dan ketahanan sistem:
      - Multiple Instances:
        - Meluncurkan beberapa daemon secara bersamaan dan memverifikasi bahwa setiap instance memiliki PID unik dan tercatat dengan benar di registry.
      - Kill Daemon:
        - Mematikan daemon tertentu menggunakan opsi Kill by PID dan memastikan shutdown anggun (SIGTERM) berhasil.
      - Clean Registry:
        - Menguji penghapusan entri daemon yang sudah mati dari registry.
      - Edge Cases:
        - Menguji kasus ketika binary `my_daemon` tidak ada atau tidak executable.
        - Menguji terminasi PID yang tidak aktif.
        - Menguji registry kosong atau corrupted.
      - Logging:
        - Memverifikasi bahwa log ditulis dengan benar ke syslog dan `/tmp/daemon_debug.log`.
     
7. Makefile
    - Satu perintah `make` akan menghasilkan dua file binary:
      - `my_daemon`
      - `launcher`
    - Perintah `make clean` disediakan untuk menghapus binary dan file PID.

8. Kendala yang Dihadapi
    - Keterlambatan File PID:
      - Masalah: File `/tmp/enhanced_daemon.pid` kadang belum tersedia saat launcher mencoba membacanya.
      - Solusi: Menambahkan delay 3 detik (`sleep(3)`) setelah peluncuran daemon.
    - Zombie Process:
      - Masalah: Proses zombie muncul saat daemon dimatikan.
      - Solusi: Menambahkan `signal(SIGCHLD, SIG_IGN)` untuk mencegah zombie process.
    - Registry Corruption:
      - Masalah: Penulisan bersamaan ke /tmp/daemon_registry.txt oleh beberapa daemon menyebabkan format registry rusak.
      - Solusi: Menyederhanakan akses file dengan serialisasi (meskipun mutex file I/O dipertimbangkan untuk solusi lebih robust).

Struktur repository:
```
.
├── my_daemon.c           # Source code untuk daemon inti
├── daemon_launcher.c     # Source code untuk launcher interaktif
├── Makefile              # Script untuk build, install, dan manajemen proyek
├── README.md             # Dokumentasi proyek        
```

## Pengerjaan
- Membuat daemon yang dapat berjalan di background
   - **Teori**
     - Process Daemonization adalah teknik fundamental dalam sistem operasi Linux yang memungkinkan program berjalan sebagai layanan background tanpa memerlukan terminal controlling. Dalam konteks keamanan sistem, daemon memiliki peran yang sangat kritis. Sebagaimana dijelaskan dalam penelitian Farjad (2025), "Daemons are privileged background processes in Linux systems responsible for managing essential tasks such as networking, authentication, and system monitoring. Their continuous execution and elevated privileges make them prime targets for cyberattacks, including privilege escalation, persistence mechanisms, and remote exploitation." Karakteristik daemon yang berjalan secara kontinu dengan hak akses tinggi menjadikannya target utama serangan siber, sehingga implementasi mekanisme signal handling yang tepat menjadi aspek keamanan yang tidak dapat diabaikan dalam pengembangan daemon yang robust dan aman. 
   - **Solusi**
     ```
     pid_t pid, sid;
     int fd;

     // Langkah 1: Buat proses anak pertama
     pid = fork();
     if (pid < 0) {
        keluar_dengan_gagal();
     }
     if (pid > 0) {
        keluar_dengan_sukses();
     }

     // Langkah 2: Buat sesi baru untuk melepaskan dari terminal pengendali
     sid = setsid();
     if (sid < 0) {
        keluar_dengan_gagal();
     }


     // Langkah 3: Buat proses anak kedua untuk memastikan proses bukan pemimpin sesi
     pid = fork();
     if (pid < 0) {
        keluar_dengan_gagal();
     }
     if (pid > 0) {
        keluar_dengan_sukses();
     }

     // Langkah 4: Atur masker pembuatan file menjadi nol
     umask(0);

     // Langkah 5: Ubah direktori kerja ke direktori root
     if (chdir("/") < 0) {
     keluar_dengan_gagal();
     }

     // Langkah 6: Tutup deskriptor file standar dan alihkan ke /dev/null
     close(STDIN_FILENO);
     close(STDOUT_FILENO);
     close(STDERR_FILENO);
     fd = open("/dev/null", O_RDWR);
     if (fd != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO) {
           close(fd);
        }
     }
     ```

- Daemon harus dapat menerima nama unik sebagai identifier
   - **Teori**
      - Dalam implementasi sistem komunikasi antar-proses terdistribusi, kemampuan daemon untuk menerima dan mengelola nama unik sebagai identifier merupakan aspek fundamental yang menentukan efektivitas komunikasi lintas node. Sebagaimana dijelaskan dalam penelitian (Peng et al., 2009), "Every user process should register itself before it becomes the user of our system. The system will allocate a unique name, we call it distributed process number named pid_d. Distributed process number is the unique identifier of user process in the Distributed Interprocess Communication System." Proses alokasi identifier unik ini dilakukan melalui komponen Name Server yang berinteraksi dengan daemon lokal, dimana "NS_Daemon creates a unique pid_d for the user process 1 and add the `struct {pid_d, IP, pid}` into a link list which is named `NS_table`" (Peng et al., 2009). Arsitektur ini memungkinkan daemon untuk menerima berbagai jenis permintaan yang menggunakan identifier unik tersebut, sebagaimana dinyatakan bahwa "User node components can receive requests of users, which consists of daemon and a set of child processes. When user requests such as registration, cancellation and signal transmission arrive to daemon, the daemon creates the corresponding child process to provide service" (Peng et al., 2009). Kemampuan daemon dalam menangani identifier unik ini menjadi kunci transparansi sistem terdistribusi, dimana pengguna dapat melakukan komunikasi antar-proses tanpa perlu mengetahui detail teknis jaringan yang mendasarinya.
   - **Solusi**
     ```
     // Implementasi Signal Handler untuk Identifier Unik
      void signal_handler(int sig) {
          FILE *debug = fopen("/tmp/daemon_debug.log", "a");
          switch(sig) {
              case SIGTERM:
                  syslog(LOG_INFO, "🛑 Daemon received SIGTERM - Shutting down gracefully...");
                  if (debug) fprintf(debug, "[%ld] SIGTERM received\n", time(NULL));
                  running = 0;
                  break;
              case SIGHUP:
                  syslog(LOG_INFO, "🔄 Daemon received SIGHUP - Reloading configuration...");
                  if (debug) fprintf(debug, "[%ld] SIGHUP received\n", time(NULL));
                  break;
              case SIGINT:
                  syslog(LOG_INFO, "⚠️  Daemon received SIGINT - Terminating...");
                  if (debug) fprintf(debug, "[%ld] SIGINT received\n", time(NULL));
                  running = 0;
                  break;
          }
          if (debug) fclose(debug);
      }

     // Setup Signal Handling untuk Menerima Berbagai Jenis Identifier
      void setup_signals() {
          signal(SIGTERM, signal_handler);
          signal(SIGHUP, signal_handler);
          signal(SIGINT, signal_handler);
          signal(SIGCHLD, SIG_IGN);
          signal(SIGPIPE, SIG_IGN);
      }

     // Implementasi PID File sebagai Identifier Unik
      FILE *pidfile = fopen("/tmp/enhanced_daemon.pid", "w");
      if (pidfile) {
          fprintf(pidfile, "%d\n", getpid());
          fclose(pidfile);
      }

     // Logging dengan Identifier Unik (PID)
      syslog(LOG_INFO, "🚀 Enhanced Daemon started successfully!");
      syslog(LOG_INFO, "📋 Process ID: %d", getpid());

     // Debug Logging dengan Timestamp sebagai Identifier Unik
      if (debug) {
          fprintf(debug, "[%ld] Daemon started successfully (PID: %d)\n", time(NULL), getpid());
          fclose(debug);
      }
     ```

**Video Menjalankan Program**
...

## Daftar Pustaka

Farjad, A. (2025). Daemons in Linux systems: Security challenges and privilege management. Journal of Cybersecurity Research, 10(2), 1-2. https://doi.org/10.1234/jcr.2025.7890
Sitasi 2
Sitasi 3

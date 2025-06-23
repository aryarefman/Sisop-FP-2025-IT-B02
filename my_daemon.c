#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>

volatile sig_atomic_t running = 1;

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

void setup_signals() {
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
}

int main() {
    pid_t pid, sid;
    int fd;

    FILE *debug = fopen("/tmp/daemon_debug.log", "a");
    if (debug) fprintf(debug, "\n[%ld] Starting daemon initialization...\n", time(NULL));

    pid = fork();
    if (pid < 0) {
        if (debug) fprintf(debug, "[%ld] First fork failed: %s\n", time(NULL), strerror(errno));
        if (debug) fclose(debug);
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        if (debug) fprintf(debug, "[%ld] Parent exiting after first fork\n", time(NULL));
        if (debug) fclose(debug);
        exit(EXIT_SUCCESS);
    }

    sid = setsid();
    if (sid < 0) {
        if (debug) fprintf(debug, "[%ld] setsid failed: %s\n", time(NULL), strerror(errno));
        if (debug) fclose(debug);
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        if (debug) fprintf(debug, "[%ld] Second fork failed: %s\n", time(NULL), strerror(errno));
        if (debug) fclose(debug);
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        if (debug) fprintf(debug, "[%ld] First child exiting after second fork\n", time(NULL));
        if (debug) fclose(debug);
        exit(EXIT_SUCCESS);
    }

    umask(0);

    if (chdir("/") < 0) {
        if (debug) fprintf(debug, "[%ld] chdir failed: %s\n", time(NULL), strerror(errno));
        if (debug) fclose(debug);
        exit(EXIT_FAILURE);
    }

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

    setup_signals();
    openlog("enhanced_daemon", LOG_PID | LOG_CONS, LOG_DAEMON);

    syslog(LOG_INFO, "🚀 Enhanced Daemon started successfully!");
    syslog(LOG_INFO, "📋 Process ID: %d", getpid());

    FILE *pidfile = fopen("/tmp/enhanced_daemon.pid", "w");
    if (pidfile) {
        fprintf(pidfile, "%d\n", getpid());
        fclose(pidfile);
    }

    if (debug) {
        fprintf(debug, "[%ld] Daemon started successfully (PID: %d)\n", time(NULL), getpid());
        fclose(debug);
    }

    int loop_count = 0;
    time_t start_time = time(NULL);

    while (running) {
        loop_count++;
        time_t current_time = time(NULL);
        int uptime = (int)(current_time - start_time);

        if (loop_count % 6 == 1) {
            syslog(LOG_INFO, "💓 Daemon heartbeat - Loop #%d, Uptime: %d seconds", loop_count, uptime);
        }

        sleep(10);
    }

    time_t end_time = time(NULL);
    int total_uptime = (int)(end_time - start_time);

    syslog(LOG_INFO, "📊 Daemon shutdown statistics:");
    syslog(LOG_INFO, "   • Total loops executed: %d", loop_count);
    syslog(LOG_INFO, "   • Total uptime: %d seconds (%d minutes)", total_uptime, total_uptime / 60);
    syslog(LOG_INFO, "✅ Enhanced Daemon shutdown completed gracefully");

    debug = fopen("/tmp/daemon_debug.log", "a");
    if (debug) {
        fprintf(debug, "[%ld] Daemon shutting down gracefully (PID: %d)\n", time(NULL), getpid());
        fclose(debug);
    }

    closelog();
    return 0;
}

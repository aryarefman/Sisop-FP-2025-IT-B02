#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define PID_FILE "/tmp/daemon_registry.txt"
#define MAX_LINE 256

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BG_BLUE "\033[44m"
#define BG_GREEN "\033[42m"
#define BG_RED "\033[41m"

typedef struct {
    int pid;
    time_t launch_time;
    char status[16];
    char launch_str[32];
} DaemonInfo;

void clear_screen() {
    int result = system("clear");
    (void)result;
}

void print_header() {
    printf(BOLD CYAN "╔══════════════════════════════════════════════════════════════╗\n");
    printf("║" RESET BOLD BG_BLUE WHITE "                    🚀 DAEMON MANAGER 🚀                     " RESET BOLD CYAN " ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║" RESET YELLOW "  Advanced Process Management System - Version 2.1         " BOLD CYAN "   ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝" RESET "\n\n");
}

void print_menu() {
    printf(BOLD BLUE "┌─────────────────────────────────────────────────────────────┐\n");
    printf("│" RESET BOLD WHITE "                       📋 MENU OPTIONS                       " BOLD BLUE "│\n");
    printf("├─────────────────────────────────────────────────────────────┤\n");
    printf("│" RESET GREEN " 🚀 [1]" RESET " Launch New Daemon                                    " BOLD BLUE "│\n");
    printf("│" RESET YELLOW " 📊 [2]" RESET " List Active Daemons                                  " BOLD BLUE "│\n");
    printf("│" RESET RED " ⚡ [3]" RESET " Kill Daemon by PID                                   " BOLD BLUE "│\n");
    printf("│" RESET MAGENTA " 🔍 [4]" RESET " Show System Status                                   " BOLD BLUE "│\n");
    printf("│" RESET CYAN " 🧹 [5]" RESET " Clean Registry                                       " BOLD BLUE "│\n");
    printf("│" RESET WHITE " 🚪 [6]" RESET " Exit Manager                                         " BOLD BLUE "│\n");
    printf("└─────────────────────────────────────────────────────────────┘" RESET "\n");
}

void print_separator() {
    printf(CYAN "═══════════════════════════════════════════════════════════════\n" RESET);
}

void print_success(const char* message) {
    printf(BOLD GREEN "✅ SUCCESS: " RESET GREEN "%s\n" RESET, message);
}

void print_error(const char* message) {
    printf(BOLD RED "❌ ERROR: " RESET RED "%s\n" RESET, message);
}

void print_info(const char* message) {
    printf(BOLD CYAN "ℹ️  INFO: " RESET CYAN "%s\n" RESET, message);
}

void print_warning(const char* message) {
    printf(BOLD YELLOW "⚠️  WARNING: " RESET YELLOW "%s\n" RESET, message);
}

char* format_time(time_t timestamp) {
    static char buffer[32];
    struct tm *tm_info = localtime(&timestamp);
    strftime(buffer, sizeof(buffer), "%H:%M:%S %d/%m", tm_info);
    return buffer;
}

char* get_uptime(time_t start_time) {
    static char buffer[32];
    time_t now = time(NULL);
    int uptime = (int)(now - start_time);
    
    if (uptime < 60) {
        sprintf(buffer, "%ds", uptime);
    } else if (uptime < 3600) {
        sprintf(buffer, "%dm %ds", uptime/60, uptime%60);
    } else {
        sprintf(buffer, "%dh %dm", uptime/3600, (uptime%3600)/60);
    }
    return buffer;
}

void save_daemon_info(int pid, time_t launch_time) {
    FILE *fp = fopen(PID_FILE, "a");
    if (fp) {
        fprintf(fp, "%d %ld\n", pid, launch_time);
        fclose(fp);
    }
}

int is_process_running(int pid) {
    if (kill(pid, 0) != 0) {
        return 0; 
    }
    
    char stat_path[64];
    sprintf(stat_path, "/proc/%d/stat", pid);
    
    FILE *stat_fp = fopen(stat_path, "r");
    if (!stat_fp) {
        return 0;
    }
    
    char state;
    int dummy_pid;
    char comm[256];
    
    if (fscanf(stat_fp, "%d %s %c", &dummy_pid, comm, &state) == 3) {
        fclose(stat_fp);
        if (state == 'Z') {
            return 0;
        }
        return 1;
    }
    
    fclose(stat_fp);
    return 0;
}

int load_daemon_info(DaemonInfo daemons[], int max_count) {
    FILE *fp = fopen(PID_FILE, "r");
    if (!fp) return 0;
    
    int count = 0;
    while (count < max_count && fscanf(fp, "%d %ld", &daemons[count].pid, &daemons[count].launch_time) == 2) {
        if (is_process_running(daemons[count].pid)) {
            strcpy(daemons[count].status, "RUNNING");
        } else {
            strcpy(daemons[count].status, "STOPPED");
        }
        strcpy(daemons[count].launch_str, format_time(daemons[count].launch_time));
        count++;
    }
    fclose(fp);
    return count;
}

void update_registry(DaemonInfo daemons[], int count) {
    FILE *fp = fopen(PID_FILE, "w");
    if (fp) {
        for (int i = 0; i < count; i++) {
            fprintf(fp, "%d %ld\n", daemons[i].pid, daemons[i].launch_time);
        }
        fclose(fp);
    }
}

void launch_daemon() {
    print_separator();
    printf(BOLD GREEN "🚀 LAUNCHING NEW DAEMON...\n" RESET);
    printf(CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" RESET);

    if (access("./my_daemon", F_OK) != 0) {
        print_error("Daemon binary './my_daemon' not found!");
        printf(RED "   • gcc -o my_daemon my_daemon.c\n" RESET);
        getchar(); getchar(); return;
    }

    if (access("./my_daemon", X_OK) != 0) {
        print_error("Daemon binary is not executable!");
        printf(RED "   • chmod +x my_daemon\n" RESET);
        getchar(); getchar(); return;
    }

    print_info("Binary found and executable, proceeding...");

    time_t launch_time = time(NULL);
    pid_t pid = fork();

    if (pid == 0) {
        execl("./my_daemon", "my_daemon", NULL);
        print_error("Failed to execute daemon binary");
        printf("Error: %s\n", strerror(errno));
        exit(1);
    } else if (pid > 0) {
        printf(BOLD YELLOW "⏳ Waiting for daemon startup...\n" RESET);
        sleep(3);

        FILE *pf = fopen("/tmp/enhanced_daemon.pid", "r");
        if (pf) {
            int daemon_pid;
            if (fscanf(pf, "%d", &daemon_pid) == 1 && is_process_running(daemon_pid)) {
                save_daemon_info(daemon_pid, launch_time);

                char msg[100];
                sprintf(msg, "Daemon launched successfully with PID: %d", daemon_pid);
                print_success(msg);

                printf(BOLD BLUE "📝 Process Details:\n" RESET);
                printf("   • PID: " BOLD "%d\n" RESET, daemon_pid);
                printf("   • Launch Time: " BOLD CYAN "%s\n" RESET, format_time(launch_time));
                printf("   • Status: " BOLD GREEN "RUNNING\n" RESET);
            } else {
                print_error("Daemon not running after launch.");
            }
            fclose(pf);
        } else {
            print_error("PID file not found. Daemon may have failed.");
        }
    } else {
        print_error("Fork failed.");
        printf("Error: %s\n", strerror(errno));
    }

    printf(CYAN "\nPress Enter to continue..." RESET);
    getchar(); getchar();
}

void auto_refresh_status() {
    print_info("Auto-refreshing daemon status...");
    
    DaemonInfo daemons[100];
    int count = load_daemon_info(daemons, 100);
    
    if (count > 0) {
        update_registry(daemons, count);
        print_success("Status refreshed and registry updated");
    }
}

void list_daemons() {
    print_separator();
    printf(BOLD YELLOW "📊 ACTIVE DAEMON LIST\n" RESET);
    printf(CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" RESET);

    auto_refresh_status();
    
    DaemonInfo daemons[100];
    int count = load_daemon_info(daemons, 100);
    
    if (count == 0) {
        print_warning("No daemon registry found");
        printf(YELLOW "   • No daemons have been launched yet\n" RESET);
        printf(YELLOW "   • Use option 1 to launch your first daemon\n" RESET);
        printf(CYAN "\nPress Enter to continue..." RESET);
        getchar();
        getchar();
        return;
    }
    
    printf(BOLD WHITE "┌──────────┬─────────────┬─────────────────┬─────────────────┐\n");
    printf("│   PID    │   Status    │   Launch Time   │    Uptime       │\n");
    printf("├──────────┼─────────────┼─────────────────┼─────────────────┤\n" RESET);
    
    int active_count = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(daemons[i].status, "RUNNING") == 0) {
            printf("│ " BOLD GREEN "%8d" RESET " │ " BOLD GREEN "  RUNNING " RESET "  │ " CYAN "%15s" RESET " │ " BOLD YELLOW "%15s" RESET " │\n", 
                   daemons[i].pid, daemons[i].launch_str, get_uptime(daemons[i].launch_time));
            active_count++;
        } else {
            printf("│ " BOLD RED "%8d" RESET " │ " BOLD RED "  STOPPED " RESET "  │ " CYAN "%15s" RESET " │ " RED "     OFFLINE    " RESET "│\n", 
                   daemons[i].pid, daemons[i].launch_str);
        }
    }
    
    printf(BOLD WHITE "└──────────┴─────────────┴─────────────────┴─────────────────┘\n" RESET);
    printf(BOLD BLUE "\n📈 Summary: " RESET "%d total, " BOLD GREEN "%d active" RESET ", " BOLD RED "%d stopped\n" RESET, 
           count, active_count, count - active_count);
    
    if (count - active_count > 0) {
        printf(BOLD YELLOW "\n💡 Tip: Use option 5 to clean stopped daemons from registry\n" RESET);
    }
    
    printf(CYAN "\nPress Enter to continue..." RESET);
    getchar();
    getchar();
}

void kill_daemon() {
    print_separator();
    printf(BOLD RED "⚡ DAEMON TERMINATION\n" RESET);
    printf(CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" RESET);
    
    DaemonInfo daemons[100];
    int count = load_daemon_info(daemons, 100);
    
    if (count == 0) {
        print_warning("No daemon registry found");
        printf(CYAN "\nPress Enter to continue..." RESET);
        getchar();
        getchar();
        return;
    }
    
    printf(BOLD YELLOW "Current Daemons:\n" RESET);
    int active_found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(daemons[i].status, "RUNNING") == 0) {
            printf("  • PID " BOLD GREEN "%d" RESET " - " GREEN "RUNNING" RESET " (launched %s, uptime: %s)\n", 
                   daemons[i].pid, daemons[i].launch_str, get_uptime(daemons[i].launch_time));
            active_found = 1;
        }
    }
    
    if (!active_found) {
        print_warning("No active daemons found to terminate");
        printf(CYAN "\nPress Enter to continue..." RESET);
        getchar();
        getchar();
        return;
    }
    
    int target;
    printf(BOLD WHITE "\n🎯 Enter PID to terminate: " RESET);
    if (scanf("%d", &target) != 1) {
        print_error("Invalid PID format");
        printf(CYAN "\nPress Enter to continue..." RESET);
        getchar();
        getchar();
        return;
    }
    
    printf(BOLD YELLOW "\n⚠️  Attempting to terminate PID %d...\n" RESET, target);
    
    if (kill(target, SIGTERM) == 0) {
        char success_msg[100];
        sprintf(success_msg, "SIGTERM sent to PID %d successfully", target);
        print_success(success_msg);
        
        printf(BOLD YELLOW "⏳ Waiting for graceful shutdown...\n" RESET);
        int checks = 0;
        int max_checks = 10;
        
        while (checks < max_checks && is_process_running(target)) {
            sleep(1);
            checks++;
            printf("   Checking... (%d/%d)\n", checks, max_checks);
        }
        
        if (!is_process_running(target)) {
            print_info("Process terminated gracefully");
            
            DaemonInfo daemons[100];
            int count = load_daemon_info(daemons, 100);
            update_registry(daemons, count);
            print_info("Registry updated automatically");
        } else {
            print_warning("Process still running, sending SIGKILL...");
            if (kill(target, SIGKILL) == 0) {
                sleep(1);
                if (!is_process_running(target)) {
                    print_info("Process forcefully terminated");
                } else {
                    print_error("Process resistant to termination");
                }
            }
        }
    } else {
        print_error("Failed to terminate daemon - Process may not exist");
    }
    
    printf(CYAN "\nPress Enter to continue..." RESET);
    getchar();
    getchar();
}

void clean_registry() {
    print_separator();
    printf(BOLD CYAN "🧹 REGISTRY CLEANUP\n" RESET);
    printf(CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" RESET);
    
    DaemonInfo daemons[100];
    int count = load_daemon_info(daemons, 100);
    
    if (count == 0) {
        print_info("Registry is already empty");
        printf(CYAN "\nPress Enter to continue..." RESET);
        getchar();
        getchar();
        return;
    }
    
    int cleaned = 0;
    int new_count = 0;
    DaemonInfo active_daemons[100];
    
    for (int i = 0; i < count; i++) {
        if (strcmp(daemons[i].status, "RUNNING") == 0) {
            active_daemons[new_count++] = daemons[i];
        } else {
            cleaned++;
        }
    }
    
    update_registry(active_daemons, new_count);
    
    char success_msg[100];
    sprintf(success_msg, "Cleaned %d stopped daemon(s) from registry", cleaned);
    print_success(success_msg);
    printf(BOLD BLUE "📊 Registry Status:\n" RESET);
    printf("   • Removed: " BOLD RED "%d stopped\n" RESET, cleaned);
    printf("   • Remaining: " BOLD GREEN "%d active\n" RESET, new_count);
    
    printf(CYAN "\nPress Enter to continue..." RESET);
    getchar();
    getchar();
}

void show_system_status() {
    print_separator();
    printf(BOLD MAGENTA "🔍 SYSTEM STATUS OVERVIEW\n" RESET);
    printf(CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" RESET);

    printf(BOLD BLUE "🖥️  System Information:\n" RESET);
    printf("   • Current PID: " BOLD "%d\n" RESET, getpid());
    printf("   • Parent PID: " BOLD "%d\n" RESET, getppid());
    printf("   • User ID: " BOLD "%d\n" RESET, getuid());
    
    time_t now = time(0);
    printf("   • Current Time: " BOLD CYAN "%s" RESET, format_time(now));

    DaemonInfo daemons[100];
    int count = load_daemon_info(daemons, 100);
    
    if (count > 0) {
        int active = 0;
        time_t oldest_launch = now;
        time_t newest_launch = 0;
        
        for (int i = 0; i < count; i++) {
            if (strcmp(daemons[i].status, "RUNNING") == 0) {
                active++;
                if (daemons[i].launch_time < oldest_launch) {
                    oldest_launch = daemons[i].launch_time;
                }
                if (daemons[i].launch_time > newest_launch) {
                    newest_launch = daemons[i].launch_time;
                }
            }
        }
        
        printf(BOLD BLUE "\n📊 Daemon Statistics:\n" RESET);
        printf("   • Total Registered: " BOLD "%d\n" RESET, count);
        printf("   • Currently Active: " BOLD GREEN "%d\n" RESET, active);
        printf("   • Inactive/Stopped: " BOLD RED "%d\n" RESET, count - active);
        
        if (active > 0) {
            printf("   • Oldest Active: " BOLD CYAN "%s\n" RESET, format_time(oldest_launch));
            printf("   • Newest Active: " BOLD CYAN "%s\n" RESET, format_time(newest_launch));
        }
    } else {
        printf(BOLD BLUE "\n📊 Daemon Statistics:\n" RESET);
        printf("   • No daemon registry found\n");
    }
    
    printf(CYAN "\nPress Enter to continue..." RESET);
    getchar();
    getchar();
}

int main() {
    int choice;
    
    while (1) {
        clear_screen();
        print_header();
        print_menu();
        
        printf(BOLD WHITE "\n🎯 Enter your choice" RESET " (1-6): ");
        
        if (scanf("%d", &choice) != 1) {
            print_error("Invalid input! Please enter a number.");
            printf(CYAN "Press Enter to continue..." RESET);
            getchar();
            getchar();
            continue;
        }
        
        switch (choice) {
            case 1: 
                launch_daemon(); 
                break;
            case 2: 
                list_daemons(); 
                break;
            case 3: 
                kill_daemon(); 
                break;
            case 4:
                show_system_status();
                break;
            case 5:
                clean_registry();
                break;
            case 6:
                clear_screen();
                printf(BOLD GREEN "🎉 Thank you for using Daemon Manager!\n" RESET);
                printf(CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" RESET);
                printf(BOLD YELLOW "👋 Goodbye and have a great day!\n" RESET);
                exit(0);
            default: 
                print_error("Invalid choice! Please select 1-6.");
                printf(CYAN "Press Enter to continue..." RESET);
                getchar();
                getchar();
        }
    }
    
    return 0;
}
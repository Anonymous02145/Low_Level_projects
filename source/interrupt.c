#include <signal.h>
#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int is_number(const char *str) {
    while (*str) {
        if (!isdigit(*str++))
            return 0;
    }
    return 1;
}

void read_dir() {
    struct dirent *entry;
    DIR *proc = opendir("/proc");

    if (proc == NULL) {
        perror("opendir /proc");
        _exit(0);
    }

    printf("%-10s %-s\n", "PID", "Process Name");
    printf("--------------------------\n");

    while ((entry = readdir(proc)) != NULL) {
        if (entry->d_type == DT_DIR && is_number(entry->d_name)) {
            char comm_path[256];
            char process_name[256];
            FILE *f;

            snprintf(comm_path, sizeof(comm_path), "/proc/%s/comm", entry->d_name);

            f = fopen(comm_path, "r");
            if (f) {
                if (fgets(process_name, sizeof(process_name), f) != NULL) {
                    // Remove newline
                    process_name[strcspn(process_name, "\n")] = 0;
                    printf("%-10s %s\n", entry->d_name, process_name);
                }
                fclose(f);
            }
        }
    }

    closedir(proc);
}

int main() {
    read_dir();

    char input[50];
    printf("\nEnter the PID to kill: ");
    fgets(input, sizeof(input), stdin);

    
    input[strcspn(input, "\n")] = 0;

    if (is_number(input)) {
        int pid = atoi(input);
        printf("\nKilling process... PID: %d\n", pid);

        if (kill(pid, SIGINT) == 0) {
            printf("Successfully sent SIGINT to process %d\n", pid);
        } else {
            perror("Failed to kill process");
        }
    } else {
        printf("Invalid PID input.\n");
    }

    return 0;
}

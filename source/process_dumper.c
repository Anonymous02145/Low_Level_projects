#include <endian.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

int is_number(const char *str) {
    while (*str) {
        if (!isdigit(*str++))
            return 0;
    }
    return 1;
}

void list_process() {
    struct dirent *dir;
    DIR *proc = opendir("/proc");

    if (proc == NULL) {
        perror("Opening dir /proc");
        _exit(1);
    }

    const char *header = "PID       Process Name\n----------------------\n";
    write(1, header, strlen(header));

    while ((dir = readdir(proc)) != NULL) {
        if (dir->d_type == DT_DIR && is_number(dir->d_name)) {
            char comm_path[256];
            char process_name[256];
            FILE *file;

            snprintf(comm_path, sizeof(comm_path), "/proc/%s/comm", dir->d_name);

            file = fopen(comm_path, "r");
            if (file) {
                if (fgets(process_name, sizeof(process_name), file) != NULL) {
                    process_name[strcspn(process_name, "\n")] = 0;  // Remove newline
                    char output[512];
                    int len = snprintf(output, sizeof(output), "%-10s %s\n", dir->d_name, process_name);
                    write(1, output, len);
                }
                fclose(file);
            }
        }
    }

    closedir(proc);
}

typedef struct {
    char name[256];
    char state[256];
    int pid;
    int ppid;
    unsigned long vm_size;
} ProcessStatus;

int dump_contents(const char *process_id, ProcessStatus *ps) {
    char path[54];
    snprintf(path, sizeof(path), "/proc/%s/status", process_id);

    FILE *file = fopen(path, "r");

    if (file == NULL) {
        perror("File opening error");
        return -1;
    }

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "Name:\t%255s", ps->name) == 1) continue;
        if (sscanf(line, "State:\t%255[^\n]", ps->state) == 1) continue;
        if (sscanf(line, "Pid:\t%d", &ps->pid) == 1) continue;
        if (sscanf(line, "PPid:\t%d", &ps->ppid) == 1) continue;
        if (sscanf(line, "VmSize:\t%lu", &ps->vm_size) == 1) continue;
    }

    fclose(file);
    return 0;
}

void print_status(const ProcessStatus *ps) {
    printf("\nProcess Status:\n");
    printf("Name     : %s\n", ps->name);
    printf("State    : %s\n", ps->state);
    printf("PID      : %d\n", ps->pid);
    printf("PPID     : %d\n", ps->ppid);
    printf("VmSize   : %lu kB\n", ps->vm_size);
}

int main() {
    char proc_id[200];
    printf("Listing processes:\n");
    list_process();
    printf("\nEnter the PID of the process to get more details: ");
    
    if (fgets(proc_id, sizeof(proc_id), stdin) == NULL) {
        fprintf(stderr, "Error reading input.\n");
        return 1;
    }

    // Remove newline from input
    proc_id[strcspn(proc_id, "\n")] = 0;

    if (!is_number(proc_id)) {
        fprintf(stderr, "Invalid PID entered.\n");
        return 1;
    }

    ProcessStatus ps; // FIX: Allocate the struct on stack
    if (dump_contents(proc_id, &ps) == 0) {
        print_status(&ps);
    } else {
        fprintf(stderr, "Error occurred while reading the status of the process %s\n", proc_id);
    }

    return 0;
}

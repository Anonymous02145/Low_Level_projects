
#include <endian.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/syscall.h>
#include "../header_files/utils.h"


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
    char action[50];
    printf("Listing processes:\n");
    list_process();
   
    printf("Enter the action to do - (kill, abort, restart, pause, resume) - [k, a. r, p, rs] : ");
    fgets(action, sizeof(action), stdin);

    action[strcspn(action, "\n")] = 0;

    printf("Enter the pid or the process name : ");
    fgets(proc_id, sizeof(proc_id), stdin);

    proc_id[strcspn(proc_id, "\n")] = 0;

    if(is_number(proc_id)){

        int id = atoi(proc_id);
        printf("user entered pid : %d\n", id);
    }
    else{

        pid_t  identy = getpidbyname(proc_id);
        if(identy != -1){
            printf("pid : %s | name : %d\n", proc_id, identy);
        }
        else {
            printf("process not found \n");
        }
    }

    if (fgets(action, sizeof(action), stdin) == NULL) {
        fprintf(stderr, "Error reading input.\n");
        return 1;
    }

    //Kill :

    if(strcmp(action, "k") == 0 || strcmp(action, "kill") == 0 || strcmp(action, "Kill")) {
        if(is_number(proc_id)){

            int id = atoi(proc_id);
            printf("user entered pid : %d\n", id);

            printf("Killig the process (Pid : %d )\n", id);
            if(syscall(SYS_kill, id, SIGKILL) == 0){
                printf("Killed process {Pid : %d}\n", id);
            }
            else {
                perror("Failed to kill process\n");
            }
        }
        else{
            pid_t number = getpidbyname(proc_id);

            if(kill(number, 0) == 0){
                if(syscall(SYS_kill, number, SIGKILL) == 0){
                    printf("Killed process.. {PID : %d}\n", number);
                }
                else{
                    perror("Failed to kill process\n");
                    printf("{PID : %d}", number);
                }
            }
            else{
                perror("Invalid Process / Invalid pid return");
            }
        }
        
    }
    
    //Abort :

    if(strcmp(action, "a") == 0 || strcmp(action, "abort") == 0 || strcmp(action, "Abort") == 0){
        if(is_number(proc_id)){
            int id = atoi(proc_id);
            printf("user entered pid : %d\n", id);

            printf("Aboring proces... (PID : %d)", id);
            if (kill(id, 0) == 0) {
                if (syscall(SYS_kill, id, SIGABRT) == 0) {
                    printf("Aborted process ... {PID : %d}\n", id);
                }
                else {
                    perror("Error Aborting process ....\n");
                    printf("PID : %d\n", id);
                }
                
            }   
            else{
                perror("Permission not acquired\n");
            }

        }

        else {
            pid_t n = getpidbyname(proc_id);
            if (kill(n, 0) == 0) {
                syscall(SYS_kill, n, SIGABRT);
                printf("Abroted process... (PID : %d)", n);
            }
            else {
                perror("Error Aborting process..\n");
                printf("PID : %d", n);
            }
        }
    }
    
    //Restart :

    if (strcmp(action, "r") == 0 || strcmp(action, "restart") == 0 || strcmp(action, "Restart") == 0) {
        if (is_number(proc_id) == 0) {
            int id = atoi(proc_id);

            printf("user entered pid : %d\n", id);

            printf("Restarting process (PID : %d)", id);
            if (kill(id, 0) == 0) {
                if (syscall(SYS_kill, id, SIGKILL) == 0) {
                    printf("\n");
                }
            
            }

        }
    
    }
    proc_id[strcspn(proc_id, "\n")] = 0;

    if (!is_number(proc_id)) {
        fprintf(stderr, "Invalid PID entered.\n");
        return 1;
    }

    ProcessStatus ps; 
    if (dump_contents(proc_id, &ps) == 0) {
        printf("Status for pid : %s\n", proc_id);
        print_status(&ps);
    } else {
        fprintf(stderr, "Error occurred while reading the status of the process %s\n", proc_id);
    }

    return 0;
}

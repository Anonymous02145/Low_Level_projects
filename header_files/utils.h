#ifndef UTILS_H
#define UTILS_H

#include <sys/time.h>
#include <ctype.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

static int is_it_number(const char *str){
    while(*str){
        if(!isdigit(*str++)){
            return 0;
        }
    }
    return 1;
}


static pid_t getpidbyname(const char *target_name){
    DIR *dir = opendir("/proc");
    if(!dir){
        return -1;
    }

    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        if(entry -> d_type == DT_DIR && is_it_number(entry -> d_name)){
            char comm_path[256];
            snprintf(comm_path, sizeof(comm_path), "/proc/%s/comm", entry -> d_name);

            FILE *file = fopen(comm_path, "r");
            if(file){
                char name[256];
                if(fgets(name, sizeof(name), file)){
                    name[strcspn(name, "\n ")] = 0;
                    if(strcmp(name, target_name) == 0){
                        fclose(file);
                        closedir(dir);
                        return atoi(entry -> d_name);
                    }
                }
                fclose(file);
            }
        }
    }
    closedir(dir);
    return -1;
}

static int restart(int pid){
    char exepath[PATH_MAX];
    char resolved_path[PATH_MAX];

    snprintf(exepath, sizeof(exepath), "/proc/%d/exe", pid);

    ssize_t len = readlink(exepath, resolved_path, sizeof(resolved_path) - 1);

    if (len == -1) {
        perror("error reading the executable path");
        return -1;
    }

    resolved_path[len] = '\0';

    if (kill(pid, 0) == 0) {
        if (syscall(SYS_kill, pid, SIGTERM) == -1) {
            perror("failed to kill process");
            return -1;
        }
    }
    else {
        printf("Permission not aquired");
    }

    sleep(1);
}

#endif 

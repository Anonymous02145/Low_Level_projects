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
    char cmdline[64];
    snprintf(cmdline, sizeof(cmdline), "/proc/%d/cmdline", pid);

    FILE *cmd_file = fopen(cmdline,"r");
    if(!cmd_file){
        perror("Failed to read the command to execute the binary !");
        return -1;
    }

    if (strstr(resolved_path, " (deleted)")) {
        char *del = strstr(resolved_path, " (deleted)");
        if (del) *del = '\0';
    }

    char *args[256] = {0};
    char argsbuf[4096];
    size_t total = fread(argsbuf, 1, sizeof(argsbuf) - 1, cmd_file);
    fclose(cmd_file);
    if (total <= 0) {
        perror("Cmd File empty");
        return -1;
    }
    int i = 0;
    for (size_t j = 0; j < total && i < 255 ; j++) {
        args[i++] = &argsbuf[j];
        while (j < total && argsbuf[j] != '\0') {
            j++;
        }
    
    }

    // contains the arguments : 
    args[i] = NULL;

    char environ_path[64];
    snprintf(environ_path, sizeof(environ_path), "/proc/%d/environ", pid);

    FILE *envi = fopen(environ_path, "r");
    if (!envi) {
        perror("Failed to read environment variables");
        return -1;
    }

    char *env[256] = {0};
    char envbuf[8192];
    total = fread(envbuf, 1, sizeof(envbuf), envi);
    fclose(envi);
    if (total <= 0) {
        perror("Environment variables empty");
        return -1;
    }

    i = 0;
    for (size_t j = 0; j < total && i < 255; j++) {
        env[i++] = &envbuf[j++];
        while (j < total && envbuf[j] != '\0') {
            j++;
        
        }
    }

    // contains the environment variables: 
    env[i] = NULL;

    snprintf(exepath, sizeof(exepath), "/proc/%d/exe", pid);

    ssize_t len = readlink(exepath, resolved_path, sizeof(resolved_path) - 1);

    if (len == -1) {
        perror("error reading the executable path");
        return -1;
    }

    //has the path to executable binary :
    resolved_path[len] = '\0';

    if (kill(pid, 0) == 0) {
        if (syscall(SYS_kill, pid, SIGTERM) == -1) {
            perror("failed to kill process");
            return -1;
        }
        else{
            printf("\n");
        }
    }
    else {
        printf("Permission not aquired");
    }

    sleep(1);

    if (syscall(SYS_execve, resolved_path, args, env) == 0) {
        printf("Restarted process : path_to_binary : %s || Arguments the process took : %s || Environment variables : %s ", resolved_path, *args, *env);

    }

    return 0;
}

#endif 

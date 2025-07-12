#include <stdio.h>
#include <threads.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>


#define Time 1000
#define NUM_PROCS 2

int main(){
    pid_t pids[NUM_PROCS];
    int i;

    for(i = 0 ; i < NUM_PROCS; i++){
        pids[i] = fork();

        if(pids[i]  == 0) {
           
            while(1){
                printf("Child 1 running  %i .....  Pid : %u", i, getpid());
                usleep(200 * 1000);
            
            }
            exit(0);
        }
   
    }
    sleep(1);
    for(i = 0; i < NUM_PROCS; i++){
        kill(pids[i], SIGSTOP);
    }

    int current = 0;
    while(1){
        kill(pids[current], SIGSTOP);
        usleep(Time * 1000);
        kill(pids[current], SIGCONT);

        current = (current + 1) % NUM_PROCS;
    }

    for(i = 0; i < NUM_PROCS; i++){
        waitpid(pids[current], NULL, 0);
    }

    return 0;
}
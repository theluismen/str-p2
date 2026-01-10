#define _POSIX_C_SOURCE 199309L // Required for Real-Time Signals

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * Signal handler 
 */
void rt_handler(int sig, siginfo_t *si, void *ucontext){
    //Check if the signal has data
    int value = si->si_value.sival_int;
    if (value != NULL){
        printf("Caught signal %d. Data recieved: %d\n", sig, value);
    } else {
        printf("Caught signal %d. NO DATA recieved\n", sig);
    }
}

int main(){
    struct sigaction sa;
    union sigval value;
    pid_t my_pid = getpid();   //to send the signal to ourselves

    //Handler Set-Up
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = rt_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGRTMIN, &sa, NULL) == -1){
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sleep(1);

    //Send first signal with data
    value.sival_int = 42;
    printf("Sending SIGRTMIN with value %d\n", value.sival_int);

    if (sigqueue(my_pid, SIGRTMIN, value) == -1){
        perror("sigqueue");
        exit(EXIT_FAILURE);
    }

    sleep(1);

    //Send second signal with diff data
    value.sival_int = 100;
    printf("Sending SIGRTMIN with value %d\n", value.sival_int);

    if (sigqueue(my_pid, SIGRTMIN, value) == -1){
        perror("sigqueue2");
        exit(EXIT_FAILURE);
    }

    sleep(1);

    //Send second signal with diff data
    value.sival_int = 0;
    printf("Sending SIGRTMIN with NO value %d\n", value.sival_int);

    if (sigqueue(my_pid, SIGRTMIN, value) == -1){
        perror("sigqueue2");
        exit(EXIT_FAILURE);
    }


    sleep(1);
    printf("Exiting after success\n");

    return 0;
}



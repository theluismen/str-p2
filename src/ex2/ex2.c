#define _POSIX_C_SOURCE 199309L // Required for POSIX timers

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>


int i = 0;

/**
 * Signal Handler : Runs a task every time the timer expires
*/
void my_handler(int signum){
    i++; 
    printf("Caught signal %d - Timer expired! Execution number %d \n", signum, i);
}

int main(){
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    struct sigaction sa;

    // --- Set up the handler
    sa.sa_handler = my_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    // --- Link handler to the SIGALRM signal
    if(sigaction(SIGALRM, &sa, NULL) == -1){
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // --- Timer creation
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGALRM;
    sev.sigev_value.sival_ptr = &timerid;

    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    } else{
        printf("Timer created successfully.\n");
    }

    // --- Configure Interval and Start
    its.it_interval.tv_sec = 2;
    its.it_interval.tv_nsec = 0;

    // set up a 1 sec. initial delay
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;

    // start the timer
    if (timer_settime(timerid, 0, &its, NULL) == -1){
        perror("timer_settime");
        exit(EXIT_FAILURE);
    } else {
        printf("Timer started... Waiting for signals \n");
    }

    while(true){
        pause();    //suspension of the process until a signal arrives.
    }

    return 0;
}

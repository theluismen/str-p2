#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>

// UNCOMMENT/COMMENT THE BLOW LINE TO FIX PRIORITY INVERSION
#define USE_PRIO_INHERITANCE

pthread_mutex_t lock;

/**
 * Function to consume CPU
 */
void busy_work(long iterations){
    volatile long i;
    for (i=0; i<iterations; i++);
}

/**
 * Low priority thread
 */
void* task_low(void* arg){
    printf("LOW: Trying to lock mutex\n");
    pthread_mutex_lock(&lock);
    printf("LOW: Locked mutex\n");

    busy_work(5000000000); //uses cpu for a time

    printf("LOW: unlocking mutex\n");
    pthread_mutex_unlock(&lock);

    return NULL;
}

/**
 * Medium priority thread
 */
void* task_medium(void* arg){
    //wait a bit for the low to lock
    sleep(1);
    printf("MED: Running (I don't need the lock, I just burn CPU)...\n");

    //burn cpu long enough to starve Low if inversion happens
    busy_work(9000000000);

    printf("MED: Finished\n");
    return NULL;
}

void* task_high(void* arg){
    //wait until low and medium have started
    sleep(2);
    printf("HIGH: Trying to lock mutex\n");

    pthread_mutex_lock(&lock); 
    printf("HIGH: LOCKED mutex Finally!. Critical section.\n");
    pthread_mutex_unlock(&lock);

    printf("HIGH: Finished\n");
    return NULL;
}

int main(){
    cpu_set_t my_set;        // Define a set of CPUs
    CPU_ZERO(&my_set);       // Clear the set
    CPU_SET(0, &my_set);     // Add CPU 0 to the set
    // Force the current process (and all future threads) to run ONLY on CPU 0
    if (sched_setaffinity(0, sizeof(cpu_set_t), &my_set) == -1) {
        perror("sched_setaffinity");
    } else {
        printf("--- CPU Affinity set to CORE 0 (Simulating Single Core) ---\n");
    }


    pthread_t t_low, t_med, t_high;
    pthread_mutexattr_t mutex_attr;
    struct sched_param param;

    //initialize mutex attributes
    pthread_mutexattr_init(&mutex_attr);

    #ifdef USE_PRIO_INHERITANCE
        printf("ENABELING PRIORITY INHERITANCE (INVERSION WON'T HAPPEN)\n");
        pthread_mutexattr_setprotocol(&mutex_attr, PTHREAD_PRIO_INHERIT);
    #else
        printf("NO PRIORITY INHERITANCE (INVERSION WILL HAPPEN)\n");
    #endif

    pthread_mutex_init(&lock, &mutex_attr);

    // Create threads
    
    //LOW thread (priority 10)
    pthread_create(&t_low, NULL, task_low, NULL);
    param.sched_priority = 10;
    pthread_setschedparam(t_low, SCHED_FIFO, &param);

    //MEDIUM thread (priority 20)
    pthread_create(&t_med, NULL, task_medium, NULL);
    param.sched_priority = 20;
    pthread_setschedparam(t_med, SCHED_FIFO, &param);

    //HIGH thread (priority 30)
    pthread_create(&t_high, NULL, task_high, NULL);
    param.sched_priority = 30;
    pthread_setschedparam(t_high, SCHED_FIFO, &param);

    pthread_join(t_low, NULL);
    pthread_join(t_med, NULL);
    pthread_join(t_high, NULL);

    pthread_mutex_destroy(&lock);
    return 0;
}
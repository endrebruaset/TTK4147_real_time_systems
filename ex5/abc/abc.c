#define _GNU_SOURCE


#include "io.h"
#include <pthread.h>
#include <time.h>
#include <sched.h>

int A_CHANNEL = 1;
int B_CHANNEL = 2;
int C_CHANNEL = 3;

const struct timespec wait = { 
    .tv_sec = 0, 
    .tv_nsec = 5000
};

struct timespec rem;


int set_cpu(int cpu_number){
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);

    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void* busy_wait(void* args) {
    set_cpu(0);
    int channel = *(int*)args;

    while(1) {
        if(!io_read(channel)) {
            io_write(channel, 0);
            nanosleep(&wait, &rem);
            io_write(channel, 1);
        }
    }
}

void* disturbance(void* args) {
    set_cpu(0);

    while(1) {
        asm volatile("" ::: "memory");
    }
}

int main() {
    io_init();

    pthread_t A_thread, B_thread, C_thread;
    pthread_create(&A_thread, NULL, busy_wait, &A_CHANNEL);
    pthread_create(&B_thread, NULL, busy_wait, &B_CHANNEL);
    pthread_create(&C_thread, NULL, busy_wait, &C_CHANNEL);

    pthread_t disturbance_threads[5];
    for (int i = 0; i < 5; i++) {
        pthread_create(&disturbance_threads[i], NULL, disturbance, NULL);
    }

    pthread_join(A_thread, NULL);
    pthread_join(B_thread, NULL);
    pthread_join(C_thread, NULL);

    for (int i = 0; i < 5; i++) {
        pthread_join(disturbance_threads[i], NULL);
    }

    return 0;
}
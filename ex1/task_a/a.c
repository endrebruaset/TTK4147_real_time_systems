#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include "timespec.h"

void busy_wait(struct timespec t);
// void busy_wait(int sec);

const struct timespec wait = {
    .tv_sec = 1, 
    .tv_nsec = 0
};

struct timespec rem;

int main() {
    busy_wait(wait);

    // busy_wait(1);

    // sleep(1)
    // usleep(1000*1000);
    // nanosleep(&wait, &rem);

    return 0;
}

void busy_wait(struct timespec t) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec then = timespec_add(now, t);

    while(timespec_cmp(now, then) < 0){
        for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

// void busy_wait(int wait_seconds) {
//     struct tms now;
//     times(&now);
//     int end_time = wait_seconds*100;

//     while(now.tms_utime + now.tms_stime < end_time){
//         for(int i = 0; i < 10000; i++){}
//         times(&now);
//     }
// }
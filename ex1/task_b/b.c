#include <x86intrin.h>
#include "time.h"
#include <sys/times.h>

#include <string.h>
#include <stdio.h>
#include <stdint.h>

/*
int main(void){
    //struct timespec now;
    struct tms now;

    for (int i = 0; i < 10*1000*1000; i++){
        //__rdtsc();
        // clock_gettime(CLOCK_MONOTONIC, &now);
        times(&now);
    }
    return 0;
}
*/

int main(void){
    int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);
    for(int i = 0; i < 10*1000*1000; i++){
        

        // uint64_t t1 = __rdtsc();
        // uint64_t t2 = __rdtsc();

        // int ns = (t2 - t1)/2.66;



        struct timespec t1;
        struct timespec t2;
        clock_gettime(CLOCK_MONOTONIC, &t1);
        clock_gettime(CLOCK_MONOTONIC, &t2);

        int ns = t2.tv_nsec - t1.tv_nsec;


        // Blir bare 0 pga. dårlig oppløsning
        // struct tms t1;
        // struct tms t2;

        // times(&t1);
        // times(&t2);

        // int ns = ;

        if(ns >= 0 && ns < ns_max){
            histogram[ns]++;
        }
    }

    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }
}
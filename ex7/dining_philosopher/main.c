//usr/bin/clang "$0" -o dining_philosophers -std=gnu11 -g -O3 -lpthread && exec ./dining_philosophers "$@"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>


const int N = 5;
pthread_mutex_t forks[N];
sem_t sit_at_table;


int left(int i) { return i; }
int right(int i) { return (i + 1) % N; }


void* dine(void* args) {
    int i = *(int*)(args);

    while(1) {
        sem_wait(&sit_at_table);
        printf("Philosopher %i is thinking... \n\r", i);

        pthread_mutex_lock(&forks[left(i)]);
        sleep(1);
        pthread_mutex_lock(&forks[right(i)]);

        printf("Philosopher %i is eating... \n\r", i);
        sleep(1);

        pthread_mutex_unlock(&forks[left(i)]);
        pthread_mutex_unlock(&forks[right(i)]);

        sem_post(&sit_at_table);
        sleep(1);
    }
}


int main() {
    for (int i = 0; i < N; ++i) {
        pthread_mutex_init(&forks[i], NULL);
    }
    sem_init(&sit_at_table, 0, N-1);
    
    pthread_t philosophers[N];
    int ids[N] = {0, 1, 2, 3, 4};
    for (int i = 0; i < N; ++i) {
        pthread_create(&philosophers[i], NULL, dine, &ids[i]);
    }
    for (int i = 0; i < N; ++i) {
        pthread_join(philosophers[i], NULL);
    }

    for (int i = 0; i < N; ++i) {
        pthread_mutex_destroy(&forks[i]);
    }
}
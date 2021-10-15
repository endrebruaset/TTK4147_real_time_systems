#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>


long shared_number = 0;
// sem_t semaphore;


void* fn(void* args){
    long own_number = 0;

    for (int i = 0; i < 50000000; i++) {
        // sem_wait(&semaphore);
        shared_number++;
        // sem_post(&semaphore);

        own_number++;
    }

    // sem_wait(&semaphore);
    printf("Shared number: %li \n\r Own number: %li\n\r", shared_number, own_number);
    // sem_post(&semaphore);

    return NULL;
}


int main() {
    // sem_init(&semaphore, 0, 1);

    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, fn, NULL);
    pthread_create(&thread2, NULL, fn, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // sem_destroy(&semaphore);
}
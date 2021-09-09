#include "array.h"
#include <stdio.h>


int main() {
    long capacity = 2;
    Array small_array = array_new(capacity);

    for (long i = 0; i < 100; i++) {
        long old_capacity = small_array.capacity;

        array_insertBack(&small_array, i);

        if (old_capacity != small_array.capacity) {
            printf("%p\n", small_array.data);
        }
    }

    array_print(small_array);
    array_destroy(small_array);

    return 0;
}
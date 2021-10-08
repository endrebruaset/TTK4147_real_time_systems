#include "io.h"

#include <rtdk.h>
#include <sys/mman.h>
#include <sched.h>
#include <pthread.h>

#include <native/task.h>
#include <native/timer.h>


// Hello world
RT_TASK hello_task;
RTIME period_ns = 1000*1000*1000;

void hello_world(void *arg){
	rt_task_set_periodic(NULL, TM_NOW, period_ns);

	while(1) {
		rt_printf("%s\n", (char*)arg);

		rt_task_wait_period(NULL);
	}

	return;
}


// Response tasks
int A_CHANNEL = 1;
int B_CHANNEL = 2;
int C_CHANNEL = 3;

void respond_to_test(void* args) {
    int channel = *(int*)args;

	RTIME wait_ns = 5000;
	RTIME max_duration = 40000000000; // 1 minute timeout
	RTIME end_time = rt_timer_read() + max_duration;

    while(1) {
        if(!io_read(channel)) {
            io_write(channel, 0);
			rt_timer_spin(wait_ns);
            io_write(channel, 1);
        }

        if (rt_timer_read() > end_time) {
			rt_printf("Time expired\n");
			rt_task_delete(NULL);
		}
		if (rt_task_yield()) {
			rt_printf("Task failed to yield\n");
			rt_task_delete(NULL);
		}
    }
}

void periodic_respond_to_test(void* args) {
	RTIME period_ns = 100000; // 100 us
	rt_task_set_periodic(NULL, TM_NOW, period_ns);

	int channel = *(int*)args;

	RTIME wait_ns = 5000;
	RTIME max_duration = 40000000000; // 1 minute timeout
	RTIME end_time = rt_timer_read() + max_duration;

	while(1) {
		if(!io_read(channel)) {
            io_write(channel, 0);
			rt_timer_spin(wait_ns);
            io_write(channel, 1);
        }

		if (rt_timer_read() > end_time) {
			rt_printf("Time expired\n");
			rt_task_delete(NULL);
		}

		rt_task_wait_period(NULL);
	}
}


// Disturbances
int set_cpu(int cpu_number){
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);

    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void* disturbance(void* args) {
    set_cpu(0);

    while(1) {
        asm volatile("" ::: "memory");
    }
}


int main(void) {
	mlockall(MCL_CURRENT | MCL_FUTURE);
	io_init();
	rt_print_auto_init(1);

	/* // Hello world
	rt_task_create(&hello_task, "Hello", 0, 50, T_CPU(0) | T_JOINABLE);
	rt_task_start(&hello_task, &hello_world, "Hello world!");
	rt_task_join(&hello_task);
	*/

	// Response Xenomai tasks
	RT_TASK task_A, task_B, task_C;

	rt_task_create(&task_A, "task_A", 0, 50, T_CPU(0) | T_JOINABLE);
	rt_task_create(&task_B, "task_B", 0, 50, T_CPU(0) | T_JOINABLE);
	rt_task_create(&task_C, "task_C", 0, 50, T_CPU(0) | T_JOINABLE);

	rt_task_start(&task_A, &periodic_respond_to_test, &A_CHANNEL);
	rt_task_start(&task_B, &periodic_respond_to_test, &B_CHANNEL);
	rt_task_start(&task_C, &periodic_respond_to_test, &C_CHANNEL);

	// Disturbance pthreads
	int number_of_disturbances = 5;

	pthread_t disturbance_threads[number_of_disturbances];
    for (int i = 0; i < number_of_disturbances; i++) {
        pthread_create(&disturbance_threads[i], NULL, disturbance, NULL);
    }

	// Join threads
	rt_task_join(&task_A);
	rt_task_join(&task_B);
	rt_task_join(&task_C);

    for (int i = 0; i < number_of_disturbances; i++) {
        pthread_join(disturbance_threads[i], NULL);
    }


	return 0;
}

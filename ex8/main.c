#include <rtdk.h>
#include <sys/mman.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <native/task.h>
#include <native/timer.h>


void busy_wait(int delay) {
	delay *= 100;
	for (; delay >0; delay--) {
		rt_timer_spin(1000);
	}
}


// Task A: Synchronization 
/*
RT_SEM semaphore;

void wait_for_broadcast(void* args) {
	int number = *(int*)(args);
	
	rt_printf("Waiting for signal\n");
	rt_sem_p(&semaphore, TM_INFINITE);
	rt_printf("Task %i received signal\n", number);
}

void broadcast(void* args) {
	rt_printf("Broadcasting semaphore\n");
	rt_sem_broadcast(&semaphore);
}
*/


// Task B and C: Priority Inversion
/*
// RT_SEM resource;
RT_MUTEX resource;
RT_SEM barrier;

void low(void* args) {
	rt_sem_p(&barrier, TM_INFINITE);

	rt_printf("Low priority trying to allocate resource\n");
	// rt_sem_p(&resource, TM_INFINITE);
	rt_mutex_acquire(&resource, TM_INFINITE);

	struct rt_task_info temp; 
	rt_task_inquire(NULL, &temp);
	rt_printf("Low priority task starting work with resource, current priority is: %i \n", temp.cprio);

	busy_wait(3);

	rt_task_inquire(NULL, &temp);
	rt_printf("Low priority task finished work with resource, current priority is: %i \n", temp.cprio);

	// rt_sem_v(&resource);
	rt_mutex_release(&resource);
}

void medium(void* args) {
	rt_sem_p(&barrier, TM_INFINITE);

	rt_task_sleep(100*1000);
	rt_printf("Medium priority task starting work\n");
	busy_wait(3);
	rt_printf("Medium priority task finished work\n");
}

void high(void* args) {
	rt_sem_p(&barrier, TM_INFINITE);

	rt_task_sleep(200*1000);
	rt_printf("High priority trying to allocate resource\n");
	// rt_sem_p(&resource, TM_INFINITE);
	rt_mutex_acquire(&resource, TM_INFINITE);

	struct rt_task_info temp; 
	rt_task_inquire(NULL, &temp);
	rt_printf("High priority task starting work with resource, current priority is: %i \n", temp.cprio);

	busy_wait(2);

	rt_task_inquire(NULL, &temp);
	rt_printf("High priority task finished work with resource, current priority is: %i \n", temp.cprio);

	// rt_sem_v(&resource);
	rt_mutex_release(&resource);
}

void broadcast(void* args) {
	rt_printf("Broadcasting barrier\n");
	rt_sem_broadcast(&barrier);
}
*/


// Task D and E: Deadlock and Priority Ceiling
#define LOW_PRIORITY 10
#define HIGH_PRIORITY 50
#define A_PRIORITY HIGH_PRIORITY + 1
#define B_PRIORITY HIGH_PRIORITY + 1

int max(int a, int b) { return a > b ? a : b; }

typedef struct {
	RT_MUTEX* mutex;
	int priority;
} ICPP_MUTEX;

typedef struct {
	RT_TASK* task;
	int base_priority;
	int current_priority;
	void* function;
} ICPP_TASK;


RT_MUTEX A;
RT_MUTEX B;
ICPP_MUTEX icpp_mutex_A = {&A, A_PRIORITY};
ICPP_MUTEX icpp_mutex_B = {&A, B_PRIORITY};
RT_SEM barrier;


void icpp_lock(ICPP_MUTEX* mutex, ICPP_TASK* task) {
	rt_mutex_acquire(mutex->mutex, TM_INFINITE);

	int new_priority = max(task->current_priority, mutex->priority);
	rt_task_set_priority(task->task, new_priority);
	task->current_priority = new_priority;
}

void icpp_unlock(ICPP_MUTEX* mutex, ICPP_TASK* task) {
	rt_mutex_release(mutex->mutex);

	// Priority should be set to highest priority of mutexes currently held, 
	// but in this case we release all mutexes at the same time. Could be 
	// implemented with a boolean array where each element i says wether 
	// mutex with index i is currently held by the task.
	rt_task_set_priority(task->task, task->base_priority);
	task->current_priority = task->base_priority;
}

void low(void* args) {
	ICPP_TASK self = *(ICPP_TASK*)(args);

	rt_sem_p(&barrier, TM_INFINITE);

	rt_printf("Low priority task has priority %i \n", self.current_priority);
	rt_printf("Low priority trying to acquire mutex A\n");
	icpp_lock(&icpp_mutex_A, &self);
	rt_printf("Low priority task has priority %i \n\n", self.current_priority);

	busy_wait(3);

	rt_printf("Low priority task has priority %i \n", self.current_priority);
	rt_printf("Low priority trying to acquire mutex B\n");
	icpp_lock(&icpp_mutex_B, &self);
	rt_printf("Low priority task has priority %i \n\n", self.current_priority);

	busy_wait(3);

	rt_printf("Low priority task has priority %i \n", self.current_priority);
	rt_printf("Low priority releasing mutexes \n");
	icpp_unlock(&icpp_mutex_B, &self);
	icpp_unlock(&icpp_mutex_A, &self);
	rt_printf("Low priority task has priority %i \n\n", self.current_priority);

	busy_wait(1);
}

void high(void* args) {
	ICPP_TASK self = *(ICPP_TASK*)(args);

	rt_sem_p(&barrier, TM_INFINITE);

	rt_task_sleep(100*1000);
	
	rt_printf("High priority task has priority %i \n", self.current_priority);
	rt_printf("High priority trying to acquire mutex B\n");
	icpp_lock(&icpp_mutex_B, &self);
	rt_printf("High priority task has priority %i \n\n", self.current_priority);

	busy_wait(1);

	rt_printf("High priority task has priority %i \n", self.current_priority);
	rt_printf("High priority trying to acquire mutex A\n");
	icpp_lock(&icpp_mutex_A, &self);
	rt_printf("High priority task has priority %i \n\n", self.current_priority);

	busy_wait(2);

	rt_printf("High priority task has priority %i \n", self.current_priority);
	rt_printf("High priority releasing mutexes \n");
	icpp_unlock(&icpp_mutex_A, &self);
	icpp_unlock(&icpp_mutex_B, &self);
	rt_printf("High priority task has priority %i \n\n", self.current_priority);

	busy_wait(1);
}

void broadcast(void* args) {
	rt_printf("Broadcasting barrier\n\n");
	rt_sem_broadcast(&barrier);
}


int main() {
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);

	// Task A 
/*
	rt_sem_create(&semaphore, "semaphore", 0, S_PRIO);
	
	RT_TASK tasks[3];
	rt_task_create(&tasks[0], "Task 1", 0, 50, T_CPU(0));
	rt_task_create(&tasks[1], "Task 2", 0, 50, T_CPU(0));
	rt_task_create(&tasks[2], "Task 3", 0, 99, T_CPU(0));
	
	int ids[2] = {1, 2};
	rt_task_start(&tasks[0], &wait_for_broadcast, &ids[0]);
	rt_task_start(&tasks[1], &wait_for_broadcast, &ids[1]);

	rt_timer_spin(100*1000);
	rt_task_start(&tasks[2], &broadcast, NULL);
	rt_timer_spin(100*1000);
	
	rt_sem_delete(&semaphore);
*/

	// Task B and C
/*
	// rt_sem_create(&resource, "resource", 1, S_PRIO);
	rt_mutex_create(&resource, "resource");
	rt_sem_create(&barrier, "barrier", 0, S_FIFO);
	
	RT_TASK task_low, task_medium, task_high;
	rt_task_create(&task_low, "Task 1", 0, 10, T_CPU(0) | T_JOINABLE);
	rt_task_create(&task_medium, "Task 2", 0, 50, T_CPU(0) | T_JOINABLE);
	rt_task_create(&task_high, "Task 3", 0, 90, T_CPU(0) | T_JOINABLE);
	
	rt_task_start(&task_low, &low, NULL);
	rt_task_start(&task_medium, &medium, NULL);
	rt_task_start(&task_high, &high, NULL);

	RT_TASK broadcaster;
	rt_task_create(&broadcaster, "broadcaster", 0, 99, T_CPU(0) | T_JOINABLE);
	rt_timer_spin(100*1000);
	rt_task_start(&broadcaster, &broadcast, NULL);
	rt_timer_spin(100*1000);

	rt_task_join(&task_low);
	rt_task_join(&task_medium);
	rt_task_join(&task_high);
	rt_task_join(&broadcaster);
	
	rt_sem_delete(&barrier);
	// rt_sem_delete(&resource);
	rt_mutex_delete(&resource);
*/
	
	// Task D
	rt_mutex_create(&A, "resource A");
	rt_mutex_create(&B, "resource B");
	rt_sem_create(&barrier, "barrier", 0, S_FIFO);

	RT_TASK task_low, task_high;
	ICPP_TASK icpp_task_low = {&task_low, LOW_PRIORITY, LOW_PRIORITY, &low};
	ICPP_TASK icpp_task_high = {&task_high, HIGH_PRIORITY, HIGH_PRIORITY, &high};

	rt_task_create(icpp_task_low.task, "Low priority task", 0, icpp_task_low.base_priority, T_CPU(0) | T_JOINABLE);
	rt_task_create(icpp_task_high.task, "High priority task", 0, icpp_task_high.base_priority, T_CPU(0) | T_JOINABLE);


	rt_task_start(icpp_task_low.task, icpp_task_low.function, &icpp_task_low);
	rt_task_start(icpp_task_high.task, icpp_task_high.function, &icpp_task_high);

	RT_TASK broadcaster;
	rt_task_create(&broadcaster, "broadcaster", 0, 90, T_CPU(0) | T_JOINABLE);
	rt_timer_spin(100*1000);
	rt_task_start(&broadcaster, &broadcast, NULL);
	rt_timer_spin(100*1000);

	rt_task_join(icpp_task_low.task);
	rt_task_join(icpp_task_high.task);
	rt_task_join(&broadcaster);

	rt_sem_delete(&barrier);
	rt_mutex_delete(&A);
	rt_mutex_delete(&B);

	return 0;
}

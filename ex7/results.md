### Task A
> Thread 1: 
Shared number: 50842713 
Own number: 50000000

> Thread 2:
Shared number: 51382215 
Own number: 50000000

We can observe that the shared number is only actually incremented about half the amount of times it was supposed to.
This happens because reading and writing to the shared variable isn't atomic. That means that one thread can read the 
variable value in between the other thread reading and writing, causing the increment to only happen once instead of twice.


### Task B
The ThreadSanitizer identifies the problem as a data race when trying to increment the shared variable.


### Task C
> Thread 1:
Shared number: 99978617 
Own number: 50000000

> Thread 2:
Shared number: 100000000 
Own number: 50000000

> Timing with synchronization:
real    1m21,964s
user    1m50,258s
sys     0m50,622s

> Timing without synchronization:
real    0m0,187s
user    0m0,365s
sys     0m0,004s

The program with synchronization uses the most system- and user time. The real time used is lower than the sum of the other two
because user and system time is counted for each thread, then added together.


### Task D
Becuase the double check depends on checking the pointer to the singleton, some threads will think the singleton is done initializing
even though only the malloc() operation has been performed.


### Task E
The ThreadSanitizer identifies a data race when some threads are printing the singlton values. This happens becuase of the problem
stated in Task D.
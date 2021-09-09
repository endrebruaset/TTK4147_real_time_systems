### Task A
##### x_dim = 100
> Memory fylles opp (8.2 GB) og Swap går til 2.4 GB. Memory er hovedminne (RAM), mens Swap er virtuelt minne.

##### x_dim = 1000
> Allokeringen tar ca. 4 GB.

##### x_dim = 10000
> Allokeringen tar ca. 0.5 GB.

##### x_dim = 10000 med skriving til minnet
> Samme utfall som med x_dim = 100. Grunnen til at det skjer ulikt uten skriving til minnet kan være at kompilatoren optimaliserer bort allokeringen av minnet som ikke brukes. 


### Task B
##### Running without memory error detection
The code runs as if there was no error when we do not try to read the element outside the array. If we use array_print(), we get the following error:
    malloc.c:2401: sysmalloc: Assertion `(old_top == initial_top (av) && old_size == 0) || ((unsigned long) (old_size) >= MINSIZE && prev_inuse (old_top) && ((unsigned long) old_end & (pagesize - 1)) == 0)' failed.

##### Running with AddressSanitizer
When the writing to the address outside the allocated memory happens the memory error detection gives a heap-buffer-overflow error.


### Task D
##### 2x
00 00 xx xx xx xx xx xx xx xx xx xx xx xx xx ...
xx xx 00 00 00 00 xx xx xx xx xx xx xx xx xx ...
xx xx xx xx xx xx 00 00 00 00 00 00 00 00 xx ...

##### 1.5x
00 00 xx xx xx xx xx xx xx xx xx xx xx xx xx ...
xx xx 00 00 00 xx xx xx xx xx xx xx xx xx xx ...
xx xx xx xx xx 00 00 00 00 xx xx xx xx xx xx ...
xx xx xx xx xx xx xx xx xx 00 00 00 00 00 00 ...

##### Test
The position of the data array after each reservation for insertion of 100 elements:
2x:
0x603000000010
0x606000000020
0x60c000000040
0x611000000040
0x615000000080
0x619000000580

1.5x:
0x603000000010
0x603000000040
0x604000000010
0x607000000090
0x60b000000040
0x60e000000040
0x611000000040
0x613000000040
0x615000000080
0x617000000080
0x619000000580
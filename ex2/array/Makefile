# program executable name
TARGET = array

# compiler flags
CFLAGS = -g -Wall -fsanitize=address

# linker flags
LDFLAGS = -g -fsanitize=address

# list of sources
SOURCES = $(shell find -name "*.c")

# default rule, to compile everything
all: $(TARGET)

# define object files
OBJECTS = $(SOURCES:.c=.o)

# link programs
$(TARGET): $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^

# compile
%.o : %.c
	gcc $(CFLAGS) -c -o $@ $<

# cleaning
clean:
	rm -f $(TARGET) $(OBJECTS)
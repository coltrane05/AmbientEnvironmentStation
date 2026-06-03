
CC = gcc
CFLAGS = -Wall -Wextra -g

all: memory_regions pointer_arithmetic

memory_regions: memory_regions.o 
	$(CC) $(CFLAGS) -o memory_regions memory_regions.o

pointer_arithmetic: pointer_arithmetic.o
	$(CC) $(CFLAGS) -o pointer_arithmetic pointer_arithmetic.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
		rm -f memory_regions *.o

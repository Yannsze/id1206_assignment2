CC = gcc
CFLAGS = -Wall -Wextra -O0
TARGET = hw2_mmap_faults

all: $(TARGET)

$(TARGET): hw2_mmap_faults.c
	$(CC) $(CFLAGS) hw2_mmap_faults.c -o $(TARGET)

run: all
	./$(TARGET) 4096 1

clean:
	rm -f $(TARGET)

.PHONY: all run clean

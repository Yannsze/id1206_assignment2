CC = gcc
CFLAGS = -Wall -Wextra -O0
USER_TARGETS = hw2_mmap_faults hw2_page_reclamation hw3-6meminfo

all: user_programs

user_programs: $(USER_TARGETS)

hw2_mmap_faults: hw2_mmap_faults.c
	$(CC) $(CFLAGS) -o $@ $^

# -lpthread is required here for multithreading
hw2_page_reclamation: hw2_page_reclamation.c
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

hw3-6meminfo: hw3-6meminfo.c
	$(CC) $(CFLAGS) -o $@ $^



clean:
	rm -f $(USER_TARGETS) *.o *.zip

zip: clean
	zip -r hw2_submission.zip . -x "*.git*" "*.DS_Store"

run: all
	./hw2_mmap_faults 4096 1
	./hw2_page_reclamation 64 20
	./hw3-6meminfo 10
   
.PHONY: all user_programs clean zip run

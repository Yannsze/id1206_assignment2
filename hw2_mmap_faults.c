
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>

int main(int argc, char **argv) {

    if (argc != 3) {
        printf("Usage: %s <num_pages> <option>\n", argv[0]);
        printf("Option 1: Normal pages\n");
        printf("Option 2: Huge pages\n");
        return 1;
    }

    int num_pages = atoi(argv[1]);
    int option = atoi(argv[2]);
    int page_size = getpagesize();

    if (num_pages <= 0 || (option != 1 && option != 2)) {
        printf("Invalid number of pages or option.\n");
        return 1;
    }

    size_t total_size = (size_t)num_pages * page_size;

    printf("Allocating %d pages of %d bytes\n",
           num_pages, page_size);

    int flags = MAP_PRIVATE | MAP_ANONYMOUS;

    if (option == 2) {
        flags |= MAP_HUGETLB;
        printf("Using huge pages\n");
    } else {
        printf("Using normal pages\n");
    }

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    char *addr = mmap(NULL, total_size,
                      PROT_READ | PROT_WRITE,
                      flags, -1, 0);

    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    char c = 'a';

    for (int i = 0; i < num_pages; i++) {
        addr[(size_t)i * page_size] = c;
        c++;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Elapsed time: %.6f seconds\n", elapsed);

    for (int i = 0; i < num_pages && i < 16; i++) {
        printf("%c ", addr[(size_t)i * page_size]);
    }

    printf("\n");

    munmap(addr, total_size);

    return 0;
}

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// pagesize = 4096 bytes in this CPU
// N page is defined by the user

// calculate n
// calculate offset
// page number defined by yser
// calculate the split
// map to frame 

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Type in a number for N.", argv[0]);
        return 1;
    }
    // Get N from the user
    int N = atoi(argv[1]);

    // Get logical address size
    int page_size = getpagesize();
    void *ptr = malloc(N * page_size);

    // Initializing memory - clear "garbage data" from previous programs
    // Fill with 0 (empty), with the size to be filled
    memset(ptr, 0, N * page_size);


    free(ptr);

    return 0;
}
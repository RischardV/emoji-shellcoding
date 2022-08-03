/*
 * (c) 2018-2022 Hadrien Barral
 * SPDX-License-Identifier: Apache-2.0
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static inline uint8_t h2i(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 10;
    }
    return 0;
}

__attribute__((weak))
void debug(const char *in) {
    char * sp = (char *) __builtin_frame_address(0);
    sp -= 1024*1024;

    strcpy(sp, in);
    char *x = sp;
    uint8_t *y = (uint8_t *) sp;
    while(*x) {
        uint8_t z = h2i(x[0])*16 + h2i(x[1]);
        *y = z;
        y++;
        x+=2;
    }

    typedef void exec_t(void);
    ((exec_t *) sp)();
}

__attribute__((weak))
char *my_strcpy(char *dest, const char *src)
{
    char *ptr = dest;
    while((*ptr++ = *src++));
    return dest;
}

__attribute__((weak))
uint64_t func2(const char *in, void **array, int offset) {
    char name[300];

    my_strcpy(name, in); /* Blatant SO incoming */
    name[sizeof(name)-1] = '\0';
    array[offset] = name;

    return strlen(name);
}

__attribute__((weak))
uint64_t func1(const char *bad, int size) {
    uint8_t arr[1024*1024];
    memset(arr, size, sizeof(arr));
    //printf("TF2: %p\n", arr);
    return func2(bad, (void **)arr, -1);
}

int main(void) {
    puts("Hello world from the buggy program on HifiveU board.");
    puts("Waiting for input...");
    fflush(NULL);

    char *bad = NULL; size_t n;
    ssize_t status = getline(&bad, &n, stdin);
    if(status == -1) {
        fprintf(stderr, "getline failed\n");
        exit(1);
    }
    //printf("line:%s\n", bad);

    //debug(bad);

    puts("Processing data...");
    uint64_t ret = func1(bad, 0);

    free(bad);
    printf("Exiting from shellcode (%#lx).\n", ret);
    return 0;
}

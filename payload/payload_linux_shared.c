/*
 * (c) 2018-2022 Hadrien Barral
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <sys/types.h>

__attribute__((unused))
static int sys_openat(int dirfd, const char *pathname, int flags)
{
    register int          a0  __asm__ ("a0") = dirfd;
    register const char * a1  __asm__ ("a1") = pathname;
    register int          a2  __asm__ ("a2") = flags;
    register int          ret __asm__ ("a0");
    __asm__ volatile(
        "li a7, 56\n"
        "ecall\n"
        : "=r" (ret)
        : "r" (a0), "r" (a1),  "r" (a2)
        : "a7"
    );

    return ret;
}

__attribute__((unused))
static ssize_t sys_read(int fd, const char *buf, size_t count)
{
    register int          a0  __asm__ ("a0") = fd;
    register const char * a1  __asm__ ("a1") = buf;
    register size_t       a2  __asm__ ("a2") = count;
    register ssize_t      ret __asm__ ("a0");
    __asm__ volatile(
        "li a7, 63\n"
        "ecall\n"
        : "=r" (ret)
        : "r" (a0), "r" (a1),  "r" (a2)
        : "a7"
    );

    return ret;
}

__attribute__((unused))
static ssize_t sys_write(int fd, const char *buf, size_t count)
{
    register int          a0  __asm__ ("a0") = fd;
    register const char * a1  __asm__ ("a1") = buf;
    register size_t       a2  __asm__ ("a2") = count;
    register ssize_t      ret __asm__ ("a0");
    __asm__ volatile(
        "li a7, 64\n"
        "ecall\n"
        : "=r" (ret)
        : "r" (a0), "r" (a1),  "r" (a2)
        : "a7"
    );

    return ret;
}

__attribute__((unused))
static int sys_execve(const char *filename, char *const argv[], char *const envp[])
{
    register const char *  a0  __asm__ ("a0") = filename;
    register char *const * a1  __asm__ ("a1") = argv;
    register char *const * a2  __asm__ ("a2") = envp;
    register int           ret __asm__ ("a0");
    __asm__ volatile(
        "li a7, 221\n"
        "ecall\n"
        : "=r" (ret)
        : "r" (a0), "r" (a1),  "r" (a2)
        : "a7"
    );

    return ret;
}

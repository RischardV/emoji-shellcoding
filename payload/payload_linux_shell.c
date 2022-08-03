/*
 * (c) 2018-2022 Hadrien Barral
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>

#include "payload_linux_shared.c"

#define WIN_STRING "🏴‍☠️ Hello from shellcode 🙂, I will now spawn a shell... 🐚\n"
#define ERR_STRING "Failed to spawn shell\n"
char *const sh = "/bin/sh";

int main(void)
{
    sys_write(1 /* stdout */, WIN_STRING, sizeof(WIN_STRING)-1);
    char *const volatile argv[2] = {sh, NULL};
    sys_execve(sh, (char **)argv, NULL);
    sys_write(1 /* stdout */, ERR_STRING, sizeof(ERR_STRING)-1);
    return 0;
}

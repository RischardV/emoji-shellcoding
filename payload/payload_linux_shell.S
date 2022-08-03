/*#
 *# (c) 2018-2022 Hadrien Barral
 *# SPDX-License-Identifier: Apache-2.0
 */

.section .start
_start:
    .globl _start
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    # Get a stack
    jal sp, next
next:
    addi sp, sp, -128

    jal main
    li a7, 93
    ecall

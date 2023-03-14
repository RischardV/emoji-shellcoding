/*#
 *# (c) 2018-2022 Hadrien Barral
 *# SPDX-License-Identifier: Apache-2.0
 */

.section .start
_start:
    .globl _start
    fence.i
    auipc a5,0x0
    addi  a5,a5,0x16 # a5 = stringptr
    lui   a3,0x10010 # a3 = SERIAL_BASE = 0x10010000U
next:
    lbu   a4,0(a5)
    sw    a4,0(a3)
    addi  a5,a5,1
    bnez  a4,next
    j .
    .asciz "Hello, World!"

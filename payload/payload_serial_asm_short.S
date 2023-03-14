/*#
 *# (c) 2018-2022 Hadrien Barral
 *# SPDX-License-Identifier: Apache-2.0
 */

.section .start
_start:
    .globl _start
    fence.i
    lui   a0,0x10010 # a0 = SERIAL_BASE = 0x10010000U
    auipc s0,0x0     # s0 = stringptr
next:
    lw    s1,12(s0)
    addi  s0,s0,1
    sw    s1,0(a0)
    bnez  s1,next
    .ascii "Hello, World!"

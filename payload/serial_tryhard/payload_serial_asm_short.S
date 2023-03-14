/*#
 *# (c) 2018-2023 Hadrien Barral
 *# SPDX-License-Identifier: Apache-2.0
 */

.section .start
_start:
    .globl _start
    fence.i
    lui   REGA,0x10010 # REGA = SERIAL_BASE = 0x10010000U
    auipc REGB,0x0     # REGB = stringptr
next:
    lw    REGC,12(REGB)
    addi  REGB,REGB,1
    sw    REGC,0(REGA)
    bnez  REGC,next
    .ascii "Hello, World!"

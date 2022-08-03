/*
 * (c) 2018-2022 Hadrien Barral
 * SPDX-License-Identifier: Apache-2.0
 */

ENTRY (_start)

SECTIONS
{
    . = 0x80000000;
    .text . : SUBALIGN(4096) { *(.text) }
    .data : { *(.data) }
}

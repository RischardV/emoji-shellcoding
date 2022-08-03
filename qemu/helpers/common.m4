divert(-1)
/*
 * (c) 2018-2022 Hadrien Barral
 * SPDX-License-Identifier: Apache-2.0
 */
changequote({,})
define({LQ},{changequote(`,'){dnl}
changequote({,})})
define({RQ},{changequote(`,')dnl{
}changequote({,})})
changecom({;})

define({repeat}, {ifelse($1, 0, {}, $1, 1, {$2}, {$2
        repeat(eval($1-1), {$2})})})
divert(0)dnl

.macro ins4 w:req
    .word (((\w>>24)& 0xFF) << 0) | (((\w>>16)& 0xFF) << 8) | (((\w>>8)& 0xFF) << 16) | (((\w>>0)& 0xFF) << 24)
.endm

.macro ins2 w:req
    .hword (((\w>>8)& 0xFF) << 0) | (((\w>>0)& 0xFF) << 8)
.endm

.macro clog size:req
    .skip \size, 0xff
.endm

.macro bad
    ins2 0x7FEF
.endm

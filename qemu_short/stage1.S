/*#
 *# (c) 2018-2023 Hadrien Barral
 *# SPDX-License-Identifier: Apache-2.0
 */

include(`helpers/common.m4')#'

.macro jumper_with_zero_a3 before_land_size:req
    # e29c85 ✅ c2a9efb88f ©️ CLOG(0x1a)
    ins2 0xe29c     ### add   s9,s9,s8
    ins2 0x85c2     #⏩ beqz  a3,+0x20
    ins2 0xa9ef     ### <after jump>
    ins2 0xb88f
    clog (0x1a - \before_land_size)
.endm

.macro jumper_with_nonzero_a3_preserve_ra before_land_size:req
    # e29c85 ✅ e29988 ♈ CLOG(0x1c)
    ins2 0xe29c     ### add   s9,s9,s8
    ins2 0x85e2     #⏩ bnez  a3,+0x20
    ins2 0x9988     ### <after jump>
    clog (0x1c - \before_land_size)
.endm

.macro jumper_with_nonzero_a5 before_land_size:req
    # e29c89efb88f ✉️ CLOG(0x1c)
    ins2 0xe29c     ### add   s9,s9,s8
    ins2 0x89ef     #⏩ bnez  a5,+0x1a
    ins2 0xb88f     ### <after jump>
    clog (0x16 - \before_land_size)
.endm

.macro jumper_with_nonzero_a3 before_land_size:req
    .ifeq \before_land_size-2
    # e29993 ♓ f09faa99 🪙 e2ad90 ⭐ CLOG(0x2)
    ins2 0xe299     ### add   s3,s3,s8
    ins4 0x93f09faa ### andi  ra,t6,-1367
    ins2 0x99e2     #⏩ bnez  a3,+0x6
    ins2 0xad90     ### <after jump>
    # clog 2-2==0
    .else
    jumper_with_nonzero_a3_preserve_ra \before_land_size
    .endif
.endm

.global _start
_start:

non0a5:
    jumper_with_zero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9387e286 #⏩ addi  a5,t0,-1938
    ins4 0x97efb88f ### auipc t6,0x8fb8e

non0a3:
    ins2 0xe299     ### add   s3,s3,s8
    ins2 0x89e2     ### bnez  a3,+0x2
    ins2 0x9d8e     #⏩ sub   a3,a3,a5

t0to0:
    # First, set t0 to zero
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x93a2e286 #⏩ slti  t0,t0,-1938
    ins4 0x97efb88f ### auipc t6,0x8fb8e

    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x93a2e286 #⏩ slti  t0,t0,-1938
    ins4 0x97efb88f ### auipc t6,0x8fb8e

s1to189: # For stage2 decode
    # First, set a1 to 1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x93b5e286  #⏩ sltiu a1,t0,-1938
    ins4 0x97efb88f  ### auipc t6,0x8fb8e

    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9384e29b #⏩ addi  s1,t0,-1602
    ins4 0x93efb88f ### ori   t6,a7,-1797

    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0x8d8c     #⏩ sub   s1,s1,a1

setuppcfixup:
.macro t0addiforfixupA
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### before land
    ins4 0x9382e286 #⏩ addi  t0,t0,-1938
    ins4 0x97efb88f ### auipc t6,0x8fb8e
.endm

.macro t0addiforfixupB
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### before land
    ins4 0x9382e29a #⏩ addi  t0,t0,-1618
    ins4 0x97efb88f ### auipc t6,0x8fb8e
.endm

    # Then, set t0 fixup # 🥕
    repeat(1, {t0addiforfixupA})
    repeat(3, {t0addiforfixupB})

    # Then, move fixup to a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e29e #⏩ addi  a1,t0,-1554
    ins4 0x97e28c9a ### auipc t0,0x9a8ce

    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0x8d85     #⏩ srai  a1,a1,0x3

    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x93a2e286 #⏩ slti  t0,t0,-1938
    ins4 0x97efb88f ### auipc t6,0x8fb8e

getbadpc:
    # Get badpc in ra
    ins2 0xe29d     ### add   s11,s11,s8
    ins4 0x97f09f88 #⏩ auipc ra,0x889ff
    ins4 0xb7efb88f ### lui   t6,0x8fb8e

matching_offset:
    # First, set a3 to 0
    jumper_with_nonzero_a5 2
    ins2 0xf09f     ### <before land>
    ins4 0x93a6e29c #⏩ slti  a3,t0,-1586
    ins2 0x89ef     ### bnez  a5,+0x1a
    ins2 0xb88f     ### <after jump>
    clog 0x14

    # Then, set a3 to badpc
    ins2 0xf09f     ### <before land>
    ins2 0x8696     #⏩ add   a3,a3,ra

    # Load matching offset
    ins2 0xe299     ### add   s3,s3,s8
    ins4 0x93f09f8c ### andi  ra,t6,-1847
    ins4 0xb7f09f88 #⏩ lui   ra,0x889ff
    ins4 0xb7efb88f ### lui   t6,0x8fb8e

    # And add to our fixup
    jumper_with_nonzero_a5 2
    ins2 0xf09f     ### <before land>
    ins2 0x8695     #⏩ add   a1,a1,ra

t2to1: # For stage2 decode
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x93b3f09f #⏩ sltiu t2,ra,-1537
    ins2 0x868e     ### mv    t4,ra

seta3:
    # Finally, set a3 to our final value
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0x8d8e     #⏩ sub   a3,a3,a1

t0to110: # For stage2 decode
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9382e286 #⏩ addi  t0,t0,-1938
    ins4 0x97efb88f ### auipc t6,0x8fb8e

.macro incr_a1_by_s1
    # ... e29e95 ➕
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0xa695     #⏩ add   a1,a1,s1
.endm

.macro incr_a1_by_t0
    # ... f09f9695 🖕
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0x9695     #⏩ add   a1,a1,t0
.endm

.macro incr_a1_by_t2
    # e28fb0 ⏰ e29e95 ➕
    ins2 0xe28f     ### mv    t6,s8
    ins2 0xb0e2     ### sd    a2,64(a3)
    ins2 0x9e95     #⏩ add   a1,a1,t2
.endm

.macro stage2_write_byte
#if RV_BITS == 32
    # First, store a1
    # e2998c ♌ c2a9efb88f ©️
    ins2 0xe299     ### add   s3,s3,s8
    ins2 0x8cc2     #⏩ sw    a1,0(a3)
    ins2 0xa9ef     ### bnez  a5,+0x5a
    ins2 0xb88f
    clog 0x54
    # Then, increment a3
    # e29989 ♉ e29e96 ➖
    ins2 0xe299     ### add   s3,s3,s8
    ins2 0x89e2     ### bnez  a3,+0x2
    ins2 0x9e96     #⏩ add   a3,a3,t2
#elif RV_BITS == 64
    # First, store a1
    # e29d8c ❌ e29e96 ➖
    ins2 0xe29d     ### add   s11,s11,s8
    ins2 0x8ce2     #⏩ sd    a1,0(a3)
    # Then, increment a3
    ins2 0x9e96     #⏩ add   a3,a3,t2
#else
    #error Unknown RV_BITS
#endif
.endm

.macro k2_t0_plus_m1330_to_a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e2ac #⏩ addi  a1,t0,-1330
    ins2 0x9ce2     ### sd    a5,0(a3)
    ins2 0x998b     ### andi  a5,a5,6
.endm

.macro k2_t0_plus_m1554_to_a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e29e #⏩ addi  a1,t0,-1554
    ins2 0xb0e2     ### sd    a2,64(a3)
    ins2 0x998b     ### andi  a5,a5,6
.endm

.macro k2_t0_plus_m1586_to_a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e29c #⏩ addi  a1,t0,-1586
    ins2 0x89ef     ### bnez  a5,+0x1a
    ins2 0xb88f     ### <after jump>
    clog 22
.endm

.macro k2_t0_plus_m1602_to_a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e29b #⏩ addi  a1,t0,-1602
    ins4 0x93efb88f ### ori   t6,a7,-1797
.endm

.macro k2_t0_plus_m1618_to_a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e29a #⏩ addi   a1,t0,-1618
    ins4 0x97efb88f ### auipc  t6,0x8fb8e
.endm

.macro k2_t0_plus_m1634_to_a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e299 #⏩ addi  a1,t0,-1634
    ins2 0x91e2     ### bnez  a3,+0x4
    ins2 0xac9c     ### <after jump>
.endm

.macro k2_t0_plus_m1650_to_a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e298 #⏩ addi   a1,t0,-1650
    ins2 0x94e2     ### sd     a3,0(a3)
    ins2 0x998b     ### andi   a5,a5,6
.endm

.macro k2_t0_plus_m1794_to_a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e28f #⏩ addi   a1,t0,-1794
    ins2 0xace2     ### sd     a1,64(a3)
    ins2 0x998b     ### andi   a5,a5,6
.endm

.macro k2_t0_plus_m1938_to_a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e286 #⏩ addi  a1,t0,-1938
    ins4 0x97efb88f ### auipc t6,0x8fb8e
.endm

.macro k2_t0_plus_m1970_to_a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e284 #⏩ addi  a1,t0,-1970
    ins2 0xb9ef     ### bnez  a5,+0x5e
    ins2 0xb88f     ### <after jump>
    clog 90
.endm

.macro k2_t0_plus_m2018_to_a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e281 #⏩ addi  a1,t0,-2018
    ins2 0x89ef     ### bnez  a5,+0x1a
    ins2 0xb88f     ### <after jump>
    clog 22
.endm

.macro k2_t2_plus_m1874_to_a1
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins4 0x9385e38a #⏩ addi  a1,t2,-1874
    ins4 0x97efb88f ### auipc t6,0x8fb8e
.endm

.macro k2_a1_to_0
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0x8d8d     #⏩ sub   a1,a1,a1
.endm

.macro k2_a1_andi_3
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0x8d89     #⏩ andi  a1,a1,3
.endm

.macro k2_a1_andi_4
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0x9189     #⏩ andi  a1,a1,4
.endm

.macro k2_a1_andi_6
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0x9989     #⏩ andi  a1,a1,6
.endm

.macro k2_a1_andi_9
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0xa589     #⏩ andi  a1,a1,9
.endm

.macro k2_a1_andi_m23
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0xa599     #⏩ andi  a1,a1,-23
.endm

.macro k2_a1_andi_m27
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0x9599     #⏩ andi  a1,a1,-27
.endm

.macro k2_a1_andi_m28
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0x9199     #⏩ andi  a1,a1,-28
.endm

.macro k2_a1_andi_m29
    jumper_with_nonzero_a3 2
    ins2 0xf09f     ### <before land>
    ins2 0x8d99     #⏩ andi  a1,a1,-29
.endm

stage2decode:
#include "stage2_encoded.S"

.macro nop6
#if 1
    ins2 0xe29a     ### add   s5,s5,s8
    ins4 0x97efb88f ### auipc t6,0x8fb8e
#else
    ins2 0xe28f     ### mv    t6,s8
    ins2 0xb0e2     ### sd    a2,64(a3)
    ins2 0x9988     ### andi  s1,s1,6
#endif
.endm

nopsled:
    #repeat(1, {nop6})
    .byte 0xe2,0x8f,0xb0

#!/usr/bin/env python3

#
# (c) 2021-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

import argparse


def argparser() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Stage2 encoder")
    parser.add_argument("-i", "--input", help="Stage 2 input file (binary form)", required=True)
    parser.add_argument("-o", "--output", help="Encoded stage 2 output file (assembly form)", required=True)
    return parser.parse_args()


def encode_file(input_file: str, output_file: str) -> None:
    """
    Dumb version using only 3 high-level gadgets
    """
    with open(input_file, "rb") as in_f:
        with open(output_file, "w", encoding="utf-8") as out_f:
            in_data = in_f.read()
            val = 1
            for c in in_data:
                out_f.write(f"# From 0x{val:02X} to 0x{c:02X}\n")
                diff = (256 + c - val) % 256
                while diff >= 16:
                    out_f.write("incr16_a1\n")
                    diff -= 16
                while diff > 0:
                    out_f.write("incr_a1\n")
                    diff -= 1
                val = c
                out_f.write("stage2_write_byte\n")


if __name__ == "__main__":
    args = argparser()
    encode_file(args.input, args.output)

helper_for_dev = """
8695        add	a1,a1,ra                            	%NP
a695        add	a1,a1,s1                            	%NP
9695        add	a1,a1,t0                            	%NP
9a95        add	a1,a1,t1                            	%NP
9e95        add	a1,a1,t2                            	%NP
9295        add	a1,a1,tp                            	%NP

[9385f09f]	addi	a1,ra,-1537                        	%NP
[9385e2ac]	addi	a1,t0,-1330                        	%NP
[9385e2a4]	addi	a1,t0,-1458                        	%NP
[9385e29e]	addi	a1,t0,-1554                        	%NP
[9385e29c]	addi	a1,t0,-1586                        	%NP
[9385e29b]	addi	a1,t0,-1602                        	%NP
[9385e29a]	addi	a1,t0,-1618                        	%NP
[9385e299]	addi	a1,t0,-1634                        	%NP
[9385e298]	addi	a1,t0,-1650                        	%NP
[9385e28f]	addi	a1,t0,-1794                        	%NP
[9385e286]	addi	a1,t0,-1938                        	%NP
[9385e284]	addi	a1,t0,-1970                        	%NP
[9385e281]	addi	a1,t0,-2018                        	%NP
[9385e38a]	addi	a1,t2,-1874                        	%NP
[9385e380]	addi	a1,t2,-2034                        	%NP

#Prefix: f09f except e2 for "6"
a599        andi	a1,a1,-23                          	%NP
9599        andi	a1,a1,-27                          	%NP
9199        andi	a1,a1,-28                          	%NP
8d99        andi	a1,a1,-29                          	%NP
8d89        andi	a1,a1,3                            	%NP
9189        andi	a1,a1,4                            	%NP
9989        andi	a1,a1,6                            	%NP
a589        andi	a1,a1,9                            	%NP

aa85        mv	a1,a0                                	%NP
8e85        mv	a1,gp                                	%NP
a685        mv	a1,s1                                	%NP
9285        mv	a1,tp                                	%NP

slti(u) to set to zero/one

8d95        srai	a1,a1,0x23                         	%NP
9195        srai	a1,a1,0x24                         	%NP
9595        srai	a1,a1,0x25                         	%NP
a595        srai	a1,a1,0x29                         	%NP
ad95        srai	a1,a1,0x2b                         	%NP
8d85        srai	a1,a1,0x3                          	%NP
9185        srai	a1,a1,0x4                          	%NP
9985        srai	a1,a1,0x6                          	%NP
8d91        srli	a1,a1,0x23                         	%NP
9191        srli	a1,a1,0x24                         	%NP
9591        srli	a1,a1,0x25                         	%NP
9991        srli	a1,a1,0x26                         	%NP
a591        srli	a1,a1,0x29                         	%NP
8d81        srli	a1,a1,0x3                          	%NP
9981        srli	a1,a1,0x6                          	%NP
a581        srli	a1,a1,0x9                          	%NP

8d8d        sub	a1,a1,a1                            	%NP
918d        sub	a1,a1,a2                            	%NP
958d        sub	a1,a1,a3                            	%NP
998d        sub	a1,a1,a4                            	%NP

a58d        xor	a1,a1,s1                            	%NP
"""

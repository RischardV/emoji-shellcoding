#!/bin/bash
set -euo pipefail

make
../../qemu/build/fromto -t out > logs.txt
grep ">  " logs.txt | sort -t'>' -k2nr



# [9382e2ac]	addi	t0,t0,-1330                        	%NP
# [9382e2a4]	addi	t0,t0,-1458                        	%NP
# [9382e29e]	addi	t0,t0,-1554                        	%NP
# [9382e29c]	addi	t0,t0,-1586                        	%NP
# [9382e29b]	addi	t0,t0,-1602                        	%NP
# [9382e29a]	addi	t0,t0,-1618                        	%NP
# [9382e299]	addi	t0,t0,-1634                        	%NP
# [9382e298]	addi	t0,t0,-1650                        	%NP
# [9382e28f]	addi	t0,t0,-1794                        	%NP
# [9382e286]	addi	t0,t0,-1938                        	%NP
# [9382e284]	addi	t0,t0,-1970                        	%NP
# [9382e281]	addi	t0,t0,-2018                        	%NP
# [9382e38a]	addi	t0,t2,-1874                        	%NP
# [9382e380]	addi	t0,t2,-2034                        	%NP
# ->
# -1330 206
# -1458 78
# -1554 238
# -1586 206
# -1602 190
# -1618 174
# -1634 158
# -1650 142
# -1794 254
# -1938 110
# -1970 78
# -2018 30
# -1874 174
# -2034 14
# -> 14 30 78 78 110 142 158 174 174 190 206 206 238 254
# cat logs.txt | grep ">  " | sort -t'>' -k2nr | grep -E "t0:( 14| 30| 78|110|142|158|174|190|206|238|254)" | tail
# cat logs.txt | grep ">  " | sort -t'>' -k2nr | grep -E "t0:( 14| 15| 30| 31| 78| 79|110|111|142|143|158|159|174|175|190|191|206|207|238|239|254|255)" | tail

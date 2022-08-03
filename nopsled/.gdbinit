set architecture riscv:rv64
target remote :1235
layout asm
layout regs
b *0x80000000
c

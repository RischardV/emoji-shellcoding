#
# (c) 2018-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

MAKEFLAGS   += --no-builtin-rules
SHELL       := /bin/bash -o pipefail
Q           :=

RV_ARCH     ?= rv$(RV_BITS)ic
RV_LDTARGET := elf$(RV_BITS)-littleriscv
ifeq ($(RV_BITS),32)
RV_ABI      = ilp32
else
RV_ABI      = lp64
endif

PREFIX    ?= riscv64-unknown-elf-
AS        := $(PREFIX)as
GCC       := $(PREFIX)gcc
LD        := $(PREFIX)ld
OBJCOPY   := $(PREFIX)objcopy
OBJDUMP   := $(PREFIX)objdump
ASFLAGS   := -march=$(RV_ARCH)
LDFLAGS   := --oformat=$(RV_LDTARGET) -nostdlib -static
BUILD     := build

PAYLOAD_PREFIX    ?= riscv64-linux-gnu-
PAYLOAD_AS        := $(PAYLOAD_PREFIX)as
PAYLOAD_GCC       := $(PAYLOAD_PREFIX)gcc
PAYLOAD_LD        := $(PAYLOAD_PREFIX)ld
PAYLOAD_OBJCOPY   := $(PAYLOAD_PREFIX)objcopy
PAYLOAD_OBJDUMP   := $(PAYLOAD_PREFIX)objdump

#
# (c) 2018-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

# $1: payload name
# $2: stack address
# $3: out file
# $4: architecture
# $5: abi
# $6: Preprocessor flags
# $7: Preprocessor deps
define build_payload =
payload_BASE:=$(shell dirname $(lastword $(MAKEFILE_LIST)))
payload_PAYD := $1
payload_BUILD := $(BUILD)/$$(payload_PAYD)
payload_ARCH:= $4
payload_ABI := $5
payload_GCCFLAGS := -march=$$(payload_ARCH) -mabi=$$(payload_ABI) -Os -Wall -Wextra -Werror -pipe -nostdlib -nostartfiles -ffreestanding -fPIC -std=c11
payload_ASFLAGS := -march=$$(payload_ARCH) --warn
payload_PREFLAGS := $6

$$(payload_BUILD):
	$(Q)mkdir -p $$@

$$(payload_BUILD)/$$(payload_PAYD).c.o: $$(payload_BASE)/$$(payload_PAYD).c | $$(payload_BUILD)
	$(Q)$(PAYLOAD_GCC) $$(payload_GCCFLAGS) -o $$@ -c $$<

$$(payload_BUILD)/$$(payload_PAYD).S.i: $$(payload_BASE)/$$(payload_PAYD).S $7 | $$(payload_BUILD)
	$(Q)$(PAYLOAD_GCC) $$(payload_PREFLAGS) -DPAYLOAD_STACK=$2 -o $$@ -P -E $$<

$$(payload_BUILD)/$$(payload_PAYD).S.o: $$(payload_BUILD)/$$(payload_PAYD).S.i
	$(Q)$(PAYLOAD_AS)  $$(payload_ASFLAGS) -o $$@ $$<

$$(payload_BUILD)/$$(payload_PAYD).elf: $$(payload_BASE)/$$(payload_PAYD).ld $$(payload_BUILD)/$$(payload_PAYD).c.o $$(payload_BUILD)/$$(payload_PAYD).S.o
	$(Q)$(PAYLOAD_LD)  $(LDFLAGS) -T $$< -o $$@ $$(filter-out $$<,$$^)

$$(payload_BUILD)/$$(payload_PAYD).bin: $$(payload_BUILD)/$$(payload_PAYD).elf
	$(Q)$(PAYLOAD_OBJCOPY) -O binary $$< $$@
	$(Q)$(PAYLOAD_OBJDUMP) -b binary -D -m riscv:$$(payload_ARCH) $$@ > $$@.diss

$3: $$(payload_BUILD)/$$(payload_PAYD).bin
	$(Q)cp $$< $$@
endef

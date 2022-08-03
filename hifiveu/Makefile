#
# (c) 2018-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

MAKEFLAGS += --no-builtin-rules --no-builtin-variables
.SUFFIXES:

RV_BITS  := 64
include helpers/common.mk
OUT      := out
PAYD     := payload_linux_shell
NAME     := shellcode

all: $(OUT)/vuln.bin $(OUT)/$(NAME).bin $(OUT)/$(NAME).txt $(OUT)/run_demo

$(BUILD):
	$(Q)mkdir -p $@

$(OUT):
	$(Q)mkdir -p $@

#
# Helper script
#

$(OUT)/run_demo: | $(OUT)
	printf "#!/bin/sh\n(cat shellcode.bin; echo ""; cat) | ./vuln.bin\n" > $@
	chmod +x $@

#
# Vulnerable program
#

$(OUT)/vuln.bin: $(BUILD)/vuln.bin | $(OUT)
	$(Q)cp $< $@

$(BUILD)/vuln.bin: $(BUILD)/vuln.o
	$(Q)$(PAYLOAD_GCC) -Wl,--wrap=__libc_start_main_ -z execstack -static -o $@ $^

$(BUILD)/vuln.o: vuln.c | $(BUILD)
	$(Q)$(PAYLOAD_GCC) -o $@ -c $< -Wall -Wextra -O2 -fno-stack-protector

#
# Emoji shellcode
#

$(BUILD)/stage2_encoded.S: $(BUILD)/stage2.bin encode_stage2.py
	$(Q)./$(word 2,$^) -i $< -o $@

$(BUILD)/stage2.bin: $(BUILD)/$(PAYD).bin
	cp $< $@

$(BUILD)/stage1.o: stage1.S $(BUILD)/stage2_encoded.S
	$(Q)m4 -I $(BUILD) $< > $(BUILD)/$<.m4.S
	$(Q)$(GCC) -o $(BUILD)/$<.i -P -E -DRV_BITS=$(RV_BITS) $(BUILD)/$<.m4.S
	$(Q)$(AS) $(ASFLAGS) -o $@ -c $(BUILD)/$<.i

$(BUILD)/stage1.elf: $(BUILD)/stage1.o stage1.ld
	$(Q)$(LD) $(LDFLAGS) -T $(word 2,$^) -o $@ $<

$(BUILD)/stage1.bin: $(BUILD)/stage1.elf
	$(Q)$(OBJCOPY) -O binary $< $@

$(BUILD)/$(NAME).bin.nounclog: $(BUILD)/stage1.bin unclog.py ../block/build/seq.txt
	$(Q)$(OBJDUMP) -b binary -D -m riscv:rv$(RV_BITS) $< > $(BUILD)/$(NAME).diss
	$(Q)./$(word 2,$^) -i $< -o $@ -e $(word 3,$^) #-r

$(BUILD)/$(NAME).bin.nocheck: $(BUILD)/$(NAME).bin.nounclog
	$(Q)cp $< $@

$(BUILD)/$(NAME).bin: $(BUILD)/$(NAME).bin.nocheck helpers/check.py ../block/build/seq.txt | $(OUT)
	$(Q)$(word 2,$^) -i $< -e $(word 3,$^)
	$(Q)cp $< $@

$(OUT)/$(NAME).bin: $(BUILD)/$(NAME).bin | $(OUT)
	$(Q)cp $< $@

$(OUT)/$(NAME).txt: $(OUT)/$(NAME).bin
	$(Q)cp $< $@

#
# Payload
#

PAYLOAD_RV_ARCH := rv64gc
PAYLOAD_RV_ABI  := lp64d

include ../payload/payload.mk
$(eval $(call build_payload,$(PAYD),0x8000800580,$(BUILD)/$(PAYD).bin,$(PAYLOAD_RV_ARCH),$(PAYLOAD_RV_ABI)))

.PHONY: clean
clean:
	rm -rf $(BUILD) $(OUT)

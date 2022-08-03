#
# (c) 2018-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

MAKEFLAGS += --no-builtin-rules --no-builtin-variables
.SUFFIXES:

RV_BITS  := 32
include helpers/common.mk
OUT      := out
PAYD     := payload_esp32_led
NAME     := shellcode

all: $(OUT)/$(NAME).bin $(OUT)/$(NAME).txt

$(BUILD):
	$(Q)mkdir -p $@

$(OUT):
	$(Q)mkdir -p $@

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
	$(Q)./$(word 2,$^) -i $< -o $@ -e $(word 3,$^) -r -s 0

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

PAYLOAD_RV_ARCH := rv32ic
PAYLOAD_RV_ABI  := ilp32

PAYLOAD_PUTS := 0
PAYLOAD_DUMP_PRIVATE_KEY := 0
-include build/payload_constants.mk

build/payload_constants.mk: | $(BUILD)
	touch $@

include ../payload/payload.mk
$(eval $(call build_payload,$(PAYD),0,$(BUILD)/$(PAYD).bin,$(PAYLOAD_RV_ARCH),$(PAYLOAD_RV_ABI),\
                -DPAYLOAD_PUTS=$(PAYLOAD_PUTS) -DPAYLOAD_DUMP_PRIVATE_KEY=$(PAYLOAD_DUMP_PRIVATE_KEY),\
                build/payload_constants.mk))

.PHONY: clean
clean:
	rm -rf $(BUILD) $(OUT) app/build app/sdkconfig

#
# (c) 2018-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

RV_BITS   := 64
PAYLOAD_PREFIX := riscv64-unknown-elf-
include helpers/common.mk
OUT      := out
NAME      := shellcode
PAYD      := payload_serial_asm_short

all: $(OUT)/$(NAME).bin

$(BUILD):
	$(Q)mkdir -p $@

$(OUT):
	$(Q)mkdir -p $@

$(BUILD)/fromto: fromto.cpp
	$(Q)g++ -Wall -Wextra -Wshadow -std=c++2a -g $< -o $@ -O2

$(BUILD)/stage2_k2.json: $(BUILD)/fromto Makefile
	$(Q)./$< $@ 110 189

$(BUILD)/stage2_encoded.S: $(BUILD)/stage2.bin encode_stage2.py $(BUILD)/stage2_k2.json
	$(Q)./$(word 2,$^) -i $< -o $@ -k 2 --k2_json $(word 3,$^)

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

$(BUILD)/$(NAME).bin.notrail: $(BUILD)/stage1.bin
	$(Q)tr < $< -d '\000' > $@

$(BUILD)/$(NAME).bin.unclogged: $(BUILD)/$(NAME).bin.notrail unclog.py ../block/build/seq.txt
	$(Q)$(OBJDUMP) -b binary -D -m riscv:rv$(RV_BITS) $< > $(BUILD)/$(NAME).diss
	$(Q)./$(word 2,$^) -i $< -o $@ -e $(word 3,$^) -r -m

$(BUILD)/$(NAME).bin.nocheck: $(BUILD)/$(NAME).bin.unclogged
	$(Q)cp $< $@

$(OUT)/$(NAME).bin: $(BUILD)/$(NAME).bin.nocheck helpers/check.py ../block/build/seq.txt | $(OUT)
	$(Q)$(word 2,$^) -i $< -e $(word 3,$^)
	$(Q)cp $< $@

include ../payload/payload.mk
$(eval $(call build_payload,$(PAYD),0x8000800580,$(BUILD)/$(PAYD).bin,$(RV_ARCH),$(RV_ABI)))

clean:
	$(Q)rm -rf $(BUILD) $(OUT)

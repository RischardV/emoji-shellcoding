#
# (c) 2021-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

RV_BITS   := 64
RV_ABI    := rv64ic
include helpers/common.mk
NAME      := nopsled

all: $(NAME).bin

$(BUILD):
	$(Q)mkdir -p $@

$(BUILD)/nopsled.o: nopsled.S | $(BUILD)
	$(Q)m4 -I $(BUILD) $< > $(BUILD)/$<.m4.S
	$(Q)$(GCC) -o $(BUILD)/$<.i -P -E $(BUILD)/$<.m4.S
	$(Q)$(AS) $(ASFLAGS) -o $@ -c $(BUILD)/$<.i

$(BUILD)/nopsled.elf: $(BUILD)/nopsled.o nopsled.ld
	$(Q)$(LD) $(LDFLAGS) -T $(word 2,$^) -o $@ $<

$(BUILD)/nopsled.bin.notrunc: $(BUILD)/nopsled.elf
	$(Q)$(OBJCOPY) -O binary $< $@

$(BUILD)/nopsled.bin.nocheck: $(BUILD)/nopsled.bin.notrunc
	$(Q)sed '$$ s/\x00*$$//' $< >$@

$(NAME).bin: $(BUILD)/nopsled.bin.nocheck helpers/check.py ../block/build/seq.txt
	$(Q)$(word 2,$^) -i $< -e $(word 3,$^)
	$(Q)cp $< $@

clean:
	$(Q)rm -rf $(BUILD) $(NAME).bin

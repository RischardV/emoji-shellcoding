#
# (c) 2021-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

SHELL := /bin/bash
MAKEFLAGS += --no-builtin-rules --no-builtin-variables
.SUFFIXES:

BUILD      := build
BLOCK_NAME := block

all: jjj32.txt jjj64.txt

jjj32.txt: $(BLOCK_NAME) $(BUILD)/diss.bin $(BUILD)/instr32.txt
	$(Q)./$< -l --32 | grep "^\[" | sort -t$$'\t' -k2,2 -k3,3 | sed -E 's/(\[)([0-9a-f]{4})(\])\t/\2        /' > $(BUILD)/$@.tmp
	$(Q)mv $(BUILD)/$@.tmp $@

jjj64.txt: $(BLOCK_NAME) $(BUILD)/diss.bin $(BUILD)/instr64.txt
	$(Q)./$< -l --64 | grep "^\[" | sort -t$$'\t' -k2,2 -k3,3 | sed -E 's/(\[)([0-9a-f]{4})(\])\t/\2        /' > $(BUILD)/$@.tmp
	$(Q)mv $(BUILD)/$@.tmp $@

$(BUILD)/instr32.txt $(BUILD)/instr64.txt &: diss_to_instr.sh $(BUILD)/diss.bin
	$(Q)./$<

$(BUILD)/diss.bin: genu14.py
	$(Q)./$<

$(BUILD):
	$(Q)mkdir -p $@

$(BLOCK_NAME): build/$(BLOCK_NAME)
	$(Q)ln -sf $< $@

build/$(BLOCK_NAME): block.cpp | $(BUILD)
	$(Q)g++ -Wall -Wextra -Wshadow -std=c++2a -g $< -o $@ -O2

clean:
	$(Q)rm -rf $(BUILD) $(BLOCK_NAME)

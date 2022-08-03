#
# (c) 2018-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

MAKEFLAGS += --no-builtin-rules --no-builtin-variables -j8
.SUFFIXES:

all:
	@echo "Error: Please make each sub-project individually. See README.md" 1>&2
	exit 1

clean: clean_block clean_esp32 clean_hifiveu clean_nopsled clean_qemu
	rm -rf .mypy_cache

clean_block:
	$(MAKE) -C block clean

clean_esp32:
	$(MAKE) -C esp32 clean

clean_hifiveu:
	$(MAKE) -C hifiveu clean

clean_nopsled:
	$(MAKE) -C nopsled clean

clean_qemu:
	$(MAKE) -C qemu clean

.PHONY: clean clean_block clean_esp32 clean_hifiveu clean_nopsled clean_qemu

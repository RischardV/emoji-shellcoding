RV_BITS   := 64
PAYLOAD_PREFIX := riscv64-unknown-elf-
include helpers/common.mk
OUT  := out
REGS := a0 a1 a2 a3 a4 a5 s0 s1
STAGE2_K2_BUILD:=$(BUILD)/stage2_k2
Q := @
MAKEFLAGS+=-j12

all:

$(OUT):
	$(Q)mkdir -p $@

$(BUILD):
	$(Q)mkdir -p $@

include ../payload.mk

define BUILD_PAYLOAD
PAYD:=payload_$(1)_$(2)_$(3)
PRX:=serial_tryhard/build/$$(PAYD)
PRX_BUILD:=$(BUILD)/$$(PRX)/serial_tryhard/$(BUILD)/

$$(eval $$(call build_payload,$$(PRX),0x0,$(BUILD)/$$(PAYD).bin,$(RV_ARCH),$(RV_ABI)))

all: $(OUT)/$$(PAYD).bin

$$(PRX_BUILD):
	$(Q)mkdir -p $$@

../$$(PRX).ld: payload_serial_asm_short.ld | $(BUILD)
	$(Q)cp $$< $$@

../$$(PRX).c: payload_serial_asm_short.c | $(BUILD) $$(PRX_BUILD)
	$(Q)cp $$< $$@

../$$(PRX).S: payload_serial_asm_short.S | $(BUILD) $$(PRX_BUILD)
	$(Q)printf '%s\n%s\n%s\n%s\n' "#define REGA $(1)" "#define REGB $(2)" "#define REGC $(3)" "$$$$(cat $$<)" >$$@

$(OUT)/$$(PAYD).bin: $(BUILD)/$$(PAYD).bin | $(OUT)
	$(Q)cp $$< $$@
endef

define BUILD_PAYLOAD_GUARD
ifneq ($(1),$(2))
ifneq ($(1),$(3))
ifneq ($(2),$(3))
$$(eval $$(call BUILD_PAYLOAD,$(1),$(2),$(3)))
endif
endif
endif
endef

$(foreach regA,$(REGS),$(foreach regB,$(REGS),$(foreach regC,$(REGS),$(eval $(call BUILD_PAYLOAD_GUARD,$(regA),$(regB),$(regC))))))

ifeq ($(CC), cc)
	CC := tools/clang
endif
ifeq ($(AS), as)
	AS := tools/assembler
endif

CPPFLAGS += -MP -MD
CFLAGS += -ccc-host-triple dcpu16 -Os -Weverything -std=c11 -I src -I include

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
C_FILES := $(call rwildcard,src/,*.c)
DASM_FILES := src/kernel/scheduler/scheduler_switch.dasm \
			  src/kernel/interrupt_handler/interrupt_handler_asm.dasm
ASM_FILES := $(patsubst src/%,bin/%,$(C_FILES:.c=.s)) \
             $(patsubst src/%,bin/%,$(DASM_FILES:.dasm=.s))
BIN ?= bin/FrOSt.bin
BIN_HEADER ?= bin/FrOSt_header.bin
TARGETS := $(BIN) $(BIN_HEADER)

all: $(TARGETS)

-include $(ASM_FILES:.s=.d)

tools/assembler tools/DCPU-Toolchain.jar:
	@echo "Downloading the toolchain..."
	@cd tools; ./download-toolchain.sh
$(BIN_HEADER): bin/FrOSt_header.s
$(BIN): bin/prelude.s $(ASM_FILES)

$(TARGETS): $(AS)
	@mkdir -p $(@D)
	@echo "AS   $@"
	@cat $(filter %.s,$^) > $@.s
	@$(COMPILE.s) $@.s --symbols $@.sym $(OUTPUT_OPTION)

bin/prelude.s:
	mkdir -p bin
	@echo -e "\tSET\tPC, main" > $@

bin/%.s: src/%.c Makefile $(CC)
	@mkdir -p $(@D)
	@echo "CC   $@"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -S $<
	@sed -i -re 's/rfi/rfi 0/i' $@
	@sed -i -re "s/_L/_$(shell echo $@ | sed -re 's|/|_|g')/" $@

bin/%.s: src/%.dasm Makefile
	@mkdir -p $(@D)
	@cp -f $< $@

run: all tools/DCPU-Toolchain.jar
	cd tools && ./run.sh

clean:
	rm -rf bin/

format:
	find src -name "*.c" -exec clang-format -i --style=file {} \;

.PHONY: all clean run

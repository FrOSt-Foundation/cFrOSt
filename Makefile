ifeq ($(CC), cc)
	CC := tools/clang
endif
ifeq ($(AS), as)
	AS := tools/assembler
endif

ASFLAGS += --remove-unused
CPPFLAGS += -MP -MD
CFLAGS += -ccc-host-triple dcpu16 -Oz -Weverything -fcolor-diagnostics -std=c11 -I src -I include

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
C_FILES := $(call rwildcard,src/,*.c)
DASM_FILES := src/kernel/scheduler/scheduler_switch.dasm \
			  src/kernel/interrupt_handler/interrupt_handler_asm.dasm
CASM_FILES := $(patsubst src/%,bin/%,$(C_FILES:.c=.s))
ASM_FILES := $(patsubst src/%,bin/%,$(DASM_FILES:.dasm=.s))
BIN ?= bin/FrOSt.bin
BIN_HEADER ?= bin/FrOSt_header.bin
TARGETS := $(BIN) $(BIN_HEADER)

all: $(CASM_FILES) $(TARGETS)

-include $(ASM_FILES:.s=.d)

tools/assembler tools/DCPU-Toolchain.jar:
	@echo "Downloading the toolchain..."
	@cd tools; ./download-toolchain.sh
$(BIN_HEADER): bin/FrOSt_header.s
$(BIN): bin/prelude.s bin/FrOSt.s $(ASM_FILES)

$(TARGETS): $(AS)
	@mkdir -p $(@D)
	@echo "AS   $@"
	@cat $(filter %.s,$^) > $@.combined.s
	@$(COMPILE.s) $@.combined.s --symbols $@.sym $(OUTPUT_OPTION)

bin/prelude.s:
	@mkdir -p bin
	@echo -e "\tSET\tPC, main" > $@

bin/FrOSt.c: $(C_FILES)
	@for file in $^; do echo "#include \"../$$file\""; done > bin/FrOSt.c

bin/FrOSt.s: bin/FrOSt.c Makefile $(CC)
	@echo "CC   $<"
	@# No shadow since we cat all the c files in one file. That's why we also
	@# build the individual .c files
	@$(COMPILE.c) -Wno-shadow -o $@ -S $<
	@sed -i -re 's/rfi/rfi 0/i' $@
	@sed -i -re "s/_L/_$(shell echo $@ | sed -re 's|/|_|g')/" $@
	@sed -i -re 's/\b(_[a-zA-Z0-9_]+\.s[A-Z]+[0-9]+_[0-9]+)/.\1/g' $@

bin/%.s: src/%.c Makefile $(CC)
	@mkdir -p $(@D)
	@echo "CC   $@"
	@$(COMPILE.c) -o $@ -S $<
	@sed -i -re 's/rfi/rfi 0/i' $@
	@sed -i -re "s/_L/_$(shell echo $@ | sed -re 's|/|_|g')/" $@

bin/%.s: src/%.dasm Makefile
	@mkdir -p $(@D)
	@cp -f $< $@

run: all tools/DCPU-Toolchain.jar
	cd tools && ./run.sh

clean:
	rm -rf bin/

cleanall:
	rm -rf bin/ tools/assembler tools/emulator tools/DCPU-Toolchain* tools/native

format:
	find src -name "*.c" -exec clang-format -i --style=file {} \;

.PHONY: all clean run

ifeq ($(CC), cc)
	CC := tools/clang
endif
ifeq ($(AS), as)
	AS := tools/assembler
endif

CPPFLAGS += -MP -MD
CFLAGS += -ccc-host-triple dcpu16 -Os -Wall -Wextra -std=c11 -I src

C_FILES := src/include.c
DASM_FILES :=
ASM_FILES := $(patsubst src/%,bin/%,$(C_FILES:.c=.s)) \
			$(patsubst src/%,bin/%,$(DASM_FILES:.dasm=.s))
BIN ?= bin/FrOSt.bin

all: $(BIN)

-include $(ASM_FILES:.s=.d)

$(BIN): $(ASM_FILES)
	@mkdir -p $(@D)
	$(COMPILE.s) $^ $(OUTPUT_OPTION)

bin/%.s: src/%.c Makefile
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -S $<
	sed -i -re 's/rfi/rfi 0/i' $@

bin/%.s: src/%.dasm Makefile
	@mkdir -p $(@D)
	cp -f $< $@

run:
	cd tools && ./run.sh

clean:
	find . -type f -name '*.s' -delete
	find . -type f -name '*.o' -delete
	#rm src/$(EXECUTABLE)

.PHONY: all clean run

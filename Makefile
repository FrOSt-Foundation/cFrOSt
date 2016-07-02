ifeq ($(CC), cc)
	CC := tools/clang
endif
ifeq ($(AS), as)
	AS := tools/assembler
endif

CFLAGS += -ccc-host-triple dcpu16 -O0 -Wall -Wextra -std=c11 -I src

C_FILES := src/include.c
DASM_FILES :=
ASM_FILES := $(C_FILES:.c=.s) $(DASM_FILES:.dasm=.s)
BIN ?= bin/FrOSt.bin

all: $(BIN)

$(BIN): $(ASM_FILES)
	$(COMPILE.s) $^ $(OUTPUT_OPTION)

%.s: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -S $<
	sed -i -re 's/\[(0x[0-9a-f]*)\+([A|B|C|X|Y|Z|I|J])\]/[\2+\1]/g' $@

%.s: %.dasm
	cp -f $< $@

run:
	cd tools && ./run.sh

clean:
	find . -type f -name '*.s' -delete
	find . -type f -name '*.o' -delete
	#rm src/$(EXECUTABLE)

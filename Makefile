SDK_BIN_PATH := bin

ifeq ($(CC), cc)
	CC := $(SDK_BIN_PATH)/clang
endif
ifeq ($(AS), as)
	AS := $(SDK_BIN_PATH)/assembler
endif

CFLAGS += -ccc-host-triple dcpu16 -O0 -Wall -Wextra -std=c11 -I src

C_FILES := src/include.c
DASM_FILES :=
ASM_FILES := $(C_FILES:.c=.s) $(DASM_FILES:.dasm=.s)
BIN ?= tools/FrOSt.bin

all: $(BIN)

$(BIN): $(ASM_FILES)
	$(COMPILE.s) $(OUTPUT_OPTION) $^

.c.o:
	$(CC) $(CFLAGS) $< -S -Iinclude -o $*.s
	find ./ -name "*.s" -exec sed -i -re 's/\[(0x[0-9a-f]*)\+([A|B|C|X|Y|Z|I|J])\]/[\2+\1]/g' {} \;
	find ./ -name "*.s" -exec sed -i -re 's/\.word/.dat/g' {} \;
	#$(AS) -o $*.o $*.s
	#$(YAS) $*.s -o $*.o
	java -jar tools/DCPU-Toolchain.jar assemble src/include.s src/include.o

%.s: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OUTPUT_OPTION) -S $<
	sed -i -re 's/\[(0x[0-9a-f]*)\+([A|B|C|X|Y|Z|I|J])\]/[\2+\1]/g' $@
	touch $<

%.s: %.dasm
	cp -f $< $@

run:
	cd tools && ./run.sh

clean:
	find . -type f -name '*.s' -delete
	find . -type f -name '*.o' -delete
	#rm src/$(EXECUTABLE)

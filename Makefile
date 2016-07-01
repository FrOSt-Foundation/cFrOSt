SDK_BIN_PATH = bin

AS = $(SDK_BIN_PATH)/as
CC = $(SDK_BIN_PATH)/clang
LD = $(SDK_BIN_PATH)/ld
YAS = $(SDK_BIN_PATH)/assembler

CFLAGS = -O0 -Wall
LDFLAGS=

SOURCES=src/include.c

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=tools/FrOSt.bin

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	#$(LD) $(LDFLAGS) $(OBJECTS) -o $@
	cp $(OBJECTS) tools/FrOSt.bin

.c.o:
	$(CC) $(CFLAGS) $< -ccc-host-triple dcpu16 -S -Iinclude -o $*.s
	find ./ -name "*.s" -exec sed -i -re 's/\[(0x[0-9a-f]*)\+([A|B|C|X|Y|Z|I|J])\]/[\2+\1]/g' {} \;
	find ./ -name "*.s" -exec sed -i -re 's/\.word/.dat/g' {} \;
	#$(AS) -o $*.o $*.s
	#$(YAS) $*.s -o $*.o
	java -jar tools/DCPU-Toolchain.jar assemble src/include.s src/include.o

run:
	cd tools && ./run.sh

clean:
	find . -type f -name '*.s' -delete
	find . -type f -name '*.o' -delete
	#rm src/$(EXECUTABLE)

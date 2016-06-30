SDK_BIN_PATH = bin

AS = $(SDK_BIN_PATH)/as
CC = $(SDK_BIN_PATH)/clang
LD = $(SDK_BIN_PATH)/ld
YAS = $(SDK_BIN_PATH)/assembler

CFLAGS = -O2 -Wall
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
	#$(AS) -o $*.o $*.s
	$(YAS) $*.s -o $*.o

run:
	cd tools && ./run.sh

clean:
#	rm kernel/*.o
	rm *.o
	rm src/kernel/*.s
	rm src/drivers/*.s
	rm src/$(EXECUTABLE)

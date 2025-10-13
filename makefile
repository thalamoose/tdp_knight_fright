PATH := $(PATH);$(CURDIR)\tools\z88dk\bin;$(CURDIR)\tools\sjasmplus\bin

SLICER = python scripts/slicer.py
MAPPER = python scripts/charmapgen.py
ASSEMBLER = sjasmplus
IMGGEN = hdfmonkey
CC = $(CURDIR)/tools/z88dk/bin/zcc
CFLAGS=+zxn -SO3 -c
LDFLAGS= --no-crt +zxn -m

OUT=build/assets
C_SRC_DIR=src/c
C_OBJ_DIR=build/c

C_SRCS := $(C_SRC_DIR)/overlay.asm $(wildcard $(C_SRC_DIR)/*.c)
C_OBJS := $(patsubst $(C_SRC_DIR)/%.c,$(C_OBJ_DIR)/%.o,$(C_SRCS))

ASSETS= $(OUT)/kfsprites.bin $(OUT)/kfplayer.bin \
		$(OUT)/kfback.bin $(OUT)/kftiles.bin \
		$(OUT)/shape_01.map $(OUT)/shape_02.map $(OUT)/charset.bin \
		build/c_code.bin \
		makefile

SOURCES=$(wildcard include/*.inc src/*.asm *.asm)
SDCARD=build/kf.img
EXECUTABLE=build/KnightFright.nex

all: $(OUT) $(C_OBJ_DIR) executable

$(OUT)/kfsprites.bin: assets/kfsprites.png makefile
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)

$(OUT)/kfplayer.bin: assets/kfplayer.png makefile
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)

$(OUT)/kfback.bin: assets/kfback.png makefile
	$(SLICER) $< $@ --palette=$(@:.bin=.pal)

$(OUT)/kftiles.bin: assets/kftiles.png makefile
	$(SLICER) $< $@ --tile --palette=$(@:.bin=.pal)

$(OUT)/shape_01.map: assets/kftiles.png assets/shape_01.png
	$(MAPPER) $^ $@

$(OUT)/shape_02.map: assets/kftiles.png assets/shape_02.png
	$(MAPPER) $^ $@

$(OUT)/charset.bin: assets/charset.png assets/charset.bin
	$(SLICER) $< $@ --tile

executable: $(EXECUTABLE)

$(EXECUTABLE): KnightFright.asm $(OUT) $(ASSETS) $(SOURCES)
	$(ASSEMBLER) --msg=war --lst=$(@:.nex=.lst) --sld=$(@:.nex=.sld) --fullpath KnightFright.asm

$(OUT):
	mkdir $(subst /,\,$@)

$(C_OBJ_DIR):
	mkdir $(subst /,\,$@)

$(C_OBJ_DIR)/%.o: $(C_SRC_DIR)/%.c $(C_OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $@

build/c_code.bin: $(C_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

image: $(SDCARD)

$(SDCARD): $(ASSETS) $(EXECUTABLE)
	$(IMGGEN) create $(SDCARD) 1M
	$(IMGGEN) mkdir $(SDCARD) /KFRIGHT
	$(IMGGEN) put $(SDCARD) $(ASSETS) /KFRIGHT
	$(IMGGEN) put $(SDCARD) $(EXECUTABLE) /KFRIGHT/KFRIGHT.NEX

clean:
	del /q *.sld *.lst *.nex
	del /s /q build

.DELETE_ON_ERROR:

PATH:=$(PATH);$(CURDIR)/../z88dk/bin
SLICER = @python scripts/slicer.py
MAPPER = @python scripts/charmapgen.py
ASSEMBLER = @$(CURDIR)/../z88dk/bin/z80asm
IMGGEN = hdfmonkey
CC = $(CURDIR)/../z88dk/bin/zcc
MV = @move /y
MKDIR= mkdir
BASEFLAGS=+zxn -mz80n -m -s --list -g -Iinclude/
CFLAGS=$(BASEFLAGS) -SO3 -c --c-code-in-asm
LDFLAGS= $(BASEFLAGS) -startup=1 -pragma-include:src/zpragma.inc
ASFLAGS= $(BASEFLAGS) -SO3 -c --debug

OUT=build/assets
ASM_SRC_DIR=src/asm
ASM_OBJ_DIR=build/asm

C_SRC_DIR=src
C_OBJ_DIR=build/c
ASM_OBJ_DIR=build/asm

C_SRCS := $(wildcard $(C_SRC_DIR)/*.c)
C_OBJS := $(patsubst $(C_SRC_DIR)/%.c,$(C_OBJ_DIR)/%.o,$(C_SRCS))

ASSETS= $(OUT)/kfsprites.bin $(OUT)/kfplayer.bin \
		$(OUT)/kfback.bin $(OUT)/kftiles.bin \
		$(OUT)/shape_01.map $(OUT)/shape_02.map $(OUT)/charset.bin \
		makefile

##ASM_SRCS := $(wildcard $(ASM_SRC_DIR)/*.asm)
ASM_SRCS := $(ASM_SRC_DIR)/initialize.asm $(ASM_SRC_DIR)/interrupts.asm \
			$(ASM_SRC_DIR)/tilemap.asm $(ASM_SRC_DIR)/stubs.asm \
			$(ASM_SRC_DIR)/utilities.asm $(ASM_SRC_DIR)/dma.asm

ASM_OBJS := $(patsubst $(ASM_SRC_DIR)/%.asm,$(ASM_OBJ_DIR)/%.o,$(ASM_SRCS))
SOURCES=
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

executable: $(OUT) src/zpragma.inc $(ASSETS) $(EXECUTABLE)

##$(EXECUTABLE): KnightFright.asm $(OUT) $(ASSETS) $(SOURCES)
##	$(ASSEMBLER) --msg=war --lst=$(@:.nex=.lst) --sld=$(@:.nex=.sld) --fullpath KnightFright.asm

$(EXECUTABLE): $(C_OBJS) $(ASM_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ -create-app -subtype=nex

$(OUT):
	$(MKDIR) $(subst /,\,$@)

$(C_OBJ_DIR):
	$(MKDIR) $(subst /,\,$@)

$(ASM_OBJ_DIR):
	$(MKDIR) $(subst /,\,$@)
	$(MKDIR) build\lst

$(C_OBJ_DIR)/%.o: $(C_SRC_DIR)/%.c $(C_OBJ_DIR) makefile
	$(CC) $(CFLAGS) $< -o $@
	$(MV) $(subst /,\,$<.lis) $(subst /,\,build/lst/) >nul:
	$(MV) $(subst /,\,$<.sym) $(subst /,\,build/lst/) >nul:

$(ASM_OBJ_DIR)/%.o: $(ASM_SRC_DIR)/%.asm $(ASM_OBJ_DIR) makefile
	$(CC) $(ASFLAGS) $< -o $@
	$(MV) $(subst /,\,$<.lis) $(subst /,\,build/lst/) >nul:
	$(MV) $(subst /,\,$<.sym) $(subst /,\,build/lst/) >nul:

$(ASM_SRC_DIR)/tilemap.asm : $(OUT)/shape_02.map

image: $(SDCARD)

$(SDCARD): $(ASSETS) $(EXECUTABLE)
	$(IMGGEN) create $(SDCARD) 1M
	$(IMGGEN) mkdir $(SDCARD) /KFRIGHT
	$(IMGGEN) put $(SDCARD) $(ASSETS) /KFRIGHT
	$(IMGGEN) put $(SDCARD) $(EXECUTABLE) /KFRIGHT/KFRIGHT.NEX

clean:
	@del /q/s *.sld *.lst *.nex *.map *.lis *.sym >nul:
	@del /s /q build >nul:

.DELETE_ON_ERROR:

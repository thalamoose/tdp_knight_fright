SILENCE=@

PATH:=$(PATH);$(CURDIR)/../z88dk/bin
SLICER = $(SILENCE)python scripts/slicer.py
MAPPER = $(SILENCE)python scripts/charmapgen.py
ASSEMBLER = $(SILENCE)sjasmplus
GENSTRUCTS = $(SILENCE)python scripts/genstructs.py
SEGMENT = $(SILENCE)python scripts/slice_and_bank.py
IMGGEN = hdfmonkey
CC = $(SILENCE)$(CURDIR)/../z88dk/bin/zcc
MV = $(SILENCE)move /y
RM = $(SILENCE)del /q
ECHO = $(SILENCE)echo
MKDIR= $(SILENCE)mkdir
RMDIR= $(SILENCE)rmdir
BASEFLAGS=+zxn -mz80n -m -s --list -g -Iinclude
OPT_FLAGS=-SO2 --opt-code-speed --max-allocs-per-node98304 --allow-unsafe-read
CFLAGS=$(BASEFLAGS) -c --c-code-in-asm $(OPT_FLAGS) -clib=new -compiler=sdcc
DEP_FLAGS = -MT $@ -MD -MF $(DEP_DIR)/$*.d
CODE_PAGE = 16
SECTIONS =  --datasegPAGE_$(CODE_PAGE) --codesegPAGE_$(CODE_PAGE) --constsegPAGE_$(CODE_PAGE) --bsssegPAGE_$(CODE_PAGE)
#
# Startup file located at z88dk\libsrc\_DEVELOPMENT\target\zxn\startup\zxn_crt_31.asm
#
LDFLAGS= $(BASEFLAGS) -startup=31 -pragma-include:src/zpragma.inc
ASFLAGS= $(BASEFLAGS) -c --debug

OUT=build/assets
ASM_SRC_DIR=src/asm
ASM_OBJ_DIR=build/asm
LST_DIR=build/lst

C_SRC_DIR=src
C_INC_DIR=include
C_OBJ_DIR=build/c
ASM_OBJ_DIR=build/asm
DEP_DIR=build/deps

C_SRCS := $(wildcard $(C_SRC_DIR)/*.c)
C_ENEMY_SRCS := $(wildcard $(C_SRC_DIR)/enemies/*.c)
C_OBJECT_SRCS := $(wildcard $(C_SRC_DIR)/objects/*.c) 
C_HDRS := $(wildcard $(C_INC_DIR)/*.h) $(wildcard $(C_INC_DIR)/enemies/*.h) $(wildcard $(C_INC_DIR)/objects/*.h)
C_OBJS := $(patsubst $(C_SRC_DIR)/%.c,$(C_OBJ_DIR)/%.o,$(C_SRCS)) \
		  $(patsubst $(C_SRC_DIR)/enemies/%.c,$(C_OBJ_DIR)/enemies/%.o,$(C_ENEMY_SRCS)) \
		  $(patsubst $(C_SRC_DIR)/objects/%.c,$(C_OBJ_DIR)/objects/%.o,$(C_OBJECT_SRCS))

C_SYMS := $(patsubst $(C_SRC_DIR)/%.c,$(C_OBJ_DIR)/%.sym.o,$(C_SRCS))
DEP_FILES := $(C_SRCS:src/%.c=$(DEP_DIR)/%.d)

ASSETS= $(OUT)/sprites.bin $(OUT)/player.bin \
		$(OUT)/bear.bin $(OUT)/bighopper.bin \
		$(OUT)/coin.bin $(OUT)/colorchanger.bin \
		$(OUT)/follower.bin $(OUT)/spike.bin \
		$(OUT)/obstacles.bin \
		$(OUT)/kfback.bin $(OUT)/tiles.bin \
		$(OUT)/shape_01.map $(OUT)/shape_02.map $(OUT)/charset.bin \
		$(OUT)/numbers.bin \
		makefile

##ASM_SRCS := $(wildcard $(ASM_SRC_DIR)/*.asm)
ASM_SRCS := $(ASM_SRC_DIR)/interrupts.asm \
			$(ASM_SRC_DIR)/print.asm \
			$(ASM_SRC_DIR)/utilities.asm $(ASM_SRC_DIR)/dma.asm \
			$(ASM_SRC_DIR)/particles.asm

ASM_OBJS := $(patsubst $(ASM_SRC_DIR)/%.asm,$(ASM_OBJ_DIR)/%.o,$(ASM_SRCS))

SDCARD=build/kf.img
EXECUTABLE=build/KnightFright.nex
SYMBOL_FILE=build/KnightFright.sld

##
## Default rule
##
all:  $(OUT) $(DEP_DIR) $(LST_DIR) $(C_OBJ_DIR) executable $(SYMBOL_FILE) 

$(OUT)/kfback.bin: assets/kfback.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) $< $@ --palette=$(@:.bin=.pal)
	$(SEGMENT) $@ $@.s 42

$(OUT)/bear.bin: assets/bear.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)
	$(SEGMENT) $@ $@.s 74

$(OUT)/bighopper.bin: assets/bighopper.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)
	$(SEGMENT) $@ $@.s 82

$(OUT)/colorchanger.bin: assets/colorchanger.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)
	$(SEGMENT) $@ $@.s 90

$(OUT)/follower.bin: assets/follower.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)
	$(SEGMENT) $@ $@.s 98

$(OUT)/player.bin: assets/player.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)
	$(SEGMENT) $@ $@.s 106

$(OUT)/spike.bin: assets/spike.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)
	$(SEGMENT) $@ $@.s 114

$(OUT)/sprites.bin: assets/sprites.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)

$(OUT)/tiles.bin: assets/tiles.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) $< $@ --tile --palette=$(@:.bin=.pal)

$(OUT)/numbers.bin: assets/numbers.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) --tile $< $@ --palette=$(@:.bin=.pal)

$(OUT)/%.map: assets/tiles.png assets/%.png
	$(ECHO) Mapping $<...
	$(MAPPER) $^ $@

$(OUT)/charset.bin: assets/charset.png assets/charset.bin
	$(ECHO) Mapping $<...
	$(SLICER) $< $@ --tile

$(OUT)/coin.bin: assets/coin.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) --size=16,16 --sprite $< $@ --palette=$(@:.bin=.pal)
	$(SEGMENT) $@ $@.s 56

$(OUT)/obstacles.bin: assets/obstacles.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)
	$(SEGMENT) $@ $@.s 58


executable: $(OUT) src/zpragma.inc $(ASSETS) $(EXECUTABLE) $(SYMBOLS)

$(EXECUTABLE): $(C_OBJS) $(ASM_OBJS)
	$(ECHO) Linking $@...
	$(CC) $(LDFLAGS) $^ -o $@ -create-app -subtype=nex > nul:
	$(RM) build\*_PAGE_*.bin

$(C_SRC_DIR)/assets.c: $(ASSETS)

$(OUT):
	$(MKDIR) $(subst /,\,$@)

$(C_OBJ_DIR):
	$(MKDIR) $(subst /,\,$@)
	$(MKDIR) $(subst /,\,$@/enemies)
	$(MKDIR) $(subst /,\,$@/objects)

$(ASM_OBJ_DIR):
	$(MKDIR) $(subst /,\,$@)

$(LST_DIR):
	$(MKDIR) $(subst /,\,$@)

$(C_OBJ_DIR)/%.o: $(C_SRC_DIR)/%.c makefile
	$(ECHO) Compiling $<...
	$(CC) $(CFLAGS) $(DEP_FLAGS) $< -o $@
	$(MV) $(subst /,\,$<.lis) $(subst /,\,build/lst/) >nul:
	$(MV) $(subst /,\,$<.sym) $(subst /,\,build/lst/) >nul:

$(C_OBJ_DIR)/objects/%.o : $(C_SRC_DIR)/objects/%.c makefile
	$(ECHO) Compiling (overlay) $<...
	$(CC) $(CFLAGS) $(SECTIONS) $(DEP_FLAGS) $< -o $@
	$(MV) $(subst /,\,$<.lis) $(subst /,\,build/lst/) >nul:
	$(MV) $(subst /,\,$<.sym) $(subst /,\,build/lst/) >nul:

$(C_OBJ_DIR)/enemies/%.o: $(C_SRC_DIR)/enemies/%.c makefile
	$(ECHO) Compiling (overlay) $<...
	$(CC) $(CFLAGS) $(SECTIONS) $(DEP_FLAGS) $< -o $@
	$(MV) $(subst /,\,$<.lis) $(subst /,\,build/lst/) >nul:
	$(MV) $(subst /,\,$<.sym) $(subst /,\,build/lst/) >nul:

$(ASM_OBJ_DIR)/%.o: $(ASM_SRC_DIR)/%.asm $(ASM_OBJ_DIR) makefile
	$(ECHO) Assembling $<...
	$(CC) $(ASFLAGS) $< -o $@
	$(MV) $(subst /,\,$<.lis) $(subst /,\,build/lst/) >nul:
	$(MV) $(subst /,\,$<.sym) $(subst /,\,build/lst/) >nul:

$(DEP_DIR): 
	$(MKDIR) $(subst /,\,$@)
	$(MKDIR) $(subst /,\,$@/enemies)
	$(MKDIR) $(subst /,\,$@/objects)

-include $(DEP_FILES)

#
# Build SD Card image
#
image: $(SDCARD)

$(SDCARD): $(ASSETS) $(EXECUTABLE)
	$(IMGGEN) create $(SDCARD) 1M
	$(IMGGEN) mkdir $(SDCARD) /KFRIGHT
	$(IMGGEN) put $(SDCARD) $(ASSETS) /KFRIGHT
	$(IMGGEN) put $(SDCARD) $(EXECUTABLE) /KFRIGHT/KFRIGHT.NEX

clean: $(OUT)
	$(RMDIR) /s/q build >nul: 2>nul:

.DELETE_ON_ERROR:

GCC_WARN_DISABLE= -Wno-builtin-declaration-mismatch
GCC_SYM_OPTS= $(GCC_WARN_DISABLE) -fno-eliminate-unused-debug-types -std=gnu99 -ffreestanding -g

symbols: build/KnightFright.sld

COMPILER_DIR=../arm-none-linux-gnueabihf/bin
SYM_GCC=$(SILENCE)$(COMPILER_DIR)/arm-none-linux-gnueabihf-gcc

build/KnightFright.sld : $(C_SYMS)
	$(ECHO) Generating symbols $@.s...
	$(GENSTRUCTS) -o $@.s $(C_SYMS)
	$(ECHO) Generating symbol file $@
	$(ASSEMBLER) --msg=war --nologo --fullpath --sld=$@ $@.s 

$(C_OBJ_DIR)/%.sym.o: $(C_SRC_DIR)/%.c makefile
	$(ECHO) Compiling symbols for $<
	$(SYM_GCC) $(GCC_SYM_OPTS) -Iinclude $< -c -o $@

test: build/test_structs.asm

build/test_structs.asm : test_structs.c scripts/genstructs.py makefile $(OUT)
	$(ECHO) Generating symbols $@...
	$(SYM_GCC) $(GCC_SYM_OPTS) -Iinclude $< -c -o $@.o
	$(GENSTRUCTS) -o $@ $@.o

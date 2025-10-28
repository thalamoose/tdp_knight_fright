PATH:=$(PATH);$(CURDIR)/../z88dk/bin
SLICER = @python scripts/slicer.py
MAPPER = @python scripts/charmapgen.py
ASSEMBLER = @sjasmplus
GENSTRUCTS = python scripts/genstructs.py
SEGMENT = @python scripts/slice_and_bank.py
IMGGEN = hdfmonkey
SILENCE=@
CC = $(SILENCE)$(CURDIR)/../z88dk/bin/zcc
MV = $(SILENCE)move /y
RM = $(SILENCE)del /s/q
ECHO = $(SILENCE)echo
MKDIR= $(SILENCE)mkdir
RMDIR= $(SILENCE)rmdir
BASEFLAGS=+zxn -mz80n -m -s --list -g -Iinclude
CFLAGS=$(BASEFLAGS) -SO0 -c --c-code-in-asm $(OPT_FLAGS)
DEP_FLAGS = -MT $@ -MD -MF $(DEP_DIR)/$*.d
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
C_HDRS := $(wildcard $(C_INC_DIR)/*.h)
C_OBJS := $(patsubst $(C_SRC_DIR)/%.c,$(C_OBJ_DIR)/%.o,$(C_SRCS))
C_SYMS := $(patsubst $(C_SRC_DIR)/%.c,$(C_OBJ_DIR)/%.sym.o,$(C_SRCS))
DEP_FILES := $(C_SRCS:src/%.c=$(DEP_DIR)/%.d)

ASSETS= $(OUT)/kfsprites.bin $(OUT)/kfplayer.bin \
		$(OUT)/kfback.bin $(OUT)/kftiles.bin \
		$(OUT)/shape_01.map $(OUT)/shape_02.map $(OUT)/charset.bin \
		makefile

##ASM_SRCS := $(wildcard $(ASM_SRC_DIR)/*.asm)
ASM_SRCS := $(ASM_SRC_DIR)/initialize.asm $(ASM_SRC_DIR)/interrupts.asm \
			$(ASM_SRC_DIR)/stubs.asm \
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
	$(SEGMENT) $@ $@.s 16

$(OUT)/kfplayer.bin: assets/kfplayer.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)
	$(SEGMENT) $@ $@.s 21

$(OUT)/kfsprites.bin: assets/kfsprites.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)

$(OUT)/kftiles.bin: assets/kftiles.png makefile
	$(ECHO) Slicing $<...
	$(SLICER) $< $@ --tile --palette=$(@:.bin=.pal)

$(OUT)/%.map: assets/kftiles.png assets/%.png
	$(ECHO) Mapping $<...
	$(MAPPER) $^ $@

$(OUT)/charset.bin: assets/charset.png assets/charset.bin
	$(ECHO) Mapping $<...
	$(SLICER) $< $@ --tile

executable: $(OUT) src/zpragma.inc $(ASSETS) $(EXECUTABLE) $(SYMBOLS)

$(EXECUTABLE): $(C_OBJS) $(ASM_OBJS)
	$(ECHO) Linking $@...
	$(CC) $(LDFLAGS) $^ -o $@ -create-app -subtype=nex


$(C_SRC_DIR)/assets.c: $(ASSETS)

$(OUT):
	$(MKDIR) $(subst /,\,$@)

$(C_OBJ_DIR):
	$(MKDIR) $(subst /,\,$@)

$(ASM_OBJ_DIR):
	$(MKDIR) $(subst /,\,$@)

$(LST_DIR):
	$(MKDIR) $(subst /,\,$@)

$(C_OBJ_DIR)/%.o: $(C_SRC_DIR)/%.c $(C_OBJ_DIR) makefile
	$(ECHO) Compiling $<...
	$(CC) $(CFLAGS) $(DEP_FLAGS) $< -o $@
	$(MV) $(subst /,\,$<.lis) $(subst /,\,build/lst/) >nul:
	$(MV) $(subst /,\,$<.sym) $(subst /,\,build/lst/) >nul:

$(ASM_OBJ_DIR)/%.o: $(ASM_SRC_DIR)/%.asm $(ASM_OBJ_DIR) makefile
	$(ECHO) Assembling $<...
	$(CC) $(ASFLAGS) $< -o $@
	$(MV) $(subst /,\,$<.lis) $(subst /,\,build/lst/) >nul:
	$(MV) $(subst /,\,$<.sym) $(subst /,\,build/lst/) >nul:

$(DEP_DIR): 
	$(MKDIR) $(subst /,\,$@)

##-include $(DEP_FILES)

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
LD_SYM_OPTS= -r

symbols: build/KnightFright.sld

COMPILER_DIR=../arm-none-linux-gnueabihf/bin
SYM_GCC=$(SILENCE)$(COMPILER_DIR)/arm-none-linux-gnueabihf-gcc

build/KnightFright.sld : $(C_SYMS)
	$(ECHO) Generating symbols $@.s...
	$(GENSTRUCTS) -o $@.s $(C_SYMS)
	$(ECHO) Generating symbol file $@
	$(ASSEMBLER) --msg=war --nologo --fullpath --sld=$@ $@.s 

$(C_OBJ_DIR)/%.sym.o: $(C_SRC_DIR)/%.c $(C_OBJ_DIR) makefile
	$(ECHO) Compiling symbols for $<
	$(SYM_GCC) $(GCC_SYM_OPTS) -Iinclude $< -c -o $@

test: build/test_structs.asm

build/test_structs.asm : test_structs.c scripts/genstructs.py makefile $(OUT)
	$(ECHO) Generating symbols $@...
	$(SYM_GCC) $(GCC_SYM_OPTS) -Iinclude $< -c -o $@.o
	$(GENSTRUCTS) -o $@ $@.o

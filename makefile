SLICER = python scripts/slicer.py
MAPPER = python scripts/charmapgen.py
ASSEMBLER = sjasmplus
IMGGEN = hdfmonkey

OUT=build/assets

ASSETS= $(OUT)/kfsprites.bin $(OUT)/kfplayer.bin \
        $(OUT)/kfback.bin $(OUT)/kftiles.bin \
        $(OUT)/shape_01.map $(OUT)/charset.bin \
        makefile

SOURCES=$(wildcard include/*.inc src/*.asm *.asm)
SDCARD=build/kf.img
EXECUTABLE=build/KnightFright.nex

all: executable

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

$(OUT)/charset.bin: assets/charset.png assets/charset.bin
    $(SLICER) $< $@ --tile

executable: $(EXECUTABLE)

$(EXECUTABLE): KnightFright.asm $(OUT) $(ASSETS) $(SOURCES)
    $(ASSEMBLER) --msg=war --lst=$(@:.nex=.lst) --sld=$(@:.nex=.sld) --fullpath KnightFright.asm

$(OUT):
    mkdir -p $(subst /,\,$@)


image: $(SDCARD)

$(SDCARD): $(ASSETS) $(EXECUTABLE)
    $(IMGGEN) create $(SDCARD) 1M
    $(IMGGEN) mkdir $(SDCARD) /KFRIGHT
    $(IMGGEN) put $(SDCARD) $(ASSETS) /KFRIGHT
    $(IMGGEN) put $(SDCARD) $(EXECUTABLE) /KFRIGHT/KFRIGHT.NEX

clean:
    del *.sld *.lst *.nex
    del /s/q build

.DELETE_ON_ERROR:

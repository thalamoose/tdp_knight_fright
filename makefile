SLICER = python scripts/slicer.py
MAPPER = python scripts/charmapgen.py

all: executable

assets:  build/assets build/assets/kfsprites.bin build/assets/kfback.bin build/assets/kftiles.bin build/assets/shape_01.map makefile

build/assets/kfsprites.bin: assets/kfsprites.png makefile
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)

build/assets/kfback.bin: assets/kfback.png makefile
	$(SLICER) $< $@ --crop 32,24,256,192 --palette=$(@:.bin=.pal)

build/assets/kftiles.bin: assets/kftiles.png makefile
	$(SLICER) $< $@ --tile --palette=$(@:.bin=.pal)

build/assets/shape_01.map: assets/kftiles.png assets/shape_01.png
	$(MAPPER) $^ $@

executable: build/KnightFright.nex

DEPENDENCIES=$(wildcard src/*.inc src/*.asm)

build/KnightFright.nex: KnightFright.asm assets $(DEPENDENCIES)
	sjasmplus --lst=$(@:.nex=.lst) --sld=$(@:.nex=.sld) --fullpath KnightFright.asm

build/assets:
	mkdir -p $(subst /,\,$@)

clean:
	del *.sld *.lst *.nex
	del /s/q build


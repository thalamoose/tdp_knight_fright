SLICER = python scripts/slicer.py

all: assets executable

assets: build/assets build/assets/kfsprites.bin build/assets/kfback.bin build/assets/kftiles.bin makefile

build/assets/kfsprites.bin: assets/kfsprites.png makefile
	$(SLICER) --size=32,32 --sprite $< $@ --palette=$(@:.bin=.pal)

build/assets/kfback.bin: assets/kfback.png makefile
	$(SLICER) $< $@ --crop 32,24,256,192 --palette=$(@:.bin=.pal)

build/assets/kftiles.bin: assets/kftiles.png makefile
	$(SLICER) $< $@ --tile --palette=$(@:.bin=.pal)

executable: build/KnightFright.nex

DEPENDENCIES=$(wildcard src/*.inc src/*.asm)

build/KnightFright.nex: KnightFright.asm $(DEPENDENCIES)
	sjasmplus --lst=$(@:.nex=.lst) --sld=$(@:.nex=.sld) --fullpath KnightFright.asm

build/assets:
	mkdir -p $(subst /,\,$@)

clean:
	del *.sld *.lst *.nex
	del /s/q build




	commit 47b9eb77f58333adebca921876eaf2b07394f50e (HEAD -> main)
Author: Brian Watson <biscuit@grovelandgames.com>
Date:   Mon Sep 1 20:48:30 2025 +0100

    Added 16 colour support to the slicer. Added tilemap

commit 63907f4ef7f542a595fa225fc1e389e66884f99b
Author: Brian Watson <biscuit@grovelandgames.com>
Date:   Mon Sep 1 12:21:41 2025 +0100

    Added palette load for l2 screen, prep for tilemap.

commit d769ae5f4e4c552a4612f1e06fd5d700ca415f30
Author: Brian Watson <biscuit@grovelandgames.com>
Date:   Sun Aug 31 22:45:58 2025 +0100

    Moved files around.

commit 205e9c2a8b62ba80375c193e02ae2c090f82ca47
Author: Brian Watson <biscuit@grovelandgames.com>
Date:   Sun Aug 31 20:01:00 2025 +0100

    Add makefile, add script to slice a bitmap up.

commit d5cfb1b7e8e7a38d49c62cb1fd69cee8c283addc
Author: Brian Watson <biscuit@grovelandgames.com>
Date:   Sat Aug 30 16:00:41 2025 +0100

    Some simple animation routines added.

commit 87d87e548f7ce4b4cae1c2037de79802eddefc17
Author: Brian Watson <biscuit@grovelandgames.com>
Date:   Fri Aug 29 17:24:46 2025 +0100

    Got some basic sprite (16x16@8bit), and backdrop working (256x192@8bit).

commit a2079a192a556829475249fc438d84700822f134
Author: Brian Watson <biscuit@grovelandgames.com>
Date:   Mon Aug 25 22:16:36 2025 +0100

    Removed extras
TARGET = funkin
TYPE = ps-exe

SRCS = src/main.c \
       src/mutil.c \
       src/random.c \
       src/archive.c \
       src/font.c \
       src/trans.c \
       src/loadscr.c \
       src/menu.c \
       src/stage.c \
       src/debug.c \
       src/save.c \
       src/psx/psx.c \
       src/psx/io.c \
       src/psx/gfx.c \
       src/psx/audio.c \
       src/psx/pad.c \
       src/psx/timer.c \
       src/stage/storage.c \
	   src/stage/jrs.c \
	   src/stage/deskroom.c \
       src/stage/black.c \
	   src/stage/week1.c \
	   src/stage/officeg.c \
	   src/stage/stage01.c \
	   src/stage/officeb.c \
	   src/stage/fnaf3.c \
	   src/stage/flipside.c \
	   src/stage/takecake.c \
	   src/stage/ngroom.c \
	   src/stage/black2.c \
	   src/stage/samroom.c \
	   src/stage/dummy.c \
       src/animation.c \
       src/character.c \
       src/character/bf.c \
       src/character/gf.c \
       src/character/gfo.c \
	   src/character/ourple.c \
	   src/character/prange.c \
	   src/character/henry.c \
	   src/character/henrytv.c \
	   src/character/bfp.c \
	   src/character/henryp.c \
	   src/character/sunny.c \
	   src/character/matpat.c \
	   src/character/lphone.c \
       src/character/fatjones.c \
	   src/character/gphone.c \
	   src/character/goldenf.c \
	   src/character/fbear.c \
	   src/character/springb.c \
	   src/character/mark.c \
	   src/character/freddy.c \
	   src/character/gfreddy.c \
	   src/character/foxy.c \
	   src/character/salvage.c \
	   src/character/cassidy.c \
	   src/character/omc.c \
	   src/character/plush.c \
	   src/character/cc.c \
	   src/character/abdul.c \
	   src/character/cakebear.c \
	   src/character/jackass.c \
	   src/character/ngt.c \
	   src/character/gino.c \
	   src/character/strap.c \
	   src/character/mangle.c \
	   src/character/criminal.c \
	   src/character/henrym.c \
	   src/character/dad.c \
       src/character/speaker.c \
       src/object.c \
       src/object/combo.c \
       src/object/splash.c \
       src/pause.c \
       mips/common/crt0/crt0.s

CPPFLAGS += -Wall -Wextra -pedantic -mno-check-zero-division
LDFLAGS += -Wl,--start-group
# TODO: remove unused libraries
LDFLAGS += -lapi
#LDFLAGS += -lc
LDFLAGS += -lc2
LDFLAGS += -lcard
LDFLAGS += -lcd
#LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
#LDFLAGS += -lgs
#LDFLAGS += -lgte
#LDFLAGS += -lgun
#LDFLAGS += -lhmd
#LDFLAGS += -lmath
LDFLAGS += -lmcrd
#LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
#LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
#LDFLAGS += -ltap
LDFLAGS += -flto -Wl,--end-group

include mips/common.mk

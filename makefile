# setup

#export CC = arm-linux-gnueabi-gcc

#CFLAGS += -march=armv5tej
#CFLAGS += -mtune=arm926ej-s
CFLAGS += -O2
#CFLAGS += -march=native
CFLAGS += -Wno-maybe-uninitialized
export CFLAGS

export LDLIBS += -L/srv/git/bse/libbse -lbse

export SRC_DIR = source

export TARGET = uvm

# build/recipies

include git/libbse/makefiles/common_setup.mk

all:
	$(MAKE) -f git/libbse/makefiles/build_liba.mk
	$(MAKE) -f git/libbse/makefiles/build_libso.mk

include git/libbse/makefiles/common.mk

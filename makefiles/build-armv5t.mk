# setup

ARCH = armv5t

export CC = arm-linux-gnueabi-gcc

CFLAGS += -march=armv5tej
CFLAGS += -mtune=arm926ej-s

# build/recipies

include makefiles/common.mk

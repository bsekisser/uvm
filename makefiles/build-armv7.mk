# setup

ARCH = armv7

export CC = arm-linux-gnueabi-gcc

CFLAGS += -march=armv7-a

# build/recipies

include makefiles/common.mk

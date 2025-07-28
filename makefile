ARCHs = aarch64 armv5t armv7

all: $(addprefix build-, $(ARCHs))

build-%:
	$(MAKE) -f makefiles/$@.mk

include git/libbse/makefiles/common.mk


# name of binary
BINARY := libgcc.a

#include config if not defined

ifndef CONFIG
	CONFIG ?= ../../make.config
	include $(CONFIG)
endif



# determine the systemroot dir to place the output
SYSROOT ?= ../../build/sysroot

# determine the dir to place the lib
LIBDIR ?= $(SYSROOT)/usr/lib

all: $(BINARY)

clean:

clean-all: 
	@rm -f $(BINARY)

$(BINARY):
	@$(AR) rcs $@

install-headers:

install: $(BINARY)
	@mkdir -p $(LIBDIR)
	@cp  $(BINARY) $(LIBDIR)
	

.PHONY: all clean clean-all install-headers  install

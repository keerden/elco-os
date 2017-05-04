# subdirs to scan for files
PROJDIRS := kernel

#include config if not defined

ifndef CONFIG
	CONFIG ?= ../make.config
	include $(CONFIG)
endif

# determine the systemroot dir to place the output
SYSROOT ?= $(PWD)/../build/sysroot
export SYSROOT

all: install-headers install

clean:
	@for proj in $(PROJDIRS); do \
		$(MAKE) clean -C $$proj; \
	done

clean-all:
	@for proj in $(PROJDIRS); do \
		$(MAKE) clean-all -C $$proj; \
	done

install-headers:
	@for proj in $(PROJDIRS); do \
		$(MAKE) install-headers -C $$proj; \
	done
install: 
	@for proj in $(PROJDIRS); do \
		$(MAKE) install -C $$proj; \
	done

.PHONY: all clean clean-all install-headers install $(PROJDIRS)

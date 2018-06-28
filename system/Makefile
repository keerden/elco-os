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

# directory containing the source of the (new to compile) system-wide global headerfiles 
NEWINCDIR ?= include


# location within sysroot that contains all system-wide header files that already exist
HEADERDIR ?= $(SYSROOT)/usr/include

#find al systemwide global headers

HDRFILES := $(shell find  $(NEWINCDIR) -maxdepth 2 -type f  -name "*.h")
INSTALLHDRFILES := $(patsubst $(NEWINCDIR)/%, $(HEADERDIR)/%,$(HDRFILES))






all: install-headers install


$(INSTALLHDRFILES) : $(HDRFILES) 
	 test -d $(shell dirname $@) || mkdir -p $(shell dirname $@) && cp -v $< $@

test-headers: $(INSTALLHDRFILES)
test-2:
	echo $(HDRFILES)
	echo $(INSTALLHDRFILES)




clean:
	@for proj in $(PROJDIRS); do \
		$(MAKE) clean -C $$proj; \
	done

clean-all:
	@for proj in $(PROJDIRS); do \
		$(MAKE) clean-all -C $$proj; \
	done

install-headers: $(INSTALLHDRFILES)
	@for proj in $(PROJDIRS); do \
		$(MAKE) install-headers -C $$proj; \
	done
install: 
	@for proj in $(PROJDIRS); do \
		$(MAKE) install -C $$proj; \
	done

.PHONY: all clean clean-all install-headers install $(PROJDIRS)

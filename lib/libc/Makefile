# subdirs to scan for files
PROJDIRS := stdio stdlib string

# name of binary
BINARY := libc.a

#include config if not defined

ifndef CONFIG
	CONFIG ?= ../../make.config
	include $(CONFIG)
endif


# automatically scan for source files
CFILES := $(shell find $(PROJDIRS) -type f -name "*.c")
ASMFILES := $(shell find $(PROJDIRS) -type f -name "*.S")
HDRFILES := $(shell find $(PROJDIRS) -type f -name "*.h")

# make a list of object files
COBJFILES := $(patsubst %.c,%.o,$(CFILES))
ASMOBJFILES := $(patsubst %.S,%.o,$(ASMFILES))

OBJFILES := $(COBJFILES) $(ASMOBJFILES)

# make a list of dependency files
CDEPFILES    := $(patsubst %.c,%.d,$(CFILES))
ASMDEPFILES    := $(patsubst %.S,%.d,$(ASMFILES))

DEPFILES := $(CDEPFILES) $(ASMDEPFILES)

ALLFILES := $(CFILES) $(ASMFILES) $(HDRFILES)

# determine the systemroot dir to place the output
SYSROOT ?= ../../build/sysroot

# determine the dir to place the lib
LIBDIR ?= $(SYSROOT)/usr/lib

# directory containing the source of the (new to compile) system-wide headerfiles 
NEWINCDIR ?= include


# location within sysroot that contains all system-wide header files that already exist
HEADERDIR ?= $(SYSROOT)/usr/include

HDRFILES := $(shell find $(NEWINCDIR) -type f -name "*.h")
INSTALLHDRFILES := $(patsubst $(NEWINCDIR)/%, $(HEADERDIR)/%,$(HDRFILES))


# enabled compiler warnings
WARNINGS := -Wall -Wextra 
#-pedantic -Wshadow -Wpointer-arith -Wcast-align \
           # -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
          #  -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
          #  -Wuninitialized -Wconversion -Wstrict-prototypes

#definitions
DEFS := 
#include paths
CINC := --sysroot=$(SYSROOT) -isystem $(HEADERDIR) -I $(NEWINCDIR)

CFLAGS :=  $(CFLAGS) $(CINC)   $(WARNINGS) $(DEFS) -O2 -g  -ffreestanding -fbuiltin -std=gnu11

all: $(BINARY)

clean: 
	@rm -f $(OBJFILES) $(DEPFILES) *.o */*.o */*/*.o

clean-all: clean
	@rm -f $(BINARY)

$(BINARY): $(OBJFILES)
	@$(AR) rcs $@ $(OBJFILES)

%.o: %.c Makefile
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@


%.o: %.S Makefile
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@ 


install-headers: $(INSTALLHDRFILES)
	
$(INSTALLHDRFILES) : $(HDRFILES) 
	test -d $(shell dirname $@) || mkdir -p $(shell dirname $@) && cp -v $< $@


install: $(BINARY)
	@mkdir -p $(LIBDIR)
	@cp  $(BINARY) $(LIBDIR)
	
-include $(DEPFILES)


.PHONY: all clean clean-all install-headers  install

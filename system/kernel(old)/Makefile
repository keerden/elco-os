# list of non-source files to be part of distribution
AUXFILES := Makefile
# subdirs to scan for files
PROJDIRS := .

# name of executable
KERNEL_EXEC ?= elco-os.kernel

# name of linkerscript
LINKSCRIPT := linker.ld

#include config if not defined

ifndef CONFIG
	CONFIG ?= ../../make.config
	include $(CONFIG)
endif


# automatically scan for source files
CFILES := $(shell find $(PROJDIRS) -type f -name "*.c")
ASMFILES := $(shell find $(PROJDIRS) -type f -name "*.S")


# make a list of object files
COBJFILES := $(patsubst %.c,%.o,$(CFILES))
ASMOBJFILES := $(patsubst %.S,%.o,$(ASMFILES))

OBJFILES := $(COBJFILES) $(ASMOBJFILES)

# make a list of dependency files
CDEPFILES    := $(patsubst %.c,%.d,$(CFILES))
ASMDEPFILES    := $(patsubst %.S,%.d,$(ASMFILES))

DEPFILES := $(CDEPFILES) $(ASMDEPFILES)

ALLFILES := $(CFILES) $(ASMFILES) $(HDRFILES) $(AUXFILES)

# determine the systemroot dir to place the output
SYSROOT ?= ../../build/sysroot

# determine the boot dir to place the kernel
BOOTDIR ?= $(SYSROOT)/boot

# directory containing the source of the (new to compile) system-wide headerfiles 
NEWINCDIR ?= ../include

# directory containing the kernel header files
KERNEL_INCDIR = /elco-os/kernel

# location within sysroot that contains all system-wide header files that already exist
HEADERDIR ?= $(SYSROOT)/usr/include

HDRFILES := $(shell find $(NEWINCDIR)$(KERNEL_INCDIR) -type f -name "*.h")
INSTALLHDRFILES := $(patsubst $(NEWINCDIR)$(KERNEL_INCDIR)/%, $(HEADERDIR)$(KERNEL_INCDIR)/%,$(HDRFILES))



# enabled compiler warnings
WARNINGS := -Wall -Wextra 
#-pedantic -Wshadow -Wpointer-arith -Wcast-align \
           # -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
          #  -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
          #  -Wuninitialized -Wconversion -Wstrict-prototypes

#definitions
DEFS := -D__is_elco_os_kernel -D_KERNEL_DEBUG
#include paths
CINC := --sysroot=$(SYSROOT) -isystem $(HEADERDIR) -I $(NEWINCDIR)


CFLAGS :=  $(CFLAGS) $(CINC)   $(WARNINGS) $(DEFS) -O2 -g  -ffreestanding -fbuiltin -std=gnu11

#linker options
LDFLAGS ?= -Wl,-Map,kernel.map

# used libs
LIBS := -nostdlib -lk -lgcc

all: $(KERNEL_EXEC)

clean: 
	@rm -f $(OBJFILES) $(DEPFILES) *.o */*.o */*/*.o

clean-all: clean
	@rm -f $(KERNEL_EXEC)

$(KERNEL_EXEC) :  $(OBJFILES) $(LINKSCRIPT) Makefile
	$(CC) -T $(LINKSCRIPT) -o $@ $(CFLAGS) $(OBJFILES) $(LDFLAGS) $(LIBS)


%.o: %.c Makefile
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@


%.o: %.S Makefile
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@ 


install-headers: $(INSTALLHDRFILES)
	
$(INSTALLHDRFILES) : $(HDRFILES) 
	test -d $(shell dirname $@) || mkdir -p $(shell dirname $@) && cp -v $< $@

install: $(KERNEL_EXEC)
	@mkdir -p $(BOOTDIR)
	@cp  $(KERNEL_EXEC) $(BOOTDIR)

	
-include $(DEPFILES)


.PHONY: all clean install-headers  install









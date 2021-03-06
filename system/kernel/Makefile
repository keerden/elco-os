# list of non-source files to be part of distribution
AUXFILES := Makefile

# name of executable
KERNEL_EXEC ?= elco-os.kernel

# name of linkerscript
LINKSCRIPT := linker.ld

#include config if not defined

ifndef CONFIG
	CONFIG ?= ../../make.config
	include $(CONFIG)
endif

# determine the systemroot dir to place the output
SYSROOT ?= ../../build/sysroot

OBJDIR := ../../build/objects/system/kernel
SRCDIR := .


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


# automatically scan for source files
CFILES := $(shell find $(SRCDIR) -type f -name "*.c")
ASMFILES := $(shell find $(SRCDIR) -type f -name "*.S")

COBJFILES := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(CFILES:.c=.o))
ASMOBJFILES := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(ASMFILES:.S=.o))

OBJFILES := $(COBJFILES) $(ASMOBJFILES)
OBJDIRS := $(dir $(OBJFILES))

# make a list of dependency files
CDEPFILES := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(CFILES:.c=.d))
ASMDEPFILES  := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(ASMFILES:.S=.d))

DEPFILES := $(CDEPFILES) $(ASMDEPFILES)





# enabled compiler warnings
WARNINGS := -Wall -Wextra
WARNINGS += -Wpointer-arith
WARNINGS += -Wcast-align
WARNINGS += -Wwrite-strings
WARNINGS += -Wswitch-default
WARNINGS += -Wunreachable-code
WARNINGS += -Winit-self
WARNINGS += -Wmissing-field-initializers
WARNINGS += -Wno-unknown-pragmas
WARNINGS += -Wstrict-prototypes
WARNINGS += -Wundef
WARNINGS += -Wold-style-definition 
WARNINGS += -Wnested-externs
WARNINGS += -Winline
WARNINGS += -Wconversion  -Wstack-usage=8192

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
	@rm -rf $(OBJDIR)

$(KERNEL_EXEC) :  $(OBJFILES) $(LINKSCRIPT) Makefile
	$(CC) -T $(LINKSCRIPT) -o $@ $(CFLAGS) $(OBJFILES) $(LDFLAGS) $(LIBS)


# %.o: %.c Makefile
$(OBJDIR)/%.o: $(SRCDIR)/%.c Makefile
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@


#%.o: %.S Makefile
$(OBJDIR)/%.o: $(SRCDIR)/%.S Makefile
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@ 


install-headers: $(INSTALLHDRFILES)
	
$(INSTALLHDRFILES) :  $(HEADERDIR)$(KERNEL_INCDIR)/% : $(NEWINCDIR)$(KERNEL_INCDIR)/%
	@test -d $(shell dirname $@) || mkdir -p $(shell dirname $@) && cp -v $< $@

install: $(KERNEL_EXEC)
	@mkdir -p $(BOOTDIR)
	@cp  $(KERNEL_EXEC) $(BOOTDIR)

	
-include $(DEPFILES)


.PHONY: all clean install-headers  install 








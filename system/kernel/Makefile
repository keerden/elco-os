# list of non-source files to be part of distribution
AUXFILES := Makefile
# subdirs to scan for files
PROJDIRS := .

# name of executable
EXEC := elco-os.kernel

# name of linkerscript
LINKSCRIPT := linker.ld

# automatically scan for source files
CFILES := $(shell find $(PROJDIRS) -type f -name "*.c")
ASMFILES := $(shell find $(PROJDIRS) -type f -name "*.s")
HDRFILES := $(shell find $(PROJDIRS) -type f -name "*.h")

# make a list of object files
COBJFILES := $(patsubst %.c,%.o,$(CFILES))
ASMOBJFILES := $(patsubst %.s,%.o,$(ASMFILES))

OBJFILES := $(COBJFILES) $(ASMOBJFILES)

# make a list of dependency files
CDEPFILES    := $(patsubst %.c,%.d,$(CFILES))
ASMDEPFILES    := $(patsubst %.c,%.d,$(ASMFILES))

DEPFILES := $(CDEPFILES) $(ASMDEPFILES)

ALLFILES := $(CFILES) $(ASMFILES) $(HDRFILES) $(AUXFILES)

# determine the systemroot dir to place the output
SYSROOT ?= ../../build/sysroot

# determine the boot dir to place the kernel
BOOTDIR ?= $(SYSROOT)/boot

# directory containing the source of the system-wide headerfiles for the kernel
INCKERNELDIR := /kernel
INCLUDESRC ?=../include$(INCKERNELDIR)



# location within sysroot hat contains header files
HEADERDIR ?= $(SYSROOT)/include

# enabled compiler warnings
WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wuninitialized -Wconversion -Wstrict-prototypes

#definitions
DEFS := -D__is_elco_os_kernel -D_KERNEL_DEBUG

CFLAGS := -O2 -g  -ffreestanding -fbuiltin -std=gnu11 --sysroot=$(SYSROOT) $(WARNINGS) $(DEFS)

#linker options
LDFLAGS ?= -Wl,-Map,kernel.map

# used libs
LIBS := -nostdlib -lkernel

all: $(EXEC)

$(EXEC) :  $(OBJFILES) $(LINKSCRIPT)
	@$(CC) -T $(LINKSCRIPT) -o $@ $(CFLAGS) $(OBJFILES) $(LDFLAGS) $(LIBS)


%.o: %.c Makefile
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@


%.o: %.s Makefile
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@ 


install: install-headers install-kernel
	@echo $(HEADERDIR)$(INCKERNELDIR)

install-headers:
	@mkdir -p $(HEADERDIR)$(INCKERNELDIR)
	@cp -RTv $(INCLUDESRC) $(HEADERDIR)$(INCKERNELDIR)

install-kernel: $(EXEC)
	@mkdir -p $(BOOTDIR)
	@cp  $(EXEC) $(BOOTDIR)
	



.PHONY: all clean install-headers install-kernel intall

-include $(DEPFILES)







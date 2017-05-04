PROJDIRS := lib system

#include config 
CONFIG := make.config

# determine the systemroot dir to place the output
export SYSROOT := $(PWD)/build/sysroot
export ISODIR := $(PWD)/build/isodir
export ISOFILE := $(PWD)/build/elco-os.iso

export BOOTDIR := $(SYSROOT)/boot
export KERNEL_EXEC := elco-os.kernel

define GRUBCFG
	menuentry "elco-os" { 
		multiboot /boot/$(KERNEL_EXEC) 
	} 
endef
export GRUBCFG







all: install-headers install

clean:
	@for proj in $(PROJDIRS); do \
		$(MAKE) clean -C $$proj; \
	done

clean-all:
	@for proj in $(PROJDIRS); do \
		$(MAKE) clean-all -C $$proj; \
	done
	@rm -rfv $(SYSROOT)
	@rm -rfv $(ISODIR)
	@rm -f $(ISOFILE)


install-headers:
	@for proj in $(PROJDIRS); do \
		$(MAKE) install-headers -C $$proj; \
	done
install:  
	@for proj in $(PROJDIRS); do \
		$(MAKE) install -C $$proj; \
	done
iso: all
	@mkdir -p $(ISODIR)
	@mkdir -p $(ISODIR)/boot
	@mkdir -p $(ISODIR)/boot/grub

	@cp $(BOOTDIR)/$(KERNEL_EXEC) $(ISODIR)/boot/$(KERNEL_EXEC)

	@echo "$$GRUBCFG"  > $(ISODIR)/boot/grub/grub.cfg 

	@grub-mkrescue -o $(ISOFILE) $(ISODIR)

qemu: iso
	qemu-system-i386 -cdrom $(ISOFILE) -m 128M

bochs: iso
	bochs

.PHONY: all clean clean-all install-headers install iso qemu $(PROJDIRS)

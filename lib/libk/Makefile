
ifndef CONFIG
	CONFIG ?= ../../make.config
	include $(CONFIG)
endif


BUILDDIR := ./build
SRCDIR := ./src
TESTDIR := ./tests

LIBDIR := $(BUILDDIR)/lib
OBJDIR := $(BUILDDIR)/objects
DEBUGDIR := $(BUILDDIR)/debug
INCLDIR := $(SRCDIR)/include
UNITYDIR := $(TESTDIR)/unity

LIB := $(LIBDIR)/libk.a
DEBUGLIB := $(LIBDIR)/libkdebug.a
DEBUGLIB_CC := -lkdebug


# automatically scan for source files
CFILES := $(shell find $(SRCDIR) -type f -name "*.c")
ASMFILES := $(shell find $(SRCDIR) -type f -name "*.S")
HDRFILES := $(shell find $(INCLDIR) -type f -name "*.h")

INSTALLHDRFILES := $(patsubst $(INCLDIR)/%, $(SYSINCDIR)/%,$(HDRFILES))


# make a list of object files

COBJFILES := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(CFILES:.c=.o))
ASMOBJFILES := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(ASMFILES:.S=.o))

OBJFILES := $(COBJFILES) $(ASMOBJFILES)
OBJDIRS := $(dir $(OBJFILES))

# make a list of dependency files
CDEPFILES := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(CFILES:.c=.d))
ASMDEPFILES  := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(ASMFILES:.S=.o))

DEPFILES := $(CDEPFILES) $(ASMDEPFILES)

# files for testing
DEBUG_OBJFILES := $(patsubst $(OBJDIR)/%,$(DEBUGDIR)/%,$(OBJFILES))
DEBUG_OBJDIRS := $(patsubst $(OBJDIR)/%,$(DEBUGDIR)/%,$(OBJDIRS))
DEBUG_DEPFILES:= $(patsubst $(OBJDIR)/%,$(DEBUGDIR)/%,$(DEPFILES))

TESTFILES := $(shell find $(TESTDIR) -type f  -name "Test*.c"  ! -path "$(UNITYDIR)/*")
TESTAUXFILES := $(shell find $(TESTDIR) -type f  -name "*.c" ! -name "Test*"  ! -path "$(UNITYDIR)/*")
TESTHFILES := $(shell find $(TESTDIR) -type f  -name "*.h" ! -path "$(UNITYDIR)/*")

TEST_TARGETS := $(patsubst %.c,%.test,$(TESTFILES))

include compiler.mak

all: $(LIB)

clean: clean-src clean-debug 
	 
clean-src:
	@rm -f $(OBJFILES) $(DEPFILES)

clean-debug:
	@rm -f $(DEBUG_OBJFILES) $(DEBUG_DEPFILES)


clean-all: clean-all-src clean-all-debug clean-all-test

clean-all-src:
	@rm -rf $(OBJDIR) $(LIB)
clean-all-debug:
	@rm -rf $(DEBUGLIB) $(DEBUGDIR)
clean-all-test:
	@rm -f $(TEST_TARGETS)
	  

debug: $(DEBUGLIB)


$(LIB): $(OBJFILES)
	@$(AR) rcs $@ $(OBJFILES)

$(OBJDIR)/%.o: $(SRCDIR)/%.c Makefile
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.S Makefile
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@

### Generate a lib with debug support

$(DEBUGLIB): $(DEBUG_OBJFILES)
	@$(TESTAR) rcs $@ $(DEBUG_OBJFILES)

$(DEBUGDIR)/%.o: $(SRCDIR)/%.c Makefile
	@mkdir -p $(@D)
	@$(TESTCC) $(DEBUG_CFLAGS) -MD -MP -c $< -o $@

$(DEBUGDIR)/%.o: $(SRCDIR)/%.S Makefile
	@mkdir -p $(@D)
	@$(TESTCC) $(DEBUG_CFLAGS) -MD -MP -c $< -o $@

### testing

test: build-tests
	@ X=0;							\
	 for test in $(TEST_TARGETS); 	\
	 do 							\
	 	$$test;						\
		if [ $$? -ne 0 ];			\
			then X=1; 				\
			break;					\
		fi;							\
	done; 							\
	exit $$X						

test-all: build-tests
	@ X=0;							\
	 for test in $(TEST_TARGETS); 	\
	 do 							\
	 	$$test;						\
		if [ $$? -ne 0 ];			\
			then X=1; 				\
		fi;							\
	done; 							\
	exit $$X	

build-tests: debug $(TEST_TARGETS)

$(TESTDIR)/%.test: $(TESTDIR)/%.c $(TESTDIR)/%.runner.c $(TESTAUXFILES) $(TESTHFILES) $(DEBUGLIB)
	@$(TESTCC) $(TEST_CFLAGS) $(UNITYDIR)/src/unity.c $(TESTAUXFILES) $< $(word 2,$^)  $(DEBUGLIB_CC)  -o $@

$(TESTDIR)/%.runner.c: $(TESTDIR)/%.c $(TESTHFILES) 
	@ruby $(UNITYDIR)/auto/generate_test_runner.rb $< $@

install: $(LIB)
	@mkdir -p $(SYSLIBDIR)
	@cp  $(LIB) $(SYSLIBDIR)

install-headers: $(INSTALLHDRFILES)
	
$(INSTALLHDRFILES) :  $(SYSINCDIR)/% : $(INCLDIR)/%
	@test -d $(shell dirname $@) || mkdir -p $(shell dirname $@) && cp -v $< $@


# automatically scan for source files

-include $(DEPFILES)
-include $(DEBUG_DEPFILES)


.PHONY: all debug test test-all clean clean-src clean-debug clean-all clean-all-tests  clean-all-debug clean-all-src  build-tests install install-headers

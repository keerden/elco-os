TESTAR:=ar
TESTAS:=as
TESTCC:=gcc




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
WARNINGS += -Wconversion 

INC_DIRS= -I$(INCLDIR)   -L $(LIBDIR)

CFLAGS :=  $(INC_DIRS)   $(WARNINGS) -Wredundant-decls -O2 -g  -ffreestanding -fbuiltin -std=gnu11
DEBUG_CFLAGS := $(CFLAGS)  -m32 -D__IS_DEBUG -D__IS_TESTING
TEST_CFLAGS :=  -iquote $(SRCDIR) $(INC_DIRS) -I$(UNITYDIR)/src  $(WARNINGS)  -O2 -g -m32  -fbuiltin -std=gnu11

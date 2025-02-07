# to generate assembler listing with LTO, add to LDFLAGS: -Wa,-adhln=$@.listing,--listing-rhs-width=200
# for better annotations add -dA -dP
# to generate assembler source with LTO, add to LDFLAGS: -save-temps=cwd

ifdef OS
	WINDOWS = 1
	SHELL = cmd.exe
endif

subdirs := $(wildcard */)
VPATH = $(subdirs)
cpp_sources := $(wildcard *.cpp) $(wildcard $(addsuffix *.cpp,$(subdirs)))
cpp_objects := $(addprefix obj/,$(patsubst %.cpp,%.o,$(notdir $(cpp_sources))))
c_sources := $(wildcard *.c) $(wildcard $(addsuffix *.c,$(subdirs)))
c_objects := $(addprefix obj/,$(patsubst %.c,%.o,$(notdir $(c_sources))))
# s_sources := support/gcc8_a_support.s support/depacker_doynax.s
s_objects := $(addprefix obj/,$(patsubst %.s,%.o,$(notdir $(s_sources))))
vasm_sources := $(wildcard *.asm) $(wildcard $(addsuffix *.asm, $(subdirs)))
vasm_objects := $(addprefix obj/, $(patsubst %.asm,%.o,$(notdir $(vasm_sources))))
objects := $(cpp_objects) $(c_objects) $(s_objects) $(vasm_objects)
# This is the place to declare the static libraries
static_libs := -lc -lm -ldebug -lnet -lunix -lc -lm -lamiga # to use Amiga paths
#static_libs := -lm -ldebug -lnet -lunix -lc -lm -lamiga # to use UNIX paths

# https://stackoverflow.com/questions/4036191/sources-from-subdirectories-in-makefile/4038459
# http://www.microhowto.info/howto/automatically_generate_makefile_dependencies.html

program = out/a
OUT = $(program)
CC = m68k-amiga-elf-gcc
VASM = vasmm68k_mot

ifdef WINDOWS
	SDKDIR = $(abspath $(dir $(shell where $(CC)))..\m68k-amiga-elf\sys-include)
	MUISDKDIR = $(abspath $(dir $(shell where $(CC)))..\m68k-amiga-elf\sys-include\SDK\MUI\C\include)
else
	SDKDIR = $(abspath $(dir $(shell which $(CC)))../m68k-amiga-elf/sys-include)
	MUISDKDIR = $(abspath $(dir $(shell which $(CC)))../m68k-amiga-elf/sys-include/SDK/MUI/C/include)
endif

# The directory where the static libraries are located
LIBSDIR = $(abspath libs)

CCFLAGS = -g -MP -MMD -m68000 -Ofast -nostdlib -Wextra -Wno-unused-function -Wno-volatile-register-var -fomit-frame-pointer -fno-tree-loop-distribution -flto -fwhole-program -fno-exceptions -I${MUISDKDIR} -I$(LIBSDIR)/library -I$(LIBSDIR)/library/include -D__stdargs=
CPPFLAGS= $(CCFLAGS) -fno-rtti -fcoroutines -fno-use-cxa-atexit
ASFLAGS = -Wa,-g,--register-prefix-optional,-I$(SDKDIR),-D
LDFLAGS = -Wl,--emit-relocs,-Ttext=0,-Map=$(OUT).map -L${LIBSDIR}
VASMFLAGS = -m68000 -Felf -opt-fconst -nowarn=62 -dwarf=3 -quiet -x -I. -I$(SDKDIR)

all: $(OUT).exe

$(OUT).exe: $(OUT).elf
	$(info Elf2Hunk $(program).exe)
	@elf2hunk $(OUT).elf $(OUT).exe -s

$(OUT).elf: $(objects)
	$(info Linking $(program).elf)
	@$(CC) $(CCFLAGS) $(LDFLAGS) $(objects) $(static_libs) -o $@
	@m68k-amiga-elf-objdump --disassemble --no-show-raw-ins --visualize-jumps -S $@ >$(OUT).s

clean:
	$(info Cleaning...)
ifdef WINDOWS
	@del /q obj\* out\*
else
	@$(RM) obj/* out/*
endif

-include $(objects:.o=.d)

$(cpp_objects) : obj/%.o : %.cpp
	$(info Compiling $<)
	@$(CC) $(CPPFLAGS) -c -o $@ $(CURDIR)/$<

$(c_objects) : obj/%.o : %.c
	$(info Compiling $<)
	@$(CC) $(CCFLAGS) -c -o $@ $(CURDIR)/$<

$(s_objects): obj/%.o : %.s
	$(info Assembling $<)
	@$(CC) $(CCFLAGS) $(ASFLAGS) -c -o $@ $(CURDIR)/$<

$(vasm_objects): obj/%.o : %.asm
	$(info Assembling $<)
	@$(VASM) $(VASMFLAGS) -o $@ $(CURDIR)/$<

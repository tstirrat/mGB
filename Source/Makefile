# If you move this project you can change the directory
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
ifndef GBDK_HOME
  GBDK_HOME = ../gbdk
endif

PROJECTNAME = mGB

MAKEFLAGS += --no-builtin-rules

LCC = $(GBDK_HOME)/bin/lcc

LCCFLAGS += -msm83:gb # GB/GBC
# LCCFLAGS += -v # verbose
# LCCFLAGS += -autobank

ifdef DEBUG
LCCFLAGS += -debug -v
endif

ifdef RELEASE
LCCFLAGS += -Wf--max-allocs-per-node50000 # A better speed optimization than --opt-code-speed
else
LCCFLAGS += -Wf--opt-code-speed # Optimize for code speed rather than size
endif

# sdcc settings (-Wf and -Wp)
# LCCFLAGS += -Wf--legacy-banking # Use legacy method to call banked functions

# sdasgb (asm) settings (-Wa)
# LCCFLAGS += -Wa-l # generate asm listing
# LCCFLAGS += -Wa-a # All symbols are global
# LCCFLAGS += -Wa-s # Create symbol file/outfile[.sym]
# LCCFLAGS += -Wa-j # Enable NoICE Debug Symbols
# LCCFLAGS += -Wa-y # Enable SDCC Debug Symbols
# LCCFLAGS += -Wa-g # Undefined symbols are global
# LCCFLAGS += -Wa-n # Don't resolve global assigned value symbols
# LCCFLAGS += -Wa-z # Disable case sensitivity for symbols

# sdldgb (linker) settings (-Wl)
# LCCFLAGS += -Wl-m # Map output generated as (out)file[.map]
# LCCFLAGS += -Wl-j # NoICE Debug output as (out)file[.noi]
# LCCFLAGS += -Wl-w # Wide listing format for map file

# makebin settings (-Wm)
# LCCFLAGS += -Wm-yS # Convert .noi file named like input file to .sym
LCCFLAGS += -Wm-yc # GBC compatible
LCCFLAGS += -Wm-yn"$(PROJECTNAME)" # cartridge name
LCCFLAGS += -Wm-yt3 # MBC type = ROM+MBC1+RAM+BATT
LCCFLAGS += -Wm-ya1 # number of ram banks: -ya 1

LCCFLAGS += -Wf-MMD

OBJDIR      = obj
OUTDIR      = out


BIN	        = $(OUTDIR)/$(PROJECTNAME).gb

MKDIRS      = $(OUTDIR) $(OBJDIR)/io $(OBJDIR)/screen $(OBJDIR)/synth $(OBJDIR)/io/sram

C_SOURCES    = $(wildcard *.c)
C_SOURCES    += $(wildcard **/*.c)

SRAM_BANK0  = $(wildcard io/sram/*.b0.c)

ASM_SOURCES  = $(wildcard **/*.s)


OBJS        = $(C_SOURCES:%.c=$(OBJDIR)/%.o)
OBJS        += $(ASM_SOURCES:%.s=$(OBJDIR)/%.o)
OBJS        += $(SRAM_BANK0:%.b0.c=$(OBJDIR)/%.b0.o)


# Builds all targets sequentially
all: $(BIN)

# Dependencies - for better incremental builds
DEPS = $(OBJS:%.o=%.d)
-include $(DEPS)

# sram banks
$(OBJDIR)/%.b0.o: %.b0.c
	$(LCC) $(LCCFLAGS) -Wf-ba0 -c -o $@ $<

# Compile .c files to .o object files
$(OBJDIR)/%.o: %.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .s assembly files to .o object files
$(OBJDIR)/%.o: %.s
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Link the compiled object files into a .gb ROM file
$(BIN):	makedirs $(OBJS)
	$(LCC) $(LCCFLAGS) -o $(BIN) $(OBJS)

.PHONY: makedirs
makedirs:
	mkdir -p $(MKDIRS)

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(OUTDIR)

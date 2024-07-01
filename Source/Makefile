# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
ifndef GBDK_HOME
	GBDK_HOME = ../../gbdk-4.3.0/
endif

LCC = $(GBDK_HOME)bin/lcc 

LCCFLAGS += -msm83:gb # GB/GBC
# LCCFLAGS += -Wa-l # generate asm listing
# LCCFLAGS += -Wl-m # generate linker map
# LCCFLAGS += -Wm-yS # generate bgb symbols # was -Wl-j  # generate .noi output
# LCCFLAGS += -Wm-yt0x03 # MBC type: -yt 3 = ROM+MBC1+RAM+BATT 
# LCCFLAGS += -Wm-ya1 # MBC Banks: -ya 1
# LCCFLAGS += -autobank

# GBDK_DEBUG = ON
ifdef GBDK_DEBUG
	LCCFLAGS += -debug -v
endif


# You can set the name of the .gb ROM file here
PROJECTNAME    = mGB
# LCCFLAGS += -yn "$(PROJECTNAME)"

BINS	         = $(PROJECTNAME).gb
CSOURCES      := $(wildcard *.c)
ASMSOURCES    := $(wildcard *.s)

all:	$(BINS)

compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | sed s/mkdir\ \-p/mkdir/ | grep -v make >> compile.bat

# Compile and link all source files in a single call to LCC
$(BINS):	$(CSOURCES) $(ASMSOURCES)
	$(LCC) $(LCCFLAGS) -o $@ $(CSOURCES) $(ASMSOURCES)

clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm *.noi *.rst
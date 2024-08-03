MYDIR	= .
BLDDIR	= $(MYDIR)/build
FW	= $(MYDIR)/framework
CC	= ../gbdk/bin/lcc -tempdir=$(BLDDIR) -Wl-j -Wl-m -Wl-w -Wl-yt2 -Wl-yo4 -Wl-ya4

TESTS    		 = $(wildcard *.c)
OBJS     		 = $(TESTS:%.c=$(BLDDIR)/%.o)
TESTROMS     = $(TESTS:%.c=$(BLDDIR)/%.gb)

BGBDIR = ../bgb
# GBDKDIR = ../gbdk

all: clean mkdirs $(BGBDIR) build-all

.PHONY: clean
clean:
	rm -rf $(BLDDIR)

.PHONY: mkdirs
mkdirs:
	mkdir -p $(BLDDIR)

$(BGBDIR):
	wget https://bgb.bircd.org/bgb.zip
	unzip -jo -qq bgb.zip  -d ../bgb
	rm bgb.zip

# gbdk-linux64.tar.gz:
# 	wget https://github.com/gbdk-2020/gbdk-2020/releases/download/4.3.0/gbdk-linux64.tar.gz

# $(GBDKDIR):
# 	tar -zxvf gbdk-linux64.tar.gz -C $(GBDKDIR)
# 	# rm gbdk-linux64.tar.gz

$(BLDDIR)/%.gb: $(MYDIR)/%.c mkdirs $(BGBDIR)
	$(CC) -o $@ $<

.PHONY: build-all
build-all: $(TESTROMS)

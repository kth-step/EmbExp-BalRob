# local settings
-include Makefile.local

# toolchain
include Makefile.toolchain


# common definitions
# ---------------------------------
OUTDIR  = output
NAME	= ${OUTDIR}/balrob.elf

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))


# file definitions
# ---------------------------------
CODE_DIRS     = embexp-balrob CMSIS_CORE_LPC11xx

SOURCES_C     = $(foreach d,$(CODE_DIRS),$(call rwildcard, $d/src/, *.c))
SOURCES_S     = $(foreach d,$(CODE_DIRS),$(call rwildcard, $d/src/, *.S))
INCLUDE_FILES = $(foreach d,$(CODE_DIRS),$(call rwildcard, $d/inc/, *.h))

OBJECTS       = $(SOURCES_C:.c=.o) $(SOURCES_S:.S=.o)

LINKERFILE    = lpc11c24_flash.ld


# compiler flags
# ---------------------------------
SFLAGS_EXTRA = -mcpu=cortex-m0 -mthumb
CFLAGS_EXTRA = -g3 -specs=nosys.specs -DUSE_OLD_STYLE_DATA_BSS_INIT -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -fno-common -D__USE_CMSIS=CMSIS_CORE_LPC11xx
LDFLAGS_POST = -L$(ARMSYS) -L$(ARMLIB) -lgcc

INCFLAGS     = $(foreach d,$(CODE_DIRS),-I$d/inc)
SFLAGS       = ${SFLAGS_EXTRA} ${INCFLAGS}
CFLAGS	     = -std=gnu99 -Wall -fno-builtin -fno-stack-protector ${INCFLAGS} ${CFLAGS_EXTRA} -fdump-rtl-expand
# -DGCC_COMPAT_COMPILATION
LDFLAGS_PRE  = -Bstatic -nostartfiles -nostdlib


# compilation and linking
# ---------------------------------
all: $(NAME)

%.o: %.S ${INCLUDE_FILES}
	${CROSS}cpp ${INCFLAGS} $< | ${CROSS}as ${SFLAGS} -o $@ -

%.o: %.c ${INCLUDE_FILES}
	${CROSS}gcc ${CFLAGS} -c -o $@ $<

$(NAME): ${OBJECTS} ${INCLUDE_FILES}
	mkdir -p ${OUTDIR}
	${CROSS}ld $(LDFLAGS_PRE) -o $@ -T $(LINKERFILE) ${OBJECTS} $(LDFLAGS_POST)
	${CROSS}objdump -t -h $@ > "$@.table"
	${CROSS}objdump -D    $@ > "$@.da"

clean:
	rm -rf ${OUTDIR}
	rm -f $(call rwildcard, , *.o)
	rm -f $(call rwildcard, , *.expand)


.PHONY: all clean


# callgraphs
# http://www.gson.org/egypt/
callgraph-install:
	rm -f egypt-1.10.tar.gz
	rm -rf egypt-1.10
	wget http://www.gson.org/egypt/download/egypt-1.10.tar.gz
	tar -xzf egypt-1.10.tar.gz
	cd egypt-1.10 && perl Makefile.PL && make && chmod +x egypt

# http://m0agx.eu/2016/12/15/making-call-graphs-with-gcc-egypt-and-cflow/
EXPANSION      = ./embexp-balrob/src/*.expand ./embexp-balrob/src/dev/*.expand
EXPANSION_SIMP = ./embexp-balrob/src/*.expand
callgraph:
	make clean
	make callgraph-full

callgraph-full: $(NAME)
	rm -f output/callgraph.png
	./egypt-1.10/egypt --include-external ${EXPANSION} | dot -Gsize=3000,3000 -Grankdir=LR -Tpng -o output/callgraph.png
	xdg-open output/callgraph.png

callgraph-simp: $(NAME)
	rm -f output/callgraph.png
	./egypt-1.10/egypt --include-external ${EXPANSION_SIMP} | dot -Gsize=3000,3000 -Grankdir=LR -Tpng -o output/callgraph.png
	xdg-open output/callgraph.png

.PHONY: callgraph callgraph-install

# running and debugging
include Makefile.run


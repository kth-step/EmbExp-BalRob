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
CFLAGS	     = -std=gnu99 -Wall -fno-builtin -fno-stack-protector ${INCFLAGS} ${CFLAGS_EXTRA} -DPUREPRINT -DDONTKNOWMATH
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
	${CROSS}objdump -t -h -D $@ > "$@_da"

clean:
	rm -rf ${OUTDIR}
	rm -f $(call rwildcard, , *.o)


.PHONY: all clean


# running and debugging
include Makefile.run


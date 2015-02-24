# 1: boot from SD
# 2: boot from NOR (XIP)
BOOT ?= 1

APP = dra7xx-hello
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
CFLAGS = -Wall -Iinclude -DCONFIG_BOOT=$(BOOT)			\
	-nostdlib -nostartfiles -ffreestanding			\
	-g -Os -fno-schedule-insns -fno-schedule-insns2
OBJS =	src/start.o			\
	src/main.o			\
	src/board.o			\
	src/common.o			\
	src/serial.o

MKIMAGE ?= mkimage

ifeq (${BOOT}, 1)
TEXT_BASE := $(shell build/get-param.sh SRAM_BASE include/config.h 1)
LDS = ld/dra7xx.lds
LDS_GEN = ld/dra7xx-gen.lds
else
LDS = ld/dra7xx-nor.lds
LDS_GEN = ld/dra7xx-nor-gen.lds
endif

default: $(APP).bin

MLO: $(APP).bin
	@if [ $(BOOT) -ne 1 ]; then					\
		echo "** Error: You don't need MLO for XIP boot";	\
		exit 1;							\
	fi
	$(MKIMAGE) -T omapimage -a $(TEXT_BASE) -d $< $@

$(APP).bin: $(OBJS) $(LDS_GEN)
	$(LD) $(OBJS) -T $(LDS_GEN) -o $(APP).elf
	$(OBJDUMP) -D $(APP).elf >$(APP).list
	$(OBJCOPY) $(APP).elf -O binary $(APP).bin

%.o: %.S
	$(CC) $(CFLAGS) -c $*.S -o $*.o

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

$(LDS_GEN): $(LDS)
	build/gen-lds.sh $(LDS) $(LDS_GEN) $(CFLAGS)

clean:
	rm -f src/*.o $(APP).elf $(APP).bin $(APP).list MLO ld/*-gen.lds

.PHONY: default clean

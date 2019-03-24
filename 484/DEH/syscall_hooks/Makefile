CC			:= ppu-gcc
CXX			:= ppu-g++
LD			:= ppu-ld 
OBJCOPY		:= ppu-objcopy
OBJDUMP		:= ppu-objdump
AR 			:= ppu-ar
STRIP		:= ppu-strip
syscall_hooks = syscall_hooks.bin
INCLUDE		=  ../lv2/include ../lv1/include

INCLUDE		+= $(PS3DEV)/ppu/ppu/include
LIBSDIR		=
LIBS 		=
TARGET		= syscall_hooks


CFLAGS = -m64 -fno-builtin -fno-exceptions \
			   -Wall $(foreach dir,$(INCLUDE),-I$(dir)) \
			   -DLV2 -DFIRMWARE_4_84DEH -ffreestanding
CFLAGS += --std=gnu99 -fgnu89-inline -Os -g
#-ffunction-sections -fdata-sections 

LDFLAGS= -T $(TARGET).ld -nostartfiles -nostdlib -nodefaultlibs -mbig-endian -Wl,-static -L. $(foreach dir,$(LIBSDIR),-L$(dir)) $(LIBS) -Wl,-g

OBJS = start.o main.o utilities.o sc_info.o sc_handler.o sc_writer.o sc_pool.o sys_hid.o ../lv2/src/usb.o ../lv2/src/patch.o ../lv2/src/interrupt.o ../lv2/src/interrupt_c.o ../lv2/src/io.o ../lv2/src/libc.o \
	../lv2/src/libc_c.o ../lv2/src/memory.o ../lv2/src/memory_c.o ../lv2/src/thread.o ../lv2/src/thread_c.o ../lv2/src/process.o \
	../lv2/src/synchronization.o ../lv2/src/modules.o ../lv2/src/modules_c.o ../lv2/src/storage.o ../lv2/src/object.o \
	../lv2/src/security.o ../lv2/src/time.o ../lv2/src/hid.o ../lv2/src/pad.o ../lv2/src/syscall.o ../lv1/src/hvcall.o

all: $(TARGET).bin asm

asm: $(TARGET).lst

%.lst: %.elf
	$(OBJDUMP) -S -D $< > $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@	
	@rm -f *.o ../lv1/src/*.o

$(TARGET).elf: $(OBJS) $(TARGET).ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.elf *.self *.lst *.bin *.482c *.map ../lv1/src/*.o ../debug/src/*.o ../lv2/src/*.o ../lv1/src/*.o
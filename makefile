GCC=riscv32-unknown-elf-gcc
OBJDUMP=riscv32-unknown-elf-objdump
MARCH=rv32i
MABI=ilp32
LD_SCRIPT=sykom.ld

CFLAGS=-O0
LDFLAGS=-ffreestanding -Wl,--gc-sections 
LDFLAGS+=-nostartfiles -nostdlib -nodefaultlibs -Wl,-T,$(LD_SCRIPT)
LDFLAGS+=-march=$(MARCH) -mabi=$(MABI)

all: sykom

sykom: crt0.s main.c sykom.dts
	$(GCC) -g $(CFLAGS) $(LDFLAGS) crt0.s main.c -o sykom
	$(OBJDUMP) -DxS sykom > sykom.lst

sykom.dts: 
	qemu-system-riscv32 -machine sykt -bios none -machine dumpdtb=sykom.dtb
	dtc -I dtb -O dts -o sykom.dts sykom.dtb

clean: 
	rm -f sykom
	rm -f sykom.lst
	rm -f sykom.dts 
	rm -f sykom.dtb

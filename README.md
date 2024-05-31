# RISC-V SoC Prime Seeker

## Description
System on Chip SoC based on the RISC-V architecture with custom peripherals for finding the n-th prime number.
The development process includes emulation through QEMU, building a custom Linux distribution with Buildroot, 
and developing Linux kernel modules for hardware interaction.



## Test application


![app.png](images%2Fapp.png)

## Verilog module

The gpioemu module is a hardware description in Verilog that finds the n-th prime number based on the value written to a specific register.
It contains a set of inputs and outputs with the specification required by GPIO. 
The module defines local parameters that determine the basic address space and specific register addresses for the argument ADDR_A,
status ADDR_S, and result ADDR_W. The registers used in the module include registers for storing the current prime number [current_number] the number of prime numbers found [prime_count], 
and the final result W. The module uses a state machine that manages the computation states,
having three states: IDLE, COMPUTE, and DONE. Prime number calculations are handled in the COMPUTE state using a loop that checks if the number is prime.

## Compilation

For project purposes, a set of virtual machines running under the Linux Debian system was prepared.
These computers have the necessary tools for the RISC-V processor in the RV32I version,
the appropriate header files, and the subsystem for building the Linux distribution. 

### _makeQemuGpioEmu_
    _makeQemuGpioEmu_ gpioemu.v

_makeQemuGpioEmu_ script converts a Verilog hardware module into a QEMU-compatible executable by compiling it through Verilator, creating necessary libraries, and linking them into the final QEMU system executable.

![makeQemuGpioEmu.png](images%2FmakeQemuGpioEmu.png)

### _make_busybox_compile_
    _make_busybox_compile_ main.c

_make_busybox_compile_ script compiles the given main.c file, integrates it into the root filesystem, and prepares the necessary boot files (fw_jump.elf, Image, rootfs.ext2) for the emulated environment.

![make_busybox_compile.png](images%2Fmake_busybox_compile.png)


### New QEMU instance for custom RISC-V system
    ./qemu-system-riscv32-sykt -M sykt -nographic \
    -bios fw_jump.elf \
    -kernel Image \
    -append "root=/dev/vda ro" \
    -drive file=rootfs.ext2,format=raw,id=hd0 \
    -device virtio-blk-device,drive=hd0 \
    -netdev user,id=net0 -device virtio-net-device,netdev=net0 ; reset

This command launches a QEMU instance for a custom RISC-V system, booting with specified firmware, kernel, and root filesystem, while configuring virtual block and network devices in a non-graphical mode.

### modprobe kernel_module
    modprobe kernel_module
Loading the specified kernel module into the Linux kernel


## Test Application
     ./main

The testing program waits for the user to input a number for which calculations 
will be performed, or to exit the application by clicking "x". 
The application implements value restrictions as specified: the input must be in the range
of 1 to 1000, otherwise, the program will ignore the input and wait for the next one.

![app.png](images%2Fapp.png)
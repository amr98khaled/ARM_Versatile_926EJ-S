#the order of the objects didn't matter (tried different orders)
#notice we didn't include uart.o nor string.o in objects because they are included in t.c as uart.c and string.c so if we compile them here and then compile t.c they will be compiled again which causes an error so we compile only t.c which in turn compiles uart.c and string.c implicitly and you can make sure by running make ss commnd to show symbols in t.elf and you will find functions from both files in the final object file .
objects = ts.o t.o 


%.o: %.s
	arm-none-eabi-as -o $@ $<
	
%.s: %.c
	arm-none-eabi-gcc -S -mcpu=arm926ej-s $<
	
%.o: %.c
	arm-none-eabi-gcc -c $<
	
t.elf: t.ld $(objects)
	arm-none-eabi-ld -T $< -o $@ $(objects)  #link ts.o to t.elf file
	
ss: t.elf
	arm-none-eabi-nm $<		#show symbols
	
delf: t.elf
	arm-none-eabi-readelf -a $<  # display all information of t.elf

daelf: t.elf
	arm-none-eabi-objdump -d $@  # disassemble t.elf file
	
t.bin: t.elf	
	arm-none-eabi-objcopy -O binary $< $@ # objcopy t.elf to t.bin
	
run: t.bin
	qemu-system-arm -M versatilepb -m 128M -kernel $< -nographic -serial mon:stdio


.PHONY: clean
clean :
	rm -rf  *.o *.elf *.bin 

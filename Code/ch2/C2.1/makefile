objects = ts.o

%.o: %.s
	arm-none-eabi-as -o $@ $<
	
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
	qemu-system-arm -M versatilepb -kernel $< -nographic -serial /dev/null


.PHONY: clean
clean :
	rm -rf  *.o *.elf *.bin 

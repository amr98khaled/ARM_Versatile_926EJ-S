#the order of the objects didn't matter (tried different orders)
#notice we didn't include uart.o nor vid.c in objects because they are included in t.c as uart.c and vid.c so if we compile them here and then compile t.c they will be compiled again which causes an error so we compile only t.c which in turn compiles uart.c and vid.c implicitly and you can make sure by running make ss commnd to show symbols in t.elf and you will find functions from both files in the final object file .
objects = ts.o t.o 


%.o: %.s
	arm-none-eabi-as -mcpu=arm926ej-s -g $< -o $@
	
%.s: %.c
	arm-none-eabi-gcc -S -mcpu=arm926ej-s $<
	
%.o: %.c
	arm-none-eabi-gcc -c -mcpu=arm926ej-s -g $< -o $@
	
t.elf: t.ld $(objects)
	arm-none-eabi-ld -T $< $(objects) -o $@  #link ts.o to t.elf file
	
sself: t.elf
	arm-none-eabi-nm $<		#show symbols for elf file
	
sspanda: panda.o
	nm -S -t d $<			#show symbols for panda.o
	
ssporkcar: porkcar.o
	nm -S -t d $<			#show symbols for porkcar.o
	
delf: t.elf
	arm-none-eabi-readelf -a $<  # display all information of t.elf

daelf: t.elf
	arm-none-eabi-objdump -d $@  # disassemble t.elf file
	
t.bin: t.elf	
	arm-none-eabi-objcopy -O binary $< $@ # objcopy t.elf to t.bin

%.o: %.bmp
	arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm $< $@		#convert bmp images to .o files
	
run: t.bin
	qemu-system-arm -M versatilepb -m 128M -kernel $< -serial mon:stdio


.PHONY: clean
clean :
	rm -rf  *.o *.elf *.bin 

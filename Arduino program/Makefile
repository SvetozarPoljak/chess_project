main.hex: main.elf
	avr-objcopy -O ihex main.elf main.hex

main.elf: main.o usart.o
	avr-gcc -mmcu=atmega328p main.o usart.o -o main.elf

main.o: main.c usart.h
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o main.o main.c

usart.o: usart.c usart.h
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o usart.o usart.c

program: main.hex
	avrdude -c arduino -p ATMEGA328P -P COM3 -b 115200 -U flash:w:main.hex

clean:
	rm *.o *.elf *.hex
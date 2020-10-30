CC = avr-gcc
MCU = atmega324a
CFLAGS = -std=gnu99 -Wall -pedantic -mmcu=$(MCU)
.PHONY = clean
.DEFAULT_OPTION = main

main: washingMachine.o
	avr-objcopy -j .text -j .data -O ihex washingMachine.o washingMachine.hex


washingMachine.o: washingMachine.c washingMachine.h
	$(CC) $(CFLAGS) washingMachine.c -o washingMachine.o

flash: main
	avrdude -p $(MCU) -c stk500 -U flash:w:washingMachine.hex:i -F -P /dev/ttyACM0


clean:
	rm *.o

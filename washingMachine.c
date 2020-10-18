#include <stdio.h>
#include <unistd.h>
#include <avr/io.h>
#include "washingMachine.h"

int main(int argc, char** argv) {
    DDRA = 0xFF;
    DDRB = (1 << PB3);
    DDRC = (1 << PC0 | 1 << PC1 | 1 << PC2);

    OCR0A = 255;

    // Set timer counter to toggle OC0B on compare match
    // and set wave form generation mode to CTC 
    TCCR0A = 1 << WGM01 | 0 << WGM00 | 0 << COM0A1 | 1 << COM0A0;
    TCCR0B = 0 << WGM02 | 0 << CS02 | 1 << CS01 | 1 << CS00;
    
    int cc = 0;
    while (1) {
	cc ^= 1;
	if (cc) {
	    PORTA = waterLevels[ERROR_WATER];
	} else {
	    PORTA = waterLevels[MEDIUM_WATER];
	}
        // delay to prevent ghosting
	for (int i = 0; i < 1000; i++);
	PORTA = 0;
	PORTC = cc;
    }
    return 0;
}

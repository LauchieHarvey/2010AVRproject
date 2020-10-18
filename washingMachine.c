#include <stdio.h>
#include <unistd.h>
#include <avr/io.h>
#include "washingMachine.h"

// This will only display one digit each iteration. The next iteration
// it will display the other.
void set_segment_display(int* cc) {
    *cc ^= 1; // Toggle the digit that gets displayed this iteration.

    // When CC == 1, it will display the water level on the right digit.
    if (*cc) {
	int selectedWaterLevel = get_water_level();
	PORTA = waterLevelsSeg[selectedWaterLevel];

    } else {
	PORTA = modesSeg[0];
    }
    // delay to prevent ghosting
    for (int i = 0; i < 1000; i++);
    PORTA = 0;
    PORTC = (*cc << PC0); 
}

// The value of S0 and S1 as an integer between 0 and 3.
// The value represents the index of the water level that
// has been selected.
int get_water_level() {
    uint8_t waterLevel = (PINC & (1 << PC2 | 1 << PC1)) >> 1;
    return waterLevel;
}

int main(int argc, char** argv) {
    // Port A is output for Seven Seg Display values.
    DDRA = 0xFF;
    DDRB = (1 << PB3);
    DDRC = (0 << PC2 | 0 << PC1 | 1 << PC0);

    OCR0A = 255;
    // Set timer counter to toggle OC0B on compare match
    // and set wave form generation mode to CTC 
    TCCR0A = (1 << WGM01 | 0 << WGM00 | 0 << COM0A1 | 1 << COM0A0);
    TCCR0B = (0 << WGM02 | 0 << CS02 | 1 << CS01 | 1 << CS00);
    
    int cc = 0;
    while (1) {
	set_segment_display(&cc);

    }
    return 0;
}
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
	PORTA = waterLevelsSeg[get_water_level()];

    } else {
	PORTA = modesSeg[get_mode()];
    }
    // delay to prevent ghosting
    for (int i = 0; i < 1000; i++);
    PORTA = 0;
    PORTC = (*cc << PC0); 
}

// Returns the value of S0 and S1 as an integer between 0 and 3.
// The value represents the index of the water level that has been selected. 
// Needs to be shifted right one bit to compensate for position in PORTC.
uint8_t get_water_level() {
    return (PINC & (1 << PC2 | 1 << PC1)) >> PC1;
}

// Returns the value of S3 as an integer, either 0 or 1. This is the index of
// the operation mode that has been selected. Needs to be shifted right by 3
// bits to compensate for its position in PORTC.
uint8_t get_mode() {
    return (PINC & (1 << PC3)) >> PC3;
}

// Sets the relevant pins to input or output depending on their purpose.
void configure_pins() {
    // Port A is output for Seven Seg Display values.
    DDRA = 0xFF;

    // Port C:  PC1 && PC2 are switches 0 and 1 that control water level.
    //		PC3 is switch 2 which controls operation mode.	 
    //		PC0 is the output that determines which ssd digit will be
    //		    displayed this iteration. (left or right)
    DDRC = (0 << PC3 | 0 << PC2 | 0 << PC1 | 1 << PC0);
}

int main(int argc, char** argv) {

    configure_pins();
   
    int cc = 0;
    while (1) {
	set_segment_display(&cc);

    }
    return 0;
}

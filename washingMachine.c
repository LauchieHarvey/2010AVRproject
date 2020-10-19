#include <stdio.h>
#include <unistd.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "washingMachine.h"

// CONTROL VARIABLES (GLOABAL VOLATILE) ======================================

volatile uint8_t sevenSegCC = 0;

volatile uint16_t timeCount = 0;

volatile uint8_t ledArrayVal = 0;

volatile bool machineStarted = false;

// 0 == normal, 1 == extended. Normal by default
volatile uint8_t machineMode = 0;


// This will only display one digit each iteration. The next iteration
// it will display the other.
void set_segment_display() {
    sevenSegCC ^= 1; // Toggle the digit that gets displayed this iteration.

    // When CC == 1, it will display the water level on the right digit.
    if (sevenSegCC) {
	PORTA = waterLevelsSeg[get_water_level()];
    } else {
	PORTA = modesSeg[get_mode()];
    }
    // delay to prevent ghosting
    for (int i = 0; i < 1000; i++);
    PORTA = 0;
    PORTC = (sevenSegCC << PC0); 
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
    //          PC6 is for the button interrupt signal.
    DDRC = (1 << PC6 | 0 << PC3 | 0 << PC2 | 0 << PC1 | 1 << PC0);

    // Port D: PD0 through PD4 are outputs for the LEDS on the IO board.
    DDRB = (1 << PB4 | 1 << PB3 | 1 << PB2 | 1 << PB1 | 1 << PB0);

    DDRD = (0 << PD2);
}

// Makes LEDs run from right to left. Takes a parameter to determine the
// direction to increment in.
void update_led_pattern(bool runLeft) { 
    PORTB = (1 << ledArrayVal);
    // Skip every second time count to make it more visible.
    if (timeCount % 2 == 0) {	
	if (runLeft) {
	    ledArrayVal = (ledArrayVal >= 3) ? 0 : ledArrayVal + 1;	
	} else {
	    ledArrayVal = (ledArrayVal <= 0) ? 3 : ledArrayVal - 1;
	}
    }
}

// Runs LEDs in right and left directions.
void update_led_pattern_rinse() {
    if (timeCount % 6 < 3) {
	update_led_pattern(true);   
    } else {
	update_led_pattern(false);
    }
}

// Set up timer/counter 1 so that we get an 
// interrupt 100 times per second, i.e. every
// 10 milliseconds.
void configure_timer() {
    OCR1A = 9999; // Clock divided by 8 - count for 10000 cycles
    TCCR1A = 0; // CTC mode
    TCCR1B = (1 << WGM12) | (1 << CS11); // Divide clock by 8

    // Enable interrupt on timer on output compare match 
    TIMSK1 = (1 << OCIE1A); 

    // Ensure interrupt flag is cleared
    TIFR1 = (1 << OCF1A); 
	
    // Set up interrupt to occur on rising edge of pin PC6 (start/stop button)
    EICRA = (1 << ISC01) | (1 << ISC00);
    EIMSK = (1 << INT0);
    EIFR = (1 << INTF0);

    // Turn on global interrupts
    sei(); 
}

// Converts the time count to seconds.
uint8_t count_to_seconds(uint16_t count) {
    return count / 100;
}

int main(int argc, char** argv) {

    configure_pins();
    configure_timer();
   
    while (1); 
    return 0;
}

// Event handler for the "start" button (B0 on IO board)
ISR(INT0_vect) {
    machineStarted = true;
    machineMode = get_mode();
}

// The timer triggers an interrupt every 10 milliseconds.
ISR(TIMER1_COMPA_vect) { 
    set_segment_display();
    if (!machineStarted) {
	return; 
    }
    // WASH cycle
    if (count_to_seconds(timeCount) < 3) {
	update_led_pattern(true);
    } else if (count_to_seconds(timeCount) >= 3) {
	PORTB = 0;
    } 
    // RINSE cycle
    if (count_to_seconds(timeCount) < 6) {
	update_led_pattern_rinse();
    } else if (count_to_seconds(timeCount) < 9) {
	// blink for 3 seconds	
	PORTB = (timeCount % 20 < 10) ? 0xFF : 0;
    } else {
	PORTB = 0;
    }
    
    ++timeCount; 
}

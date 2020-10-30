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

// 0 == normal, 1 == extended, 2 == finished. Normal by default
volatile uint8_t machineMode = 0;


// This will only display one digit each iteration. The next iteration
// it will display the other.
void set_segment_display() {
    sevenSegCC ^= 1; // Toggle the digit that gets displayed this iteration. 

    // When CC == 1, it will display the water level on the right digit.
    if (machineMode == CYCLES_FINISHED_MODE) {
	PORTA |= CYCLES_FINISHED_SEG_VAL;
	
    } else if (sevenSegCC) {
	PORTA |= waterLevelsSeg[get_water_level()];
    } else {
	PORTA |= modesSeg[get_mode()];
    }
    // delay to prevent ghosting
    for (uint8_t i = 0; i < 100; ++i);
    PORTA = (sevenSegCC << PA7);
}

// Returns the value of S0 and S1 as an integer between 0 and 3.
// The value represents the index of the water level that has been selected. 
// Needs to be shifted right one bit to compensate for position in PORTC.
uint8_t get_water_level() {   
    return (PIND & (1 << PD1 | 1 << PD0));
}

// Returns the value of S3 as an integer, either 0 or 1. This is the index of
// the operation mode that has been selected. 0 == Normal mode, 1 == Extended 
uint8_t get_mode() {
    return (PIND & (1 << PD4)) >> PD4;
}

// Sets the relevant pins to input or output depending on their purpose.
void configure_pins() {
    // Port A is output for Seven Seg Display values.
    DDRA = 0xFF; 

    // PC6 is for the button interrupt signal.
    DDRC = (1 << PC6);

    // Port B: outputs for the LEDS on the IO board.
    DDRB = (1 << PB4 | 1 << PB3 | 1 << PB2 | 1 << PB1 | 1 << PB0);

    // Port D: Switch output pins and Buttons
    DDRD = (0 << PD4 | 0 << PD3 | 0 << PD2 | 0 << PD1 | 0 << PD0);
}

// Makes LEDs run from right to left. Takes a parameter to determine the
// direction to increment in.
void update_led_pattern(bool runLeft) { 
    PORTB = (1 << ledArrayVal);

    if (runLeft) {
        ledArrayVal = (ledArrayVal >= 3) ? 0 : ledArrayVal + 1;	
    } else {
        ledArrayVal = (ledArrayVal <= 0) ? 3 : ledArrayVal - 1;
    }
}

// Runs LEDs in right and left directions (So it bounces back and forth).
void update_led_pattern_spin() {
    if ((timeCount % 6) < 3) {
	update_led_pattern(true);   
    } else {
	update_led_pattern(false);
    }
}

// Return the width of a pulse (in clock cycles) given a duty cycle (%) and
// the period of the clock (measured in clock cycles)
uint16_t duty_cycle_to_pulse_width(float dutycycle) {
	return (dutycycle / 100) * PWM_CLOCK_PERIOD;
}

// Set up timer/counter 1 so that we get an 
// interrupt 12.5 times per second, i.e. every
// 80 milliseconds.
void configure_timer1() {
     // Clock divided by 64 - count for 10000 cycles
    OCR1A = PWM_CLOCK_PERIOD;

    TCCR1A = 0;
    TCCR1B = (1 << WGM12 | 1 << CS11 | 1 << CS10);

    // Enable interrupt on timer on output compare match 
    TIMSK1 = (1 << OCIE1A); 

    // Ensure interrupt flag is cleared
    TIFR1 = (1 << OCF1A); 
	
    // Set up interrupt to occur on falling edge of pin PC6 (start/stop button)
    EICRA = (1 << ISC11 | 0 << ISC10 | 1 << ISC01 | 0 << ISC00);
    EIMSK = (1 << INT1 | 1 << INT0);
    EIFR = (1 << INTF1 | 1 << INTF0);

    // Turn on global interrupts
    sei(); 
}

void configure_timer0() {
    // Set it to 255 so that it is off to start with.
    OCR0B = 255;

    TCCR0A = (0 << COM0A1 | 0 << COM0A0 | 1 << COM0B1 | 1 << COM0B0 | 1 << WGM01 | 1 << WGM00);
    TCCR0B = (0 << WGM02 | 0 << CS02 | 0 << CS01 | 1 << CS00);
}

// Converts the time count to seconds. count / (counts / second) = seconds
uint8_t count_to_seconds(uint16_t count) {
    return count / 12.5;
}

int main(int argc, char** argv) {

    configure_pins();
    configure_timer1();
    configure_timer0();
   
    while (1) {
        set_segment_display();    
    } 
    return 0;
}

// WASH CYCLE handles pwm and LED changes
void wash_cycle() {

    OCR0B = duty_cycle_to_pulse_width(WASH_DUTY_CYCLE) - 1;
    update_led_pattern(true);
}

// First pattern of the rinse cycle, running LEDS to the right. PWM change. 
void rinse_cycle_main() {

    OCR0B = duty_cycle_to_pulse_width(RINSE_DUTY_CYCLE) - 1;
    update_led_pattern(false);	
}

// blink for 3 seconds, occurs after the right running LED pattern.	
void rinse_cycle_blink() {

    PORTB = (timeCount % DELAY_CONSTANT < DELAY_CONSTANT / 4) ? 0x0F : 0;
}

// SPIN CYCLE
void spin_cycle() {
    uint8_t timeInSec = count_to_seconds(timeCount);
    if (timeInSec == 18 || timeInSec == 15) {
        // Clear the LED bits and then set the first bit to 1.
        int mask = (1 << PB0 | 1 << PB1 | 1 << PB2 | 1 << PB3);
        PORTB &= ~mask;
        PORTB |= (1 << PB0);
    }

    OCR0B = duty_cycle_to_pulse_width(SPIN_DUTY_CYCLE) - 1;
    update_led_pattern_spin();
}

// Function that looks at the time of the program to determine which cycle
// should be runnning currently. Returns a pointer to the function that 
// handles the cycle.
cycleFn determine_cycle() {
    if (count_to_seconds(timeCount) < 3) {
        return wash_cycle;
    } 
    if ((count_to_seconds(timeCount) < 12 && count_to_seconds(timeCount) > 9 &&
	    machineMode == EXTENDED_MODE) || count_to_seconds(timeCount) < 6) {
        return rinse_cycle_main;

    } else if ((count_to_seconds(timeCount) < 15 && machineMode == EXTENDED_MODE) ||
	    count_to_seconds(timeCount) < 9) {
        return rinse_cycle_blink;
    }
    if ((count_to_seconds(timeCount) < 18 && machineMode == EXTENDED_MODE) ||
	(count_to_seconds(timeCount) < 15 && machineMode == NORMAL_MODE)) {
        return spin_cycle;
    }

    // All cycles are over.
    return NULL;
}

// Event handler for the "start" button (B0 on IO board)
ISR(INT0_vect) {
    if (!machineStarted || machineMode == CYCLES_FINISHED_MODE) {	
	machineStarted = true;
	machineMode = get_mode();
	timeCount = 0;
    }
}

// Event handler for the "reset" button (B1 on IO board)
ISR(INT1_vect) {
    PORTB = 0;
    OCR0B = 255;
    machineStarted = false;
    machineMode = get_mode();
    timeCount = 0;
}

// The timer triggers this interrupt every 80 milliseconds.
ISR(TIMER1_COMPA_vect) { 
    if (!machineStarted || machineMode == CYCLES_FINISHED_MODE) {
	return; 
    }

    cycleFn currentCycle = determine_cycle();
    if (currentCycle) {
        currentCycle(); 
        ++timeCount;
        return;
    }
    
    // If it reaches this point then all of the cycles have finished.
    PORTB = 0;
    OCR0B = 255;
    machineMode = CYCLES_FINISHED_MODE;
    machineStarted = false;
    ++timeCount; 
}

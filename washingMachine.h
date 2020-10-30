#ifndef WASHING_MACHINE_H
#define WASHING_MACHINE_H

// WATER LEVEL DEFINITIONS ===================================================
#define LOW_WATER_SEG_VAL 8
#define MEDIUM_WATER_SEG_VAL 64
#define HIGH_WATER_SEG_VAL 1
#define ERROR_WATER_SEG_VAL 121


int waterLevelsSeg[5] = {LOW_WATER_SEG_VAL, MEDIUM_WATER_SEG_VAL,
    HIGH_WATER_SEG_VAL, ERROR_WATER_SEG_VAL};

// OPERATIONAL MODE DEFINITIONS ==============================================

#define NORMAL_MODE_SEG_VAL 84
#define EXTENDED_MODE_SEG_VAL 121

#define NORMAL_MODE 0
#define EXTENDED_MODE 1
#define CYCLES_FINISHED_MODE 2

int modesSeg[3] = {NORMAL_MODE_SEG_VAL, EXTENDED_MODE_SEG_VAL};

#define WASH_DUTY_CYCLE 10
#define RINSE_DUTY_CYCLE 50
#define SPIN_DUTY_CYCLE 90

// MISC DEFINITIONs ==========================================================

#define DELAY_CONSTANT 15

#define CYCLES_FINISHED_SEG_VAL 63


// FUNCTION DEFINITIONS ======================================================

uint8_t get_water_level();

uint8_t get_mode();

void set_segment_display();

void configure_pins();

uint8_t count_to_seconds(uint16_t count);

#endif

#ifndef WASHING_MACHINE_H
#define WASHING_MACHINE_H

// WATER LEVEL DEFINITIONS ===================================================
#define LOW_WATER_SEG_VAL 8
#define MEDIUM_WATER_SEG_VAL 64
#define HIGH_WATER_SEG_VAL 1
#define ERROR_WATER_SEG_VAL 121

int waterLevelsSeg[4] = {LOW_WATER_SEG_VAL, MEDIUM_WATER_SEG_VAL,
    HIGH_WATER_SEG_VAL, ERROR_WATER_SEG_VAL};

// OPERATIONAL MODE DEFINITIONS ==============================================

#define NORMAL_MODE_SEG_VAL 84
#define EXTENDED_MODE_SEG_VAL 121

int modesSeg[2] = {NORMAL_MODE_SEG_VAL, EXTENDED_MODE_SEG_VAL};

enum Cycles {
    WASH,
    RINSE,
    RINSE2,
    SPIN
} cycles;

// FUNCTION DEFINITIONS ======================================================

uint8_t get_water_level();

uint8_t get_mode();

void set_segment_display();

void configure_pins();

#endif

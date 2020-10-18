#ifndef WASHING_MACHINE_H
#define WASHING_MACHINE_H


// WATER LEVEL DEFINITIONS ===================================================
#define LOW_WATER_DIGIT 8
#define LOW_WATER 0
#define MEDIUM_WATER_DIGIT 64
#define MEDIUM_WATER 1
#define HIGH_WATER_DIGIT 1
#define HIGH_WATER 2
#define ERROR_WATER_DIGIT 121
#define ERROR_WATER 3

int waterLevels[4] = {LOW_WATER_DIGIT, MEDIUM_WATER_DIGIT,
    HIGH_WATER_DIGIT, ERROR_WATER_DIGIT};

// OPERATIONAL MODE DEFINITIONS

#define NORMAL_MODE_DIGIT 84
#define NORMAL_MODE 0
#define EXTENDED_MODE_DIGIT 121
#define EXTENDED_MODE 1

int modes[2] = {NORMAL_MODE_DIGIT, EXTENDED_MODE_DIGIT};

#endif

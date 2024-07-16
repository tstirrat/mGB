#pragma once

#include <gbdk/platform.h>
#include <stdbool.h>

extern uint8_t noiFreq[72];
extern uint8_t noiEnv;
extern uint8_t noiMode;
extern bool noiSus;

extern bool noiNoteOffTrigger;
extern int8_t noiOct;

void updateNoi(void);
void setPitchBendFrequencyOffsetNoise(void);

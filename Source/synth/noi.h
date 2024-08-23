#pragma once

#include "common.h"
#include <gbdk/platform.h>
#include <stdbool.h>

#define MAX_NOI_FREQ 72

extern uint8_t noiFreq[72];
extern synth_state noiState;

void updateNoi(void);
void setPitchBendFrequencyOffsetNoise(void);

#pragma once

#include "common.h"
#include <gbdk/platform.h>
#include <stdbool.h>

extern synth_state pu1State;

extern synth_state pu2State;

void updatePu1(void);
void playNotePu1(void);

void updatePu2(void);
void playNotePu2(void);

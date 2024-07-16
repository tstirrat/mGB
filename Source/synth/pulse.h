#pragma once

#include <gbdk/platform.h>
#include <stdbool.h>

typedef struct pulse_state {
  uint8_t envelope;
  bool sus;
  uint8_t retrig;
  uint8_t velocity;
  bool noteOffTrigger;
  int8_t octave;
} pulse_state;

extern pulse_state pu1State;

extern pulse_state pu2State;

void updatePu1(void);
void playNotePu1(void);

void updatePu2(void);
void playNotePu2(void);

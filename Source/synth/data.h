#pragma once

#include <gbdk/platform.h>
#include <stdbool.h>

typedef enum Parameter {
  PU1_Transpose = 0,
  PU1_Shape = 1,
  PU1_Envelope = 2,
  PU1_Sweep = 3,
  PU1_PBRange = 4,
  PU1_Sustain = 5,
  PU1_Pan = 6,

  PU2_Transpose = 7,
  PU2_Shape = 8,
  PU2_Envelope = 9,
  PU2_PBRange = 10,
  PU2_Sustain = 11,
  PU2_Pan = 12,

  WAV_Transpose = 13,
  WAV_Shape = 14,
  WAV_Offset = 15,
  WAV_Sweep = 16,
  WAV_PBRange = 17,
  WAV_Sustain = 18,
  WAV_Pan = 19,

  NOI_Transpose = 20,
  NOI_Env = 21,
  NOI_Sustain = 22,
  NOI_Pan = 23,

  PU1_Slot = 24,
  PU2_Slot = 25,
  WAV_Slot = 26,
  NOI_Slot = 27,
} Parameter;

extern uint8_t dataSet[28];

extern uint8_t dataSetSnap[28];

extern bool parameterLock[24];

void updateValueSynth(Parameter p);

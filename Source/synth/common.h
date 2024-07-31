#pragma once

#include <gbdk/platform.h>
#include <stdbool.h>

#define PU1 0
#define PU2 1
#define WAV 2
#define NOI 3

#define PBWHEEL_CENTER 0x80

extern const uint16_t freq[72];

typedef struct note_status_t {
  bool active;
  uint8_t note;
} note_status_t;

extern note_status_t noteStatus[4];

// temp holder of current freq data, can be for any synth
extern uint16_t currentFreq;

extern uint16_t currentFreqData[4];

// Pitch wheel
extern bool pbWheelActive[4];
extern uint8_t pbWheelIn[4];
extern uint8_t pbWheelInLast[4];
extern uint8_t pbRange[4];

typedef struct pb_note_range {
  uint8_t low;
  uint8_t high;
} pb_note_range;

extern pb_note_range pbNoteRange[4];

// Vibrato
extern uint8_t vibratoPosition[4];
extern uint8_t vibratoSpeed[4];
extern uint8_t vibratoDepth[4];
extern bool vibratoSlope[4];
extern uint8_t vibratoTimer[4];

// Pan
extern uint8_t outputSwitch[4];
extern uint8_t outputSwitchValue[4];

void updateSynths(void);
void setOutputPanBySynth(uint8_t synth, uint8_t value);
void setOutputPan(uint8_t synth, uint8_t value);
void setPitchBendFrequencyOffset(uint8_t synth);
void stopAllSynths(void);

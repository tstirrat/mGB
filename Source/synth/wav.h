#pragma once

#include <gbdk/platform.h>
#include <stdbool.h>

// NR32_REG (0xFF1C) volume ctrl:
//                   0b-VV-----
#define AUD3LEVEL_25 0b01100000
#define AUD3LEVEL_50 0b01000000
#define AUD3LEVEL_100 0b0100000

// The mask which represents the Chan 3 output level
#define AUD3LEVEL_MASK 0b01100000

// The WAV oscilator is 2 octaves higher
#define WAV_OCTAVE_OFFSET 24U

extern uint16_t wavCurrentFreq;

extern int8_t wavOct;
extern bool wavNoteOffTrigger;
extern bool wavSus;

// current wav offset
extern uint8_t wavDataOffset;

// previous wav offset
extern uint8_t wavShapeLast;

// manual sweep
extern uint8_t wavSweepSpeed;
extern uint16_t counterWav;
extern bool cueWavSweep;
extern uint8_t wavStepCounter;
extern uint8_t counterWavStart;

// initial waveforms
extern const uint8_t wavData[256];

void updateWav(void);
void loadWav(uint8_t offset);
void playNoteWav(void);
void updateWavSweep(void);

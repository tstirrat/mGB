#pragma once

#include <gbdk/platform.h>

#define MIDI_STATUS_BIT 0x80
#define MIDI_STATUS_NOTE_ON 0x09
#define MIDI_STATUS_NOTE_OFF 0x08
#define MIDI_STATUS_AT 0x0A
#define MIDI_STATUS_CC 0x0B
#define MIDI_STATUS_PC 0x0C
#define MIDI_STATUS_AT_MONO 0x0D
#define MIDI_STATUS_PB 0x0E

#define MIDI_STATUS_SYSTEM 0xF0

// MIDI status
extern uint8_t statusByte;
// MIDI address (e.g. note/CC control)
extern uint8_t addressByte;
// MIDI value (e.g. velocity/value)
extern uint8_t valueByte;
extern uint8_t capturedAddress;

void updateMidiBuffer(void);

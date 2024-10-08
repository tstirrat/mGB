#include "midi.h"
#include "../mGB.h"
#include "midi_asm.h"
#include "serial.h"

uint8_t statusByte;
uint8_t addressByte;
uint8_t valueByte;
uint8_t capturedAddress;

void updateMidiBuffer(void) {
  if (serialBufferReadPosition == serialBufferPosition) {
    return;
  }

  serialBufferReadPosition++; // unsigned overflow from 255 -> 0 is automatic.

  uint8_t byte = serialBuffer[serialBufferReadPosition];

  // STATUS BYTE
  if (byte & MIDI_STATUS_BIT) {
    if ((byte & MIDI_STATUS_SYSTEM) == MIDI_STATUS_SYSTEM) {
      return;
    }
    statusByte = byte;
    capturedAddress = false;
    systemIdle = false;
    return;
  }

  // 2ND BYTE (note/CC control)
  if (!capturedAddress) {
    capturedAddress = true;
    addressByte = byte;
    systemIdle = false;
    return;
  }

  // 3RD BYTE (velocity/value)
  capturedAddress = false;
  valueByte = byte;
  systemIdle = false;

  switch ((statusByte >> 4) & 0x0F) {
  case MIDI_STATUS_PB:
    asmEventMidiPB();
    break;
  case MIDI_STATUS_CC:
    asmEventMidiCC();
    break;
  case MIDI_STATUS_NOTE_ON:
    asmEventMidiNote();
    break;
  case MIDI_STATUS_NOTE_OFF:
    asmEventMidiNoteOff();
    break;
  case MIDI_STATUS_PC:
    asmEventMidiPC();
    break;
  }
}

#include <gbdk/emu_debug.h>

#include "../Source/io/midi.h"

#include "../Source/io/serial.c"
#include "../Source/mGB.h"
#include "../Source/synth/common.c"
#include "../Source/synth/pulse.c"
#include "../Source/synth/wav.c"

bool systemIdle = true;

uint8_t statusByte;
uint8_t addressByte;
uint8_t valueByte;
uint8_t capturedAddress;

uint8_t result[2] = {0U, 0U};

void main(void) {
  rAUDENA = AUDENA_ON;
  rAUDVOL = AUDVOL_VOL_LEFT(7U) | AUDVOL_VOL_RIGHT(7U);

  setOutputPan(WAV, 64U);

  wavDataOffset = 1U * 16U;
  loadWav(1U * 16U);
  rAUD3LEVEL = 0x00U; // mimics mGB

  addressByte = 64U; // MIDI note
  valueByte = 127U;  // MIDI velocity

  playNoteWav();
  updateWav();
  // updateWavSweep();

  delay(100);

  result[0] = wavCurrentFreq;
  result[1] = wavCurrentFreq >> 8U;

  EMU_BREAKPOINT;
}

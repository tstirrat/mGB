#include <gbdk/emu_debug.h>
#include <gbdk/platform.h>

#include "../Source/io/midi.h"

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

  setOutputPan(PU1, 64U);

  addressByte = 64U; // MIDI note
  valueByte = 127U;  // MIDI velocity

  playNotePu1();
  updatePu1();

  delay(100);

  EMU_BREAKPOINT;
}

#include "poly.h"
#include "../io/midi.h"
#include "pulse.h"
#include "wav.h"

uint8_t polyVoiceSelect;
uint8_t polyNoteState[3];

void playNotePoly(void) {
  uint8_t note = addressByte;
  uint8_t velocity = valueByte;

  if (velocity == 0) {
    // Note off
    if (polyNoteState[0] == note) {
      playNotePu1();
    }
    if (polyNoteState[1] == note) {
      playNotePu2();
    }
    if (polyNoteState[2] == note) {
      playNoteWav();
    }
  } else {
    // Note on
    polyVoiceSelect++;
    if (polyVoiceSelect >= 3) {
      polyVoiceSelect = 0;
    }

    switch (polyVoiceSelect) {
    case 0:
      polyNoteState[0] = note;
      playNotePu1();
      break;
    case 1:
      polyNoteState[1] = note;
      playNotePu2();
      break;
    case 2:
      polyNoteState[2] = note;
      playNoteWav();
      break;
    }
  }
}

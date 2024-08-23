#include "pulse.h"
#include "../io/midi.h"
#include "common.h"

synth_state pu1State;

synth_state pu2State;

inline void updatePulse(uint8_t synth, synth_state *state, uint8_t *rLOW,
                        uint8_t *rHIGH, uint8_t *rENV) {
  if (state->noteOffTrigger) {
    *rENV = 0x00;
    state->noteOffTrigger = false;
    return;
  }

  if (pbWheelIn[synth] != PBWHEEL_CENTER) {
    if (pbWheelIn[synth] != pbWheelInLast[synth]) {
      pbWheelInLast[synth] = pbWheelIn[synth];
      pbWheelActive[synth] = true;
      setPitchBendFrequencyOffset(synth);
    } else {
      pbWheelActive[synth] = true;
    }
    return;
  }

  if (pbWheelActive[synth]) {
    pbWheelActive[synth] = false;
    pbWheelInLast[synth] = PBWHEEL_CENTER;

    const uint16_t f = currentFreqData[synth] = freq[noteStatus[synth].note];

    *rLOW = f;
    *rHIGH = f >> 8U;
  }
}

inline void playNotePulse(uint8_t synth, synth_state *state, uint8_t *rLOW,
                          uint8_t *rHIGH, uint8_t *rENV) {
  const uint8_t noteIndex = addressByte + state->octave;

  if (noteIndex >= MAX_FREQ) {
    return;
  }

  // Note off
  if (!valueByte) {
    if (noteStatus[synth].note == noteIndex) {
      noteStatus[synth].active = false;

      if (!state->sus) {
        state->noteOffTrigger = true;
      }
    }
    return;
  }

  uint8_t retrig = 0x00;

  // Note on
  if (*rENV == 0x00 || valueByte != state->velocity || state->noteOffTrigger) {
    retrig = AUDHIGH_RESTART;
    state->velocity = valueByte;
    *rENV = ((valueByte << 1) & 0xF0) | state->envelope;
  }

  const uint16_t f = currentFreqData[synth] = freq[noteIndex];

  if (f == 0) {
    return;
  }

  noteStatus[synth].note = noteIndex;

  *rLOW = f;
  *rHIGH = (f >> 8U) | retrig;

  vibratoPosition[synth] = 0;
  pbWheelInLast[synth] = PBWHEEL_CENTER;

  pbNoteRange[synth].low = noteIndex - pbRange[synth];
  pbNoteRange[synth].high = noteIndex + pbRange[synth];

  noteStatus[synth].active = true;
  state->noteOffTrigger = false;
}

// _asmUpdatePu1 was: 483/463 clock cycles, 100 bytes
// now: 428/407 clock cycles, 90 bytes
// = -55 cycles, 12.8% better
void updatePu1(void) {
  updatePulse(PU1, &pu1State, &rAUD1LOW, &rAUD1HIGH, &rAUD1ENV);
}

// _asmPlayNotePu1 was: 806/776 clock cycles, 180 bytes
// now: 822/802 clock cycles, 182 bytes
// = +22 cycles, 1.9% worse
void playNotePu1(void) {
  playNotePulse(PU1, &pu1State, &rAUD1LOW, &rAUD1HIGH, &rAUD1ENV);
}

// _asmUpdatePu2 was: 483/463 clock cycles, 100 bytes
// now: 431/410 clock cycles, 91 bytes
// = -52 cycles, 12.0% better
void updatePu2(void) {
  updatePulse(PU2, &pu2State, &rAUD2LOW, &rAUD2HIGH, &rAUD2ENV);
}

// _asmPlayNotePu2 was: 792/772 clock cycles, 181 bytes
// now: 822/802 clock cycles, 182 bytes
// = +30 cycles, 3.7% worse
void playNotePu2(void) {
  playNotePulse(PU2, &pu2State, &rAUD2LOW, &rAUD2HIGH, &rAUD2ENV);
}

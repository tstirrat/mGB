#include "common.h"
#include "../io/midi.h"
#include "../mGB.h"
#include "pulse.h"
#include "wav.h"

// The pulse channels’ period dividers are clocked at 1048576 Hz, once per four
// dots, and their waveform is 8 samples long. This makes their sample rate
// equal to `1048576 / (2048 - period_value)` Hz.
//
// with a resulting tone frequency equal to `131072 / (2048 - period_value)` Hz.

// MIDI note to channel's "period value" lookup.
//
// 0 = Notes too low or high to fit in 2048 sample window and would produce an
// incorrect note freq
const uint16_t freq[128] = {
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,

    44,   156,  262,  363,  457,  547,  631,  710,  786,  854,  923,  986,
    1046, 1102, 1155, 1205, 1253, 1297, 1339, 1379, 1417, 1452, 1486, 1517,
    1546, 1575, 1602, 1627, 1650, 1673, 1694, 1714, 1732, 1750, 1767, 1783,
    1798, 1812, 1825, 1837, 1849, 1860, 1871, 1881, 1890, 1899, 1907, 1915,
    1923, 1930, 1936, 1943, 1949, 1954, 1959, 1964, 1969, 1974, 1978, 1982,
    1985, 1988, 1992, 1995, 1998, 2001, 2004, 2006, 2009, 2011, 2013, 2015,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0};

note_status_t noteStatus[4];

uint16_t currentFreq;

uint16_t currentFreqData[4];

bool pbWheelActive[4] = {0x00, 0x00, 0x00, 0x00};
uint8_t pbWheelIn[4] = {PBWHEEL_CENTER, PBWHEEL_CENTER, PBWHEEL_CENTER,
                        PBWHEEL_CENTER};
uint8_t pbWheelInLast[4] = {PBWHEEL_CENTER, PBWHEEL_CENTER, PBWHEEL_CENTER,
                            PBWHEEL_CENTER};
uint8_t pbRange[4] = {2, 2, 2, 12};

pb_note_range pbNoteRange[4];

// Vibrato
uint8_t vibratoPosition[4] = {0, 0, 0, 0};
uint8_t vibratoSpeed[4] = {1, 1, 1, 1};
uint8_t vibratoDepth[4] = {0, 0, 0, 0};
bool vibratoSlope[4] = {0, 0, 0, 0};
uint8_t vibratoTimer[4];

// Pan
uint8_t outputSwitch[4] = {0x01, 0x02, 0x40, 0x80};
uint8_t outputSwitchValue[4] = {3, 3, 3, 3};

void setPitchBendFrequencyOffset(uint8_t synth) {
  uint16_t freqRange;
  uint16_t f = freq[noteStatus[synth].note];
  systemIdle = false;
  if (pbWheelIn[synth] & PBWHEEL_CENTER) {
    freqRange = freq[pbNoteRange[synth].high];
    currentFreq = (uint16_t)(pbWheelIn[synth] - 0x7F);
    currentFreq <<= 6;
    currentFreq /= 128;
    currentFreq = currentFreq * (freqRange - f);
    currentFreq = f + (currentFreq >> 6);
  } else {
    freqRange = freq[pbNoteRange[synth].low];
    currentFreq = (uint16_t)(PBWHEEL_CENTER - pbWheelIn[synth]);
    currentFreq <<= 6;
    currentFreq /= 128;
    currentFreq = currentFreq * (f - freqRange);
    currentFreq = f - (currentFreq >> 6);
  }
  switch (synth) {
  case PU1:
    rAUD1HIGH = (currentFreq >> 8U);
    rAUD1LOW = currentFreq;
    currentFreqData[synth] = currentFreq;
    break;
  case PU2:
    rAUD2HIGH = (currentFreq >> 8U);
    rAUD2LOW = currentFreq;
    currentFreqData[synth] = currentFreq;
    break;
  default:
    rAUD3HIGH = (currentFreq >> 8U);
    rAUD3LOW = currentFreq;
    currentFreqData[synth] = currentFreq;
  }
}

void addVibrato(uint8_t synth) {
  if (vibratoDepth[synth]) {
    currentFreq = currentFreqData[synth] + vibratoPosition[synth];
    pbWheelInLast[synth] = PBWHEEL_CENTER;
    if (synth == PU1) {
      rAUD1HIGH = (currentFreq >> 8U);
      rAUD1LOW = currentFreq;
    } else if (synth == PU2) {
      rAUD2HIGH = (currentFreq >> 8U);
      rAUD2LOW = currentFreq;
    } else if (synth == WAV) {
      rAUD3HIGH = (currentFreq >> 8U);
      rAUD3LOW = currentFreq;
    } else {
      rAUD4POLY = currentFreq;
    }
  }
}

void updateVibratoPosition(uint8_t synth) {
  if (vibratoTimer[synth] == vibratoSpeed[synth]) {
    vibratoTimer[synth] = 0x00;
    if (vibratoSlope[synth] && vibratoPosition[synth] < vibratoDepth[synth]) {
      vibratoPosition[synth] += 1;
    } else {
      vibratoSlope[synth] = 0;
      if (vibratoPosition[synth]) {
        vibratoPosition[synth] -= 1;
      } else {
        vibratoSlope[synth] = 1;
      }
    }
    addVibrato(synth);
  }
  vibratoTimer[synth]++;
}

void updateSynths(void) {
  if (vibratoDepth[PU1])
    updateVibratoPosition(PU1);
  if (vibratoDepth[PU2])
    updateVibratoPosition(PU2);
  if (vibratoDepth[WAV])
    updateVibratoPosition(WAV);
  if (vibratoDepth[NOI])
    updateVibratoPosition(NOI);

  updateWavSweep();
}

inline void setOutputSwitch(void) {
  rAUDTERM =
      outputSwitch[0] + outputSwitch[1] + outputSwitch[2] + outputSwitch[3];
}

void setOutputPanBySynth(uint8_t synth, uint8_t value) {
  outputSwitchValue[synth] = value;

  if (value == 3U) {
    value = 0x11 << synth;
  } else if (value == 2U) {
    value = 0x10 << synth;
  } else if (value == 1U) {
    value = 0x01 << synth;
  } else {
    value = 0x00;
  }
  outputSwitch[synth] = value;
  setOutputSwitch();
}

void setOutputPan(uint8_t synth, uint8_t value) {
  if (value > 96U) {
    value = 0x10U << synth;
    outputSwitchValue[synth] = 2;
  } else if (value > 32U) {
    value = 0x11U << synth;
    outputSwitchValue[synth] = 3;
  } else {
    value = 0x01U << synth;
    outputSwitchValue[synth] = 1;
  }
  outputSwitch[synth] = value;
  setOutputSwitch();
}

void stopAllSynths(void) {
  rAUD1ENV = 0;
  rAUD2ENV = 0;
  rAUD3LEVEL = 0;
  rAUD4ENV = 0;
  pbWheelIn[PU1] = pbWheelIn[PU2] = pbWheelIn[WAV] = pbWheelIn[NOI] =
      PBWHEEL_CENTER;
  pu1State.sus = pu2State.sus = wavSus = false;
}

#include "wav.h"
#include "../io/midi.h"
#include "../mGB.h"
#include "common.h"
#include <gbdk/platform.h>
#include <string.h>

uint16_t wavCurrentFreq;

int8_t wavOct;
bool wavNoteOffTrigger;
bool wavSus;

// current wav offset
uint8_t wavDataOffset;
// previous wav offset
uint8_t wavShapeLast;

// manual sweep
uint8_t wavSweepSpeed;
uint16_t counterWav;
bool cueWavSweep;
uint8_t wavStepCounter;
uint8_t counterWavStart;
// initial waveforms
const uint8_t wavData[256] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                              0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,

                              0x22, 0x55, 0x77, 0xAA, 0xBB, 0xDD, 0xEE, 0xFF,
                              0xEE, 0xDD, 0xBB, 0xAA, 0x77, 0x66, 0x44, 0x00,

                              0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                              0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
                              0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,

                              0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,

                              0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

                              0x16, 0x13, 0xAA, 0xB3, 0x25, 0x81, 0xE8, 0x2A,
                              0x1B, 0xEB, 0xF8, 0x85, 0xE1, 0x28, 0xFF, 0xA4,

                              0x34, 0x09, 0x91, 0xA7, 0x63, 0xB8, 0x99, 0xA1,
                              0x3F, 0xE4, 0xD0, 0x61, 0x66, 0x73, 0x59, 0x7C,

                              0x86, 0x04, 0x2F, 0xAC, 0x85, 0x17, 0xD6, 0xA1,
                              0x03, 0xCF, 0x27, 0xE4, 0xF8, 0x27, 0x89, 0x2C,

                              0x30, 0x1B, 0xD4, 0x93, 0xD3, 0x6E, 0x35, 0x13,
                              0x53, 0x05, 0x75, 0xB9, 0x79, 0xCF, 0x36, 0x00,

                              0xD4, 0x2C, 0xC6, 0x4E, 0x2C, 0x12, 0xE2, 0x15,
                              0x8B, 0xAF, 0x3D, 0xEF, 0x6E, 0xF1, 0xBF, 0xD9,

                              0x43, 0x17, 0x2B, 0xF3, 0x12, 0xC2, 0xCB, 0x8C,
                              0x3B, 0x43, 0xF2, 0xDF, 0x5D, 0xF9, 0xEF, 0x31,

                              0x6D, 0x46, 0xF6, 0x7A, 0xEE, 0x17, 0x35, 0xF4,
                              0xDA, 0xFE, 0x7C, 0x28, 0xB8, 0x55, 0x12, 0x57,

                              0xFF, 0x82, 0xBB, 0x85, 0xEF, 0xD4, 0x7C, 0xA1,
                              0x05, 0xB4, 0xFF, 0xC1, 0x95, 0x27, 0x30, 0x03};

// _asmUpdateWav was: 653/628 clock cycles, 134 bytes
// now: 684/658 clock cycles, 144 bytes
// = +31 cycles, 4.7% worse
void updateWav(void) {

  if (wavNoteOffTrigger) {
    // Turn off wave channel
    rAUD3LEVEL = 0;
    wavNoteOffTrigger = false;
    return;
  }

  // Check if wave data needs to be updated
  if (wavShapeLast != wavDataOffset) {
    loadWav(wavDataOffset);

    pbWheelInLast[WAV] = PBWHEEL_CENTER;
    return;
  }

  if (pbWheelIn[WAV] != PBWHEEL_CENTER) {
    if (pbWheelIn[WAV] != pbWheelInLast[WAV]) {
      pbWheelInLast[WAV] = pbWheelIn[WAV];
      pbWheelActive[WAV] = true;
      setPitchBendFrequencyOffset(WAV);
      wavCurrentFreq = currentFreqData[WAV];
      return;
    } else {
      pbWheelActive[WAV] = true;
    }
    return;
  }

  if (pbWheelActive[WAV]) {
    // Reset pitch bend wheel
    pbWheelInLast[WAV] = PBWHEEL_CENTER;

    wavCurrentFreq = freq[noteStatus[WAV].note];

    currentFreqData[WAV] = wavCurrentFreq;
    rAUD3LOW = wavCurrentFreq;
    rAUD3HIGH = (wavCurrentFreq >> 8U) & ~AUDHIGH_RESTART;
  }
}

// _asmPlayNoteWav was: 842/827 clock cycles, 191 bytes
// now: 922/907 clock cycles, 205 bytes
// = +80 cycles, 9.5% worse
void playNoteWav(void) {
  const uint8_t noteIndex = addressByte + WAV_OCTAVE_OFFSET + wavOct;

  if (noteIndex >= MAX_FREQ) {
    return;
  }

  // Note off
  if (!valueByte) {
    if (noteStatus[WAV].note == noteIndex) {
      noteStatus[WAV].active = false;

      if (!wavSus) {
        wavNoteOffTrigger = true;
      }
    }
    return;
  }

  currentFreqData[WAV] = wavCurrentFreq = freq[noteIndex];

  if (wavCurrentFreq == 0) {
    return;
  }

  // Note on
  noteStatus[WAV].note = noteIndex;

  // channel volume louder = smaller value:
  // const uint8_t noteVelocity = valueByte & AUD3LEVEL_MASK;
  // if (noteVelocity == 0x60) {
  rAUD3LEVEL = AUD3LEVEL_100; // fixed max vol
  // } else if (noteVelocity == 0x40) {
  //   rAUD3LEVEL = AUD3LEVEL_50;
  // } else {
  //   rAUD3LEVEL = AUD3LEVEL_25;
  // }

  // rAUD3HIGH = 0x00; // was in ASM, probably not needed?

  rAUD3LOW = wavCurrentFreq;
  rAUD3HIGH = wavCurrentFreq >> 8U;

  // Reset various counters and flags
  vibratoPosition[WAV] = 0;
  wavStepCounter = 0;
  counterWav = 0;
  counterWavStart = 0;
  pbWheelInLast[WAV] = PBWHEEL_CENTER;

  // Set pitch bend range
  pbNoteRange[WAV].low = noteIndex - pbRange[WAV];
  pbNoteRange[WAV].high = noteIndex + pbRange[WAV];

  noteStatus[WAV].active = true;
  cueWavSweep = true;
  wavNoteOffTrigger = false;
}

/** Manual sweep on the WAV channel */
void updateWavSweep(void) {
  if (wavSweepSpeed) {
    if (!wavStepCounter) {
      counterWav++;
      if (wavSweepSpeed && cueWavSweep) {
        wavCurrentFreq = currentFreqData[WAV] - (counterWav << wavSweepSpeed);
        if (!(wavSweepSpeed >> 3U) && (wavCurrentFreq > 0x898U)) {
          wavCurrentFreq = 0U;
          cueWavSweep = false;
        }
        rAUD3HIGH = wavCurrentFreq >> 8U;
        rAUD3LOW = wavCurrentFreq;
      }
    }
    wavStepCounter++;
    if (wavStepCounter == 0x02U)
      wavStepCounter = 0;
  }
}

// _asmLoadWav was: 697 clock cycles, 159 bytes
// now: 235 clock cycles, 53 bytes
// + _memcpy: 247/227 clock cycles, 44 bytes
// = total: 482/462 clock cycles, 97 bytes
// = -215 cycles, 44% better
void loadWav(uint8_t offset) {
  wavShapeLast = offset;

  // Turn off wave channel
  rAUD3ENA = AUDENA_OFF;

  memcpy(&_AUD3WAVERAM, &wavData[offset], 16U);

  // Turn on wave channel
  rAUD3ENA = AUDENA_ON;

  // Set wave frequency registers
  rAUD3LOW = wavCurrentFreq;
  rAUD3HIGH = (wavCurrentFreq >> 8U) | AUDHIGH_RESTART;

  systemIdle = false;
}

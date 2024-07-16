#include "noi.h"
#include "../io/midi.h"
#include "../mGB.h"
#include "common.h"

uint8_t noiFreq[72] = {
    0x94, 0x87, 0x86, 0x85, 0x84, 0x77, 0x76, 0x75, 0x74, 0x67, 0x66, 0x65,
    0x64, 0x57, 0x56, 0x55, 0x54, 0x47, 0x46, 0x45, 0x44, 0x37, 0x36, 0x35,
    0x34, 0x27, 0x26, 0x25, 0x24, 0x17, 0x16, 0x15, 0x14, 0x07, 0x06, 0x00,
    0x9C, 0x8F, 0x8E, 0x8D, 0x8C, 0x7F, 0x7E, 0x7D, 0x7C, 0x6F, 0x6E, 0x6D,
    0x6C, 0x5F, 0x5E, 0x5D, 0x5C, 0x4F, 0x4E, 0x4D, 0x4C, 0x3F, 0x3E, 0x3D,
    0x3C, 0x2F, 0x2E, 0x2D, 0x2C, 0x1F, 0x1E, 0x1D, 0x1C, 0x0F, 0x0E, 0x08};

uint8_t noiEnv;
uint8_t noiMode;
bool noiSus;

bool noiNoteOffTrigger;
int8_t noiOct;

void updateNoi(void) {
  if (pbWheelIn[NOI] != PBWHEEL_CENTER) {
    if (pbWheelIn[NOI] != pbWheelInLast[NOI]) {
      pbWheelInLast[NOI] = pbWheelIn[NOI];
      pbWheelActive[NOI] = true;
      setPitchBendFrequencyOffsetNoise();
    } else {
      pbWheelActive[NOI] = true;
    }
    return;
  }
  if (pbWheelActive[NOI]) {
    pbWheelActive[NOI] = 0x00;
    pbWheelInLast[NOI] = 0x80;

    uint8_t freq = noiFreq[noteStatus[NOI].note];

    rAUD4POLY = freq;
    currentFreqData[NOI] = freq;

    rAUD4LEN = 0xFF;

    // The following line is commented out in the original assembly:
    // rAUD4GO = 0x80;
  }
}

void playNoteNoi(void) {
  uint8_t noteIndex = addressByte - 24U + noiOct;

  if (valueByte == 0) {
    // Note off
    if (noteStatus[NOI].note == noteIndex) {
      noteStatus[NOI].active = 0;

      if (!noiSus) {
        rAUD4ENV = 0x00;
      }
    }
    return;
  }

  // Note on
  noteStatus[NOI].note = noteIndex;

  // Set envelope
  uint8_t envelope = ((valueByte << 1) & 0xF0) | noiEnv;
  rAUD4ENV = envelope;

  // Set frequency
  uint8_t freq = noiFreq[noteIndex];
  rAUD4POLY = freq;
  currentFreqData[NOI] = freq;

  // Set channel control
  rAUD4LEN = 0xFF;
  rAUD4GO = 0x80;

  // Reset vibrato and pitch bend
  vibratoPosition[NOI] = 0;
  pbWheelInLast[NOI] = PBWHEEL_CENTER;

  // Set note status
  noteStatus[NOI].active = true;
}

void setPitchBendFrequencyOffsetNoise(void) {
  systemIdle = 0;
  if (pbWheelIn[NOI] & PBWHEEL_CENTER) {
    // noteStatus[NOI].note = noteStatus[NOI].note;
    currentFreq = noiFreq[noteStatus[NOI].note +
                          ((pbWheelIn[NOI] - PBWHEEL_CENTER) >> 3)];
  } else {
    // noteStatus[NOI].note = noteStatus[NOI].note;
    currentFreq = noiFreq[noteStatus[NOI].note -
                          ((PBWHEEL_CENTER - pbWheelIn[NOI]) >> 3)];
  }
  rAUD4POLY = currentFreq;
  currentFreqData[NOI] = currentFreq;
}

#include "data.h"
#include "common.h"
#include "noi.h"
#include "pulse.h"
#include "wav.h"

uint8_t dataSet[28] = {
    0x02, // 0   Transpose 0 - 6
    0x00, // 1   Shape 0 - 3
    0x06, // 2   Envelope 0 - F
    0x00, // 3   Sweep 0 - F
    0x02, // 4   PB Range 0 - 12
    0x00, // 5   Sustain 0 - 1
    0x03, // 6   Pan 0 - 3

    0x02, // 7   Transpose 0 - 6
    0x00, // 8   Shape 0 - 3
    0x06, // 9   Envelope 0 - F
    0x02, // 10  PB Range 0 - 12
    0x00, // 11  Sustain 0 - 1
    0x03, // 12  Pan 0 - 3

    0x02, // 13  Transpose 0 - 6
    0x00, // 14  Shape 0 - F
    0x00, // 15  Offset 0 - F
    0x00, // 16  Sweep 0 - F
    0x02, // 17  PB Range 0 - 12
    0x00, // 18  Sustain 0 - 1
    0x03, // 19  Pan 0 - 3

    0x02, // 20  Transpose 0 - 6
    0x06, // 21  Env 0 - F
    0x00, // 22  Sustain 0 - 1
    0x03, // 23  Pan 0 - 3

    0x00, // 24  Save 0
    0x00, // 25  Save 1
    0x00, // 26  Save 2
    0x00, // 27  Save 3
};

uint8_t dataSetSnap[28];

bool parameterLock[24];

void updateValueSynth(Parameter p) {
  // EMU_printf("updateValueSynth(%d)\n", p);
  switch (p) {
  case PU1_Transpose:
    pu1State.octave = dataSet[p];
    pu1State.octave = (pu1State.octave - 2U) * 12U;
    break;
  case PU1_Shape:
    rAUD1LEN = ((dataSet[p] << 3) << 3) | 7;
    break;
  case PU1_Envelope:
    pu1State.envelope = dataSet[p];
    break;
  case PU1_Sweep:
    NR10_REG = dataSet[p];
    break;
  case PU1_PBRange:
    pbRange[0] = dataSet[p];
    break;
  case PU1_Sustain:
    pu1State.sus = dataSet[p];
    if (!dataSet[p] && !noteStatus[PU1].active)
      rAUD1ENV = 0U;
    break;
  case PU1_Pan:
    setOutputPanBySynth(0U, dataSet[p]);
    break;
  case PU2_Transpose:
    pu2State.octave = dataSet[p];
    pu2State.octave = (pu2State.octave - 2U) * 12U;
    break;
  case PU2_Shape:
    NR21_REG = ((dataSet[p] << 3) << 3) | 7;
    break;
  case PU2_Envelope:
    pu2State.envelope = dataSet[p];
    break;
  case PU2_PBRange:
    pbRange[PU2] = dataSet[p];
    break;
  case PU2_Sustain:
    pu2State.sus = dataSet[p];
    if (!dataSet[p] && !noteStatus[PU2].active)
      rAUD2ENV = 0U;
    break;
  case PU2_Pan:
    setOutputPanBySynth(PU2, dataSet[p]);
    break;
  case WAV_Transpose:
    wavOct = dataSet[p];
    wavOct = (wavOct - 2U) * 12U;
    break;
  case WAV_Shape:
    wavDataOffset = (dataSet[p] << 4) + dataSet[WAV_Offset];
    break;
  case WAV_Offset:
    wavDataOffset = (dataSet[WAV_Shape] << 4) + dataSet[p];
    break;
  case WAV_Sweep:
    wavSweepSpeed = dataSet[p];
    break;
  case WAV_PBRange:
    pbRange[WAV] = dataSet[p];
    break;
  case WAV_Sustain:
    wavSus = dataSet[p];
    if (!dataSet[p] && !noteStatus[WAV].active)
      rAUD3LEVEL = 0U;
    break;
  case WAV_Pan:
    setOutputPanBySynth(2, dataSet[p]);
    break;
  case NOI_Transpose:
    noiState.octave = dataSet[p];
    noiState.octave = (noiState.octave - 2U) * 12U;
    break;
  case NOI_Env:
    noiState.envelope = dataSet[p];
    break;
  case NOI_Sustain:
    noiState.sus = dataSet[p];
    if (!dataSet[p] && !noteStatus[NOI].active)
      rAUD4ENV = 0U;
    break;
  case NOI_Pan:
    setOutputPanBySynth(NOI, dataSet[p]);
    break;
  default:
    break;
  }
}

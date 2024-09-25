
#include "main.h"
#include "../io/pad.h"
#include "../io/sram.h"
#include "../mGB.h"
#include "../synth/common.h"
#include "../synth/data.h"
#include "../synth/pulse.h"
#include "../synth/wav.h"
#include "screen.h"

static const uint8_t VERSION_NUMBER[] = "v.1.3.4-HustlaMod";

static const uint8_t HELP_DATA[10][18] = {
    "octave           ",
    "wav          cc01",
    "envelope     cc02",
    "sweep        cc03",
    "pb range     cc04",
    "sustain      cc64",
    "pan          cc10",
    "preset       cc05",
    "wav offset   nocc",
    //
    "                 ",
};

// Lookup from cursor position to HELP_DATA
static const uint8_t HELP_MAP[4][9] = {{0, 1, 2, 3, 4, 5, 6, 9, 7},
                                       {0, 1, 2, 9, 4, 5, 6, 9, 7},
                                       {0, 1, 8, 3, 4, 5, 6, 9, 7},
                                       {0, 9, 2, 9, 9, 5, 6, 9, 7}};

const uint8_t SCREEN_XO = 8;
const uint8_t SCREEN_YO = 16;
const uint8_t SCREEN_XSPACE = 32;
const uint8_t SCREEN_YSPACE = 8;
const uint8_t SCREEN_PSTARTX = 24;
const uint8_t SCREEN_PSTARTY = 41;

const uint8_t NUM_COLS = 4;
const uint8_t LAST_COL = 3;

const uint8_t NUM_ROWS = 9;
const uint8_t LAST_ROW = 8;

bool cursorEnable[4] = {1, 0, 0, 0};
uint8_t cursorColumn;
uint8_t cursorRow[4];
uint8_t cursorBigStartX[4];
uint8_t cursorBigStartY[4];
uint8_t cursorStartX[4];
uint8_t cursorStartY[4];
uint8_t cursorColumnLast;
uint8_t cursorRowMain;

uint8_t updateDisplaySynthCounter;

const uint8_t cursorToParam[4][9] = {
    {0, 1, 2, 3, 4, 5, 6, PARAM_NONE, 24},
    {7, 8, 9, PARAM_NONE, 10, 11, 12, PARAM_NONE, 25},
    {13, 14, 15, 16, 17, 18, 19, PARAM_NONE, 26},
    {20, PARAM_NONE, 21, PARAM_NONE, PARAM_NONE, 22, 23, PARAM_NONE, 27}};

const uint8_t paramToLoc[28][3] = {
    {3, 5, 6},   {3, 6, 4},   {3, 7, 16},   {3, 8, 255},
    {3, 9, 49},  {3, 10, 2},  {3, 11, 4},

    {7, 5, 6},   {7, 6, 4},   {7, 7, 16},   {7, 9, 49},
    {7, 10, 2},  {7, 11, 4},

    {11, 5, 6},  {11, 6, 16}, {11, 7, 32},  {11, 8, 16},
    {11, 9, 49}, {11, 10, 2}, {11, 11, 4},

    {15, 5, 6},  {15, 7, 16}, {15, 10, 2},  {15, 11, 4},

    {3, 13, 16}, {7, 13, 16}, {11, 13, 16}, {15, 13, 16},
};

const uint8_t octmap[6][2] = {{3, 65}, {2, 65}, {83, 83},
                              {2, 64}, {3, 64}, {4, 64}};

const uint8_t panmap[4][2] = {
    {37, 37},
    {37, 28},
    {22, 37},
    {22, 28},
};

const uint8_t susmap[2][2] = {
    {83, 0},
    {82, 0},
};

const uint8_t markerMapTiles[4][4] = {{0x03, 0x02, 0x00, 0x5B},
                                      {0x07, 0x02, 0x00, 0x5B},
                                      {0x0B, 0x02, 0x00, 0x5B},
                                      {0x0F, 0x02, 0x00, 0x5B}};

void initMainScreen(void) {
  for (j = 0; j != NUM_COLS; j++) {
    cursorBigStartX[j] = SCREEN_XO + SCREEN_PSTARTX + (j * SCREEN_XSPACE) - 1;
    cursorBigStartY[j] = SCREEN_YO + SCREEN_PSTARTY - 8;
    cursorStartX[j] = SCREEN_XO + SCREEN_PSTARTX + (j * SCREEN_XSPACE) - 8;
    cursorStartY[j] = SCREEN_YO + SCREEN_PSTARTY;
    cursorRow[j] = cursorStartY[j];
  }
}

void showMainScreen(void) {
  // EMU_printf("showMainScreen\n");
  cls();
  currentScreen = SCREEN_MAIN;
  bkg[0] = 66;
  set_bkg_tiles(3, 3, 1, 1, bkg);
  bkg[0] = 67;
  set_bkg_tiles(7, 3, 1, 1, bkg);
  bkg[0] = 68;
  set_bkg_tiles(11, 3, 1, 1, bkg);
  bkg[0] = 69;
  set_bkg_tiles(15, 3, 1, 1, bkg);

  for (j = 0; j != 28; j++) {
    bkg[0] = bkg[1] = 1;
    set_bkg_tiles(paramToLoc[j][0], paramToLoc[j][1], 2, 1, bkg);
  }

  updateDisplay();
  showCursor();
}

void renderMainScreen(void) {
  // EMU_printf("renderMainScreen\n");
  updateDisplaySynthCounter = (updateDisplaySynthCounter + 1) & 3U;

  updateDisplaySynth();
  // printbyte(statusByte, addressByte, valueByte);
  setPlayMarker();
}

void printVersion(void) {
  //
  printString(1, 16, sizeof(VERSION_NUMBER), VERSION_NUMBER);
}

void printHelp(void) {
  j = HELP_MAP[cursorColumn][cursorRowMain];
  printString(1, 16, sizeof(HELP_DATA[j]), HELP_DATA[j]);
}

void updateDisplayValue(Parameter p, uint8_t v) {
  bkg[1] = 0;
  switch (p) {
  case PU1_Transpose:
  case PU2_Transpose:
  case WAV_Transpose:
  case NOI_Transpose:
    set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, octmap[v]);
    break;
  case PU1_Shape:
  case PU2_Shape:
    bkg[0] = 44 + v;
    set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, bkg);
    break;
  case PU1_Envelope:
  case PU2_Envelope:
  case NOI_Env:
    if (!v) {
      bkg[0] = 83;
      set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, bkg);
    } else {
      bkg[0] = 48 + v;
      set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, bkg);
    }
    break;
  case PU1_PBRange:
  case PU2_PBRange:
  case WAV_PBRange:
    bkg[0] = 1 + (v >> 4);
    bkg[1] = 1 + (0x0F & v);
    set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, bkg);
    break;
  case PU1_Sustain:
  case PU2_Sustain:
  case WAV_Sustain:
  case NOI_Sustain:
    set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, susmap[v]);
    break;
  case PU1_Pan:
  case PU2_Pan:
  case WAV_Pan:
  case NOI_Pan:
    set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, panmap[v]);
    break;
  case WAV_Shape:
    if (v > 7U) {
      bkg[1] = v - 7;
      bkg[0] = 48;
      set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, bkg);
    } else {
      bkg[0] = 39 + v;
      set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, bkg);
    }
    break;
  case PU1_Sweep:
    bkg[0] = 1 + (v >> 4);
    bkg[1] = 1 + (0x0F & v);
    set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, bkg);
    break;
  case WAV_Offset:
    bkg[0] = 1 + (v >> 4);
    bkg[1] = 1 + (0x0F & v);
    set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, bkg);
    break;
  case WAV_Sweep:
    bkg[0] = 1 + v;
    set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, bkg);
    break;
  case PU1_Slot:
  case PU2_Slot:
  case WAV_Slot:
  case NOI_Slot:
    bkg[0] = 1 + (0x0F & v);
    set_bkg_tiles(paramToLoc[p][0], paramToLoc[p][1], 2, 1, bkg);
    break;
  default:
    break;
  }
}

void updateDisplaySynth(void) {
  // printbyte(serialBufferPosition,serialBufferReadPosition,serialBuffer[serialBufferPosition]);
  // EMU_printf("updateDislpaySynthCounter = %d\n", updateDisplaySynthCounter);

  for (i = 0; i != NUM_ROWS; i++) {
    Parameter p = cursorToParam[updateDisplaySynthCounter][i];
    if (p != PARAM_NONE) {
      updateDisplayValue(p, dataSet[p]);
    }
  }
}

void updateDisplay(void) {
  for (j = 0; j != NUM_COLS; j++) {
    for (i = 0; i != NUM_ROWS; i++) {
      const Parameter p = cursorToParam[j][i];
      if (p != PARAM_NONE) {
        // updateValue(p, dataSet[p]);
        updateDisplayValue(p, dataSet[p]);

        // TODO: why is this needed here?
        // updateValueSynth(p);
      }
    }
  }
}

void setCursor(void) {
  const bool shiftSelect = (joyState & J_SELECT) > 0;

  if (cursorColumnLast != cursorColumn) {
    if (cursorColumn > 0xF0U)
      cursorColumn = LAST_COL;
    if (cursorColumn > LAST_COL)
      cursorColumn = 0;
    cursorEnable[cursorColumn] = true;
    if (!shiftSelect) {
      cursorEnable[cursorColumnLast] = false;
    }

    // cursorRow[cursorColumn] = cursorRowMain;
    cursorColumnLast = cursorColumn;
  }

  for (j = 0; j != NUM_COLS; j++) {
    if (!shiftSelect && cursorColumn != j) {
      cursorEnable[j] = false;
    }
    if (cursorEnable[j]) {
      if (cursorRow[j] > 0xF0U)
        cursorRow[j] = LAST_ROW;
      if (cursorRow[j] > LAST_ROW)
        cursorRow[j] = 0;
      move_sprite(SPRITE_ARRT_START, cursorBigStartX[cursorColumn],
                  cursorBigStartY[0]);
      move_sprite(j + SPRITE_ARRL_START, cursorStartX[j],
                  (cursorRow[j] * SCREEN_YSPACE) + SCREEN_PSTARTY + SCREEN_YO);
      if (j == cursorColumn)
        cursorRowMain = cursorRow[j];
    }
  }
  for (j = 0; j != NUM_COLS; j++) {
    if (!cursorEnable[j]) {
      cursorRow[j] = cursorRowMain;
      move_sprite(j + SPRITE_ARRL_START, 0, 0);
    }
  }
  printHelp();
}

void showCursor(void) { setCursor(); }

void setPlayMarker(void) {
  for (j = 0; j != NUM_COLS; j++) {
    bkg[0] = markerMapTiles[j][(2U + noteStatus[j].active)];
    set_bkg_tiles(markerMapTiles[j][0], markerMapTiles[j][1U], 1U, 1U, bkg);
  }
}

void clearParameterLocks(void) {
  for (j = 0; j != 24; j++)
    parameterLock[j] = false;
}

void setDataValue(void) {
  bool increasing = false;
  uint8_t delta = 1;
  systemIdle = false;

  j = 0;
  if (i & J_UP) {
    j = 1;
    increasing = true;
    delta = 16;
  } else if (i & J_DOWN) {
    j = 1;
    delta = 16;
  } else if (i & J_LEFT) {
    j = 1;
  } else if (i & J_RIGHT) {
    increasing = true;
    j = 1;
  }
  if (j) {
    for (j = 0; j != NUM_COLS; j++) {
      const Parameter p = cursorToParam[j][cursorRow[j]];
      if (cursorEnable[j] && p != PARAM_NONE) {
        const uint8_t limit = paramToLoc[p][2];
        switch (p) {
        case PU1_Pan:
        case PU2_Pan:
        case WAV_Pan:
        case NOI_Pan:
          if (i & J_DOWN) {
            dataSet[p] = 0;
          } else if (i & J_UP) {
            dataSet[p] = 3;
          } else if (i & J_LEFT) {
            dataSet[p] = 2;
          } else if (i & J_RIGHT) {
            dataSet[p] = 1;
          }
          break;
        case PU1_Transpose:
        case PU2_Transpose:
        case WAV_Transpose:
        case NOI_Transpose:
          delta = 1;
        default:
          if (increasing) {
            dataSet[p] += delta;
            if (dataSet[p] >= limit)
              dataSet[p] = (limit - 1);
          } else if (dataSet[p]) {
            if (dataSet[p] > delta) {
              dataSet[p] -= delta;
            } else {
              dataSet[p] = 0;
            }
          }
          // EMU_printf("setDataValue dataSet[%d]: %d\n", p, dataSet[p]);
        }
        if (p < NUM_PARAMS) {
          parameterLock[p] = true;
        }
        updateValueSynth(p);
      }
    }
  }
}

void getPadMainScreen(void) {
  if (joyState == 0) {
    clearParameterLocks();
    return;
  }

  if (joyState & J_A) {
    setDataValue();
    return;
  }

  if (joyState & J_B) {
    if (joyState & J_SELECT) {
      snapRecall(RECALL_SAVE);
    } else {
      snapRecall(RECALL_LOAD);
    }
    return;
  }

  if (joyState & J_UP) {
    cursorRow[cursorColumn]--;
    setCursor();
    return;
  }

  if (joyState & J_RIGHT) {
    cursorColumn++;
    setCursor();
    return;
  }

  if (joyState & J_DOWN) {
    cursorRow[cursorColumn]++;
    setCursor();
    return;
  }

  if (joyState & J_LEFT) {
    cursorColumn--;
    setCursor();
    return;
  }
}

void printbyte(uint8_t v1, uint8_t v2, uint8_t v3) {
  bkg[0] = (v1 >> 4) + 1;
  bkg[1] = (0x0F & v1) + 1;

  bkg[2] = 0;

  bkg[3] = (v2 >> 4) + 1;
  bkg[4] = (0x0F & v2) + 1;

  bkg[5] = 0;

  bkg[6] = (v3 >> 4) + 1;
  bkg[7] = (0x0F & v3) + 1;

  bkg[8] = 0;
  set_bkg_tiles(1, 16, 10, 1, bkg);
}

void snapRecall(RecallMode mode) {
  if (cursorRowMain == LAST_ROW) {
    for (l = 0; l < NUM_COLS; l++) {
      if (cursorEnable[l]) {
        if (mode == RECALL_SAVE) {
          saveDataSet(l);
        } else {
          loadDataSet(l);
          updateSynth(l);
        }
      }
    }
  } else {
    if (mode == RECALL_SAVE) {
      for (j = 0; j != 24; j++)
        dataSetSnap[j] = dataSet[j];
    } else {
      for (j = 0; j != 24; j++)
        dataSet[j] = dataSetSnap[j];
      updateDisplay();
    }
  }
}

void updateSynth(uint8_t synth) {
  Parameter p;
  for (i = 0; i != NUM_ROWS; i++) {
    p = cursorToParam[synth][i];
    if (p != PARAM_NONE) {
      // TODO: is this noop assign needed?
      // dataSet[p] = dataSet[p];
      updateValueSynth(p);
    }
  }
}

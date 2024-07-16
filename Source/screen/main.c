
#include "main.h"
#include "../io/sram.h"
#include "../mGB.h"
#include "../synth/common.h"
#include "../synth/data.h"
#include "../synth/pulse.h"
#include "../synth/wav.h"
#include "utils.h"

bool recallMode;
uint8_t lastPadRead;
bool joyState[8] = {0, 0, 0, 0, 0, 0, 0, 0};
// v 1.3.3
const uint8_t versionnumber[10] = {32, 81, 2, 81, 4, 81, 4, 0, 0, 0};

// TODO: can these be represented more clearly as text strings?
const uint8_t helpdata[10][18] = {
    {25, 13, 30, 11, 32, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {33, 11, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 1, 2},
    {15, 24, 32, 15, 22, 25, 26, 15, 0, 0, 0, 0, 0, 0, 13, 13, 1, 3},
    {29, 33, 15, 15, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 1, 4},
    {26, 12, 0, 28, 11, 24, 17, 15, 0, 0, 0, 0, 0, 0, 13, 13, 1, 5},
    {29, 31, 29, 30, 11, 19, 24, 0, 0, 0, 0, 0, 0, 0, 13, 13, 7, 5},
    {26, 11, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 2, 1},
    {26, 28, 15, 29, 15, 30, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 1, 6},
    {33, 11, 32, 0, 25, 16, 16, 29, 15, 30, 0, 0, 0, 0, 13, 13, 1, 3},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

const uint8_t helpmap[4][9] = {{0, 1, 2, 3, 4, 5, 6, 9, 7},
                               {0, 1, 2, 9, 4, 5, 6, 9, 7},
                               {0, 1, 8, 3, 4, 5, 6, 9, 7},
                               {0, 9, 2, 9, 9, 5, 6, 9, 7}};

const uint8_t SCREEN_XO = 8;
const uint8_t SCREEN_YO = 16;
const uint8_t SCREEN_XSPACE = 32;
const uint8_t SCREEN_YSPACE = 8;
const uint8_t SCREEN_PSTARTX = 24;
const uint8_t SCREEN_PSTARTY = 41;

bool cursorEnable[4] = {1, 0, 0, 0};
uint8_t cursorColumn;
uint8_t cursorRow[4];
uint8_t cursorBigStartX[4];
uint8_t cursorBigStartY[4];
uint8_t cursorStartX[4];
uint8_t cursorStartY[4];
uint8_t cursorColumnLast;
uint8_t cursorRowMain;
bool shiftSelect;

uint8_t updateDisplaySynthCounter;

const uint8_t tableCursorLookup[4][9] = {
    {0, 1, 2, 3, 4, 5, 6, 255, 24},
    {7, 8, 9, 255, 10, 11, 12, 255, 25},
    {13, 14, 15, 16, 17, 18, 19, 255, 26},
    {20, 255, 21, 255, 255, 22, 23, 255, 27}};

const uint8_t tableData[28][3] = {
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
  for (j = 0; j != 4; j++) {
    cursorBigStartX[j] = SCREEN_XO + SCREEN_PSTARTX + (j * SCREEN_XSPACE) - 1;
    cursorBigStartY[j] = SCREEN_YO + SCREEN_PSTARTY - 8;
    cursorStartX[j] = SCREEN_XO + SCREEN_PSTARTX + (j * SCREEN_XSPACE) - 8;
    cursorStartY[j] = SCREEN_YO + SCREEN_PSTARTY;
    cursorRow[j] = cursorStartY[j];
  }
}

void showMainScreen(void) {
  cls();
  currentScreen = 1;
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
    set_bkg_tiles(tableData[j][0], tableData[j][1], 2, 1, bkg);
  }

  updateDisplay();
  showCursor();
}

void toggleScreen(void) {
  if (currentScreen == 0) {
    DISPLAY_ON;
    showMainScreen();
  } else {
    currentScreen = 0;
    DISPLAY_OFF;
  }
}

void mainScreen(void) {
  if (currentScreen == 0) {
    return;
  };

  updateDisplaySynthCounter = (updateDisplaySynthCounter + 1) & 3U;

  updateDisplaySynth();
  // printbyte(statusByte, addressByte, valueByte);
  setPlayMarker();
}

void printversion(void) { set_bkg_tiles(1, 16, 10, 1, versionnumber); }

void printhelp(void) {
  j = helpmap[cursorColumn][cursorRowMain];
  set_bkg_tiles(1, 16, 18, 1, helpdata[j]);
}

void updateDisplayValue(uint8_t p, uint8_t v) {
  bkg[1] = 0;
  switch (p) {
  case 0U:
  case 7U:
  case 13U:
  case 20U:
    set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, octmap[v]);
    break;
  case 1U:
  case 8U:
    bkg[0] = 44 + v;
    set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, bkg);
    break;
  case 2U:
  case 9U:
  case 21U:
    if (!v) {
      bkg[0] = 83;
      set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, bkg);
    } else {
      bkg[0] = 48 + v;
      set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, bkg);
    }
    break;
  case 4U:
  case 10U:
  case 17U:
    bkg[0] = 1 + (v >> 4);
    bkg[1] = 1 + (0x0F & v);
    set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, bkg);
    break;
  case 5U:
  case 11U:
  case 18U:
  case 22U:
    set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, susmap[v]);
    break;
  case 6U:
  case 12U:
  case 19U:
  case 23U:
    set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, panmap[v]);
    break;
  case 14U:
    if (v > 7U) {
      bkg[1] = v - 7;
      bkg[0] = 48;
      set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, bkg);
    } else {
      bkg[0] = 39 + v;
      set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, bkg);
    }
    break;
  case 3U:
    bkg[0] = 1 + (v >> 4);
    bkg[1] = 1 + (0x0F & v);
    set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, bkg);
    break;
  case 15U:
    bkg[0] = 1 + (v >> 4);
    bkg[1] = 1 + (0x0F & v);
    set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, bkg);
    break;
  case 16U:
    bkg[0] = 1 + v;
    set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, bkg);
    break;
  case 24U:
  case 25U:
  case 26U:
  case 27U:
    bkg[0] = 1 + (0x0F & v);
    set_bkg_tiles(tableData[p][0], tableData[p][1], 2, 1, bkg);
    break;
  default:
    break;
  }
}

void updateDisplaySynth(void) {
  // printbyte(serialBufferPosition,serialBufferReadPosition,serialBuffer[serialBufferPosition]);
  for (i = 0; i != 0x09U; i++) {
    if (tableCursorLookup[updateDisplaySynthCounter][i] != 0xFFU) {
      updateDisplayValue(
          tableCursorLookup[updateDisplaySynthCounter][i],
          dataSet[tableCursorLookup[updateDisplaySynthCounter][i]]);
    }
  }
}

void updateDisplay(void) {
  uint8_t x = 0;
  for (j = 0; j != 0x04U; j++) {
    for (i = 0; i != 0x09U; i++) {
      if (tableCursorLookup[j][i] != 0xFFU) {
        // updateValue(tableCursorLookup[j][i],dataSet[tableCursorLookup[j][i]]);
        updateDisplayValue(tableCursorLookup[j][i],
                           dataSet[tableCursorLookup[j][i]]);

        // TODO: why is this needed here?
        // updateValueSynth(tableCursorLookup[j][i]);
      }
    }
  }
}

void setCursor(void) {
  if (cursorColumnLast != cursorColumn) {
    if (cursorColumn > 0xF0U)
      cursorColumn = 0x03U;
    if (cursorColumn > 0x03U)
      cursorColumn = 0U;
    cursorEnable[cursorColumn] = 1U;
    if (!joyState[6]) {
      cursorEnable[cursorColumnLast] = 0U;
    }

    // cursorRow[cursorColumn] = cursorRowMain;
    cursorColumnLast = cursorColumn;
  }

  for (j = 0; j != 4; j++) {
    if (shiftSelect && !joyState[6] && cursorColumn != j) {
      cursorEnable[j] = 0;
    }
    if (cursorEnable[j]) {
      if (cursorRow[j] > 0xF0U)
        cursorRow[j] = 0x08U;
      if (cursorRow[j] > 0x08U)
        cursorRow[j] = 0U;
      move_sprite(SPRITE_ARRT_START, cursorBigStartX[cursorColumn],
                  cursorBigStartY[0]);
      move_sprite(j + SPRITE_ARRL_START, cursorStartX[j],
                  (cursorRow[j] * SCREEN_YSPACE) + SCREEN_PSTARTY + SCREEN_YO);
      if (j == cursorColumn)
        cursorRowMain = cursorRow[j];
    }
  }
  for (j = 0; j != 4; j++) {
    if (!cursorEnable[j]) {
      cursorRow[j] = cursorRowMain;
      move_sprite(j + SPRITE_ARRL_START, 0, 0);
    }
  }
  if (!joyState[6]) {
    shiftSelect = 0;
  } else {
    shiftSelect = 1;
  }
  printhelp();
}

void showCursor(void) { setCursor(); }

void setPlayMarker(void) {
  for (j = 0; j != 4; j++) {
    bkg[0] = markerMapTiles[j][(2U + noteStatus[j].active)];
    set_bkg_tiles(markerMapTiles[j][0], markerMapTiles[j][1U], 1U, 1U, bkg);
  }
}

void clearParameterLocks(void) {
  for (j = 0; j != 24; j++)
    parameterLock[j] = 0;
}

void setDataValue(void) {
  bool up = 0;
  uint8_t inc = 1;
  systemIdle = 0;

  j = 0;
  if (i & J_UP) {
    j = 1;
    up = 1;
    inc = 16;
  } else if (i & J_DOWN) {
    j = 1;
    inc = 16;
  } else if (i & J_LEFT) {
    j = 1;
  } else if (i & J_RIGHT) {
    up = 1;
    j = 1;
  }
  if (j) {
    for (j = 0; j != 4; j++) {
      if (cursorEnable[j] && tableCursorLookup[j][cursorRow[j]] != 0xFFU) {
        x = tableCursorLookup[j][cursorRow[j]];
        l = tableData[x][2];
        switch (x) {
        case 6:
        case 12:
        case 19:
        case 23:
          if (i & J_DOWN) {
            dataSet[x] = 0;
          } else if (i & J_UP) {
            dataSet[x] = 3;
          } else if (i & J_LEFT) {
            dataSet[x] = 2;
          } else if (i & J_RIGHT) {
            dataSet[x] = 1;
          }
          break;
        case 0:
        case 7:
        case 13:
        case 20:
          inc = 1;
        default:
          if (up) {
            dataSet[x] += inc;
            if (dataSet[x] >= l)
              dataSet[x] = (l - 1);
          } else if (dataSet[x]) {
            if (dataSet[x] > inc) {
              dataSet[x] -= inc;
            } else {
              dataSet[x] = 0;
            }
          }
        }
        parameterLock[x] = 1;
        updateValueSynth(x);
      }
    }
  }
}

// TODO: move the main pad handler out, split into screen level handlers
void getPad(void) {
  i = joypad();
  if (i != lastPadRead) {
    lastPadRead = i;
    if (i) {
      if ((i & J_A) && !joyState[0]) {
        joyState[0] = 1;
        if (i & J_SELECT) {
          toggleScreen();
        } else {
          setDataValue();
        }
        return;
      } else if (joyState[0]) {
        joyState[0] = 0;
        setDataValue();
        return;
      }
      if ((i & J_B) && !joyState[1]) {
        joyState[1] = 1;
        if (i & J_SELECT) {
          recallMode = 0;
        } else {
          recallMode = 1;
        }
        snapRecall();
        return;
      } else if (joyState[1]) {
        joyState[1] = 0;
        return;
      }
      if ((i & J_UP) && !joyState[2]) {
        joyState[2] = 1;
        cursorRow[cursorColumn]--;
        setCursor();
        return;
      } else if (joyState[2]) {
        joyState[2] = 0;
        return;
      }
      if ((i & J_RIGHT) && !joyState[5]) {
        joyState[5] = 1;
        cursorColumn++;
        setCursor();
        return;
      } else if (joyState[5]) {
        joyState[5] = 0;
        return;
      }
      if ((i & J_DOWN) && !joyState[3]) {
        joyState[3] = 1;
        cursorRow[cursorColumn]++;
        setCursor();
        return;
      } else if (joyState[3]) {
        joyState[3] = 0;
        return;
      }
      if ((i & J_LEFT) && !joyState[4]) {
        joyState[4] = 1;
        cursorColumn--;
        setCursor();
        return;
      } else if (joyState[4]) {
        joyState[4] = 0;
        return;
      }
      if ((i & J_SELECT) && !joyState[6]) {
        joyState[6] = 1;
        return;
      } else if (joyState[6]) {
        joyState[6] = 0;
        return;
      }
      if ((i & J_START) && !joyState[7]) {
        joyState[7] = 1;
        rAUD1ENV = 0;
        rAUD2ENV = 0;
        rAUD3LEVEL = 0;
        rAUD4ENV = 0;
        pbWheelIn[0] = pbWheelIn[1] = pbWheelIn[2] = pbWheelIn[3] =
            PBWHEEL_CENTER;
        pu1State.sus = pu2State.sus = wavSus = 0;

        return;
      } else if (joyState[7]) {
        joyState[7] = 0;
        return;
      }
    } else {
      clearParameterLocks();
      for (j = 0; j != 8; j++) {
        if (joyState[j]) {
          joyState[j] = 0;
        }
      }
      return;
    }
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

void snapRecall(void) {
  if (cursorRowMain == 0x08U) {
    for (l = 0; l < 4; l++) {
      if (cursorEnable[l]) {
        if (!recallMode) {
          saveDataSet(l);
        } else {
          loadDataSet(l);
          updateSynth(l);
        }
      }
    }
  } else {
    if (!recallMode) {
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
  for (i = 0; i != 0x09U; i++) {
    if (tableCursorLookup[synth][i] != 0xFFU) {
      // TODO: is this noop assign needed?
      dataSet[tableCursorLookup[synth][i]] =
          dataSet[tableCursorLookup[synth][i]];
      updateValueSynth(tableCursorLookup[synth][i]);
    }
  }
}

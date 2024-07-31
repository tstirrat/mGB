#pragma once

#include <gbdk/platform.h>
#include <stdbool.h>

typedef enum RecallMode {
  RECALL_SAVE = 0,
  RECALL_LOAD = 1,
} RecallMode;

// v 1.3.3
extern const uint8_t versionnumber[10];

// TODO: can these be represented more clearly as text strings?
extern const uint8_t helpdata[10][18];

extern const uint8_t helpmap[4][9];

extern const uint8_t SCREEN_XO;
extern const uint8_t SCREEN_YO;
extern const uint8_t SCREEN_XSPACE;
extern const uint8_t SCREEN_YSPACE;
extern const uint8_t SCREEN_PSTARTX;
extern const uint8_t SCREEN_PSTARTY;

extern bool cursorEnable[4];
extern uint8_t cursorColumn;
extern uint8_t cursorRow[4];
extern uint8_t cursorBigStartX[4];
extern uint8_t cursorBigStartY[4];
extern uint8_t cursorStartX[4];
extern uint8_t cursorStartY[4];
extern uint8_t cursorColumnLast;
extern uint8_t cursorRowMain;

extern uint8_t updateDisplaySynthCounter;

// A lookup of cursor position [col][row] to parameter index
// e.g. 0,0 = PU1_Transpose
extern const uint8_t cursorToParam[4][9];

// A lookup per param of p = [x, y, limit]
// e.g. PU1_Transpose (0) = [3, 5, 6] max 6
extern const uint8_t paramToLoc[28][3];

extern const uint8_t octmap[6][2];

extern const uint8_t panmap[4][2];

extern const uint8_t susmap[2][2];

// A lookup of column marker (0-3) -> [x, y, inactive tile, active tile]
extern const uint8_t markerMapTiles[4][4];

void initMainScreen(void);
void updateDisplay(void);
void updateDisplaySynth(void);
void showCursor(void);
void setPlayMarker(void);
void snapRecall(RecallMode mode);
void updateSynth(uint8_t synth);
void showMainScreen(void);
void renderMainScreen(void);
void printversion(void);
void toggleScreen(void);
void clearParameterLocks(void);

void getPadMainScreen(void);

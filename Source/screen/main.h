#pragma once

#include <gbdk/platform.h>
#include <stdbool.h>

extern bool recallMode;
extern uint8_t lastPadRead;
extern bool joyState[8];

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
extern bool shiftSelect;

extern uint8_t updateDisplaySynthCounter;

extern const uint8_t tableCursorLookup[4][9];

extern const uint8_t tableData[28][3];

extern const uint8_t octmap[6][2];

extern const uint8_t panmap[4][2];

extern const uint8_t susmap[2][2];

extern const uint8_t markerMapTiles[4][4];

void initMainScreen(void);
void updateDisplay(void);
void updateDisplaySynth(void);
void showCursor(void);
void setPlayMarker(void);
void snapRecall(void);
void updateSynth(uint8_t synth);
void mainScreen(void);
void showMainScreen(void);
void printversion(void);
void getPad(void);

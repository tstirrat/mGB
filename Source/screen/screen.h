#pragma once

#include <gbdk/platform.h>

#define RGB_LIGHT RGB(23, 29, 31)
#define RGB_DARK RGB(0, 0, 2)
#define RGB_HAWT RGB(31, 5, 31)
#define RGB_HAWTBLU RGB(0, 15, 31)
#define SPRITE_ARRL 0x79
#define SPRITE_ARRT 0x80
#define SPRITE_ARRT_START 26
#define SPRITE_ARRL_START 27

extern uint8_t bkg[20];

typedef enum Screen {
  SCREEN_NONE = 0,
  SCREEN_SPLASH = 1,
  SCREEN_MAIN = 2,
} Screen;

extern Screen currentScreen;

extern const uint8_t data_barrow[16];

extern const uint8_t data_larrow[16];

extern const uint16_t bgpalette[4];
extern const uint16_t spritepalette[8];

void cls(void);
void hideCursor(void);
void displaySetup(void);
void toggleScreen(void);
void renderCurrentScreen(void);
void showScreen(Screen screen);

// Print a string at x, y using the bg tile font
void printString(uint8_t x, uint8_t y, uint8_t len, uint8_t chars[]);

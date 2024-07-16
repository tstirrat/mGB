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
#define SPRITE_PUFREQ_LOW 1

void cls(void);
void hideCursor(void);
void displaySetup(void);

extern uint8_t bkg[20];
extern uint8_t currentScreen;

// TODO: generate from png
extern const uint8_t data_barrow[16];

// TODO: generate from png
extern const uint8_t data_larrow[16];

// TODO: generate from png
extern const uint8_t data_font[1472];

extern const uint16_t bgpalette[4];
extern const uint16_t spritepalette[8];

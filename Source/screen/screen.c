#include "screen.h"
#include "../mGB.h"
#include "bkg_tiles.h"
#include "main.h"
#include "splash.h"

uint8_t bkg[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

Screen currentScreen;

const uint8_t data_barrow[16] = {
    0b00000000, //
    0b00000000, //
    0b11100111, // ooo  ooo
    0b11100111, // ooo  ooo
    0b01111110, //  oooooo
    0b01111110, //  oooooo
    0b00111100, //   oooo
    0b00111100, //   oooo
    0b00011000, //    oo
    0b00011000, //    oo
    0b00000000, //
    0b00000000, //
    0b00000000, //
    0b00000000, //
    0b00000000, //
    0b00000000, //
};

const uint8_t data_larrow[16] = {
    0b11000000, // oo
    0b11000000, // oo
    0b11100000, // ooo
    0b11100000, // ooo
    0b01110000, //  ooo
    0b01110000, //  ooo
    0b11100000, // ooo
    0b11100000, // ooo
    0b11000000, // oo
    0b11000000, // oo
    0b00000000, //
    0b00000000, //
    0b00000000, //
    0b00000000, //
    0b00000000, //
    0b00000000, //
};

const uint16_t bgpalette[] = {RGB_BLACK, RGB_BLACK, RGB_HAWT, RGB_WHITE};

const uint16_t spritepalette[] = {RGB_WHITE, RGB_LIGHTGRAY, RGB_LIGHTGRAY,
                                  RGB_BLACK, RGB_HAWT,      RGB_HAWT,
                                  RGB_HAWT,  RGB_HAWT};

void cls(void) {
  for (j = 0; j != 20; j++)
    bkg[j] = 0x00U;
  for (j = 0; j != 18; j++) {
    set_bkg_tiles(0, j, 20, 1, bkg);
  }
}

void hideCursor(void) {
  for (j = 0; j != 4; j++) {
    move_sprite(j + SPRITE_ARRL_START, 0, 0);
  }
  move_sprite(SPRITE_ARRT_START, 0, 0);
}

void displaySetup(void) {
  DISPLAY_OFF;
  set_bkg_palette(0, 1, &bgpalette[0]);

  /* Initialize the background tiles */
  set_bkg_data(0, 92, data_tiles);

  set_sprite_palette(0, 2, &spritepalette[0]);

  SPRITES_8x8;

  set_sprite_data(SPRITE_ARRT, 0x01U, data_barrow);
  set_sprite_data(SPRITE_ARRL, 0x01U, data_larrow);

  set_sprite_tile(SPRITE_ARRT_START, SPRITE_ARRT);
  for (j = 0; j != 4; j++)
    set_sprite_tile(j + SPRITE_ARRL_START, SPRITE_ARRL);

  for (j = 0; j != 4; j++)
    set_sprite_prop(j + SPRITE_ARRT_START, 1);
  for (j = 0; j != 4; j++)
    set_sprite_prop(j + SPRITE_ARRL_START, 1);

  DISPLAY_ON;

  SHOW_BKG;
  SHOW_SPRITES;
}

void toggleScreen(void) {
  // EMU_printf("toggleScreen\n");
  if (currentScreen == SCREEN_NONE) {
    DISPLAY_ON;
    // TODO: restore to last screen
    showScreen(SCREEN_MAIN);
  } else {
    currentScreen = SCREEN_NONE;
    DISPLAY_OFF;
  }
}

void showScreen(Screen screen) {
  switch (screen) {
  case SCREEN_SPLASH:
    showSplashScreen();
    break;
  case SCREEN_MAIN:
    showMainScreen();
    break;
  }
}

void renderCurrentScreen(void) {
  if (currentScreen == SCREEN_MAIN) {

    renderMainScreen();
  }
}

// Lookup from ASCII to background tile index
static const uint8_t asciiLookup[] = {
    // non printable chars
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    //
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    // !  "  #  $  %  &  '  (  )  *  +  ,  -   .  /
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0,

    // 0    ->                  9  :  ;  <  =  >  ?
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0, 0, 0, 0, 0, 0,

    // UPPERCASE
    // @, then A -> O
    0, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    // P - Z                                    [  \  ]   ^  _
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 0, 0, 0, 64, 0,

    // lowercase
    // `, then a -> o
    0, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    // p - z  (lowecase)                        {  |  }  ~ Del
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 0, 0, 0, 0, 0,
    //
};

void printString(uint8_t x, uint8_t y, uint8_t len, uint8_t chars[]) {
  for (uint8_t i = 0; i < len; i++) {
    // TODO: This is pretty heavy, if we rearrange the bkg tile map font to be
    // ASCII ordered, then it will only require a simple add/subtract operation.
    bkg[(uint8_t)i] = asciiLookup[chars[i]];
  }

  set_bkg_tiles(x, y, len, 1, bkg);
}

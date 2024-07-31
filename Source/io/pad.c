#include "pad.h"
#include "../mGB.h"
#include "../screen/main.h"
#include "../screen/screen.h"
#include "../synth/common.h"
#include "../synth/pulse.h"
#include "../synth/wav.h"

uint8_t joyState;

// moved the main pad handler out, split into screen level handlers
// Before: 1770/1628 clock cycles, 379 bytes
// After: getPad: 194 clock cycles, 46 bytes
// getPadMainScreen: 418/387 clock cycles, 97 bytes
//        = 612/581, 143 bytes
void getPad(void) {
  i = joypad();
  if (i != joyState) {
    joyState = i;

    if ((i & J_A) && (i & J_SELECT)) {
      toggleScreen();
      return;
    }

    if (i == J_START) {
      stopAllSynths();
      return;
    }

    switch (currentScreen) {
    case SCREEN_MAIN:
      getPadMainScreen();
      break;
    }
  }
}

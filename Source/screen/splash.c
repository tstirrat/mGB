#include "splash.h"
#include "main.h"
#include "utils.h"

// TODO: more into res/logo.png and convert in build step
const uint8_t logo[] = {70, 72, 74, 0,  0,  0,  0,  0,
                        71, 73, 75, 76, 77, 78, 79, 80};

void showSplashScreen(void) {
  hideCursor();
  cls();
  set_bkg_tiles(6, 7, 8, 2, logo);
}

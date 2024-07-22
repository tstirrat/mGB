#include <gbdk/emu_debug.h>
#include <gbdk/platform.h>

#include "../Source/screen/splash.c"
#include "../Source/screen/utils.c"

uint8_t j;

void main(void) {

  displaySetup();

  showSplashScreen();
  delay(100);

  EMU_BREAKPOINT;
}

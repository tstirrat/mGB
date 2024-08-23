#include "mGB.h"
#include "io/midi.h"
#include "io/pad.h"
#include "io/sram.h"
#include "screen/main.h"
#include "screen/screen.h"
#include "screen/splash.h"
#include "synth/common.h"
#include "synth/noi.h"
#include "synth/pulse.h"
#include "synth/wav.h"

uint8_t i;
uint8_t x;
uint8_t j;
uint8_t l;
bool systemIdle = true;

void setSoundDefaults(void) {
  rAUDENA = AUDENA_ON; // Turn sound on
  rAUDVOL = AUDVOL_VOL_LEFT(7U) | AUDVOL_VOL_RIGHT(7U);

  setOutputPan(PU1, 64U);
  setOutputPan(PU2, 64U);
  setOutputPan(WAV, 64U);
  setOutputPan(NOI, 64U);

  loadWav(wavDataOffset); // tRIANGLE
  rAUD3LEVEL = 0x00U;

  rAUD4GO = 0x80U;
  rAUD4LEN = 0x3FU; // sound length
}

void testSynths(void) {
  addressByte = 0x40;
  valueByte = 0x7F;
  playNotePu1();
}

void main(void) {

  CRITICAL {
    cpu_fast();
    initMemory();
    displaySetup();
    initMainScreen();
    setSoundDefaults();
    add_TIM(updateSynths);

    loadDataSet(PU1);
    loadDataSet(PU2);
    loadDataSet(WAV);
    loadDataSet(NOI);
  }

  /* Set TMA to divide clock by 0x100 */
  rTMA = 0x00U;
  /* Set clock to 262144 Hertz */
  rTAC = TACF_START | TACF_262KHZ;
  /* Handle VBL and TIM interrupts */
  set_interrupts(VBL_IFLAG | TIM_IFLAG | SIO_IFLAG);

  showScreen(SCREEN_SPLASH);
  delay(2000);

  showScreen(SCREEN_MAIN);
  printVersion();
  // testSynths();
  gameMain();
}

inline void gameMain(void) {
  rSC = SIOF_XFER_START | SIOF_CLOCK_EXT;
  while (1) {
    systemIdle = true;

    updateMidiBuffer();

    if (systemIdle)
      getPad();

    if (systemIdle)
      updatePu1();

    if (systemIdle)
      updatePu2();

    if (systemIdle)
      updateWav();

    if (systemIdle)
      updateNoi();

    if (systemIdle)
      renderCurrentScreen();
  }
}

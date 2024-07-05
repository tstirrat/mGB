#include "mGB.h"
#include <gb/isr.h>

void serial_isr(void) CRITICAL INTERRUPT {
  serialBufferPosition++; // unsigned overflow from 255 -> 0 is automatic.
  serialBuffer[serialBufferPosition] = rSB;

  // queue another read
  rSC = SIOF_XFER_START | SIOF_CLOCK_EXT;
}

ISR_VECTOR(VECTOR_SERIAL, serial_isr)
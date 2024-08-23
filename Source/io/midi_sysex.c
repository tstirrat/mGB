#include "midi_sysex.h"
#include "../mGB.h"

uint8_t sysexBytesCount;
sysex_payload_t sysexPayload;
uint8_t sysexBuffer[24];

// adapted from
// https://github.com/FortySevenEffects/arduino_midi_library/blob/2d64cc3c2ff85bbee654a7054e36c59694d8d8e4/src/MIDI.cpp#L87
// (MIT licensed)
// Reduced parameter count to improve perf

/*!
 \brief Decode System Exclusive messages.

 SysEx messages are encoded to guarantee transmission of data bytes higher
 than 127 without breaking the MIDI protocol. Use this static method to
 reassemble your received message.

 \param outData The output buffer where to
 store the decrypted message.
 \param inLength The length of the input
 buffer.
 \return The length of the output buffer.
 @see encodeSysEx @see getSysExArrayLength
 Code inspired from Ruin & Wesen's SysEx encoder/decoder - http://ruinwesen.com
 */
static uint8_t decodeSysEx(uint8_t *outData, uint8_t inLength) {
  uint8_t count = 0;
  uint8_t msbStorage = 0;
  uint8_t byteIndex = 0;

  for (uint8_t i = 0; i < inLength; ++i) {
    if ((i % 8) == 0) {
      msbStorage = sysexBuffer[i];
      byteIndex = 6;
    } else {
      const uint8_t body = sysexBuffer[i];
      const uint8_t msb = (((msbStorage >> byteIndex) & 1) << 7);
      byteIndex--;
      outData[count++] = msb | body;
    }
  }
  return count;
}

void captureSysexByte(uint8_t byte) {
  sysexBytesCount++;
  systemIdle = false;

  const uint8_t bufferIndex = sysexBytesCount - SYSEX_HEADER_SIZE;

  if (byte == SYSEX_EOF) {
    // EMU_printf("sysex EOF: %#02x\n", byte);
    sysexPayload.size = decodeSysEx(&sysexPayload.data[0], bufferIndex);
    sysexPayload.ready = true;
    sysexBytesCount = 0;
    return;
  }

  switch (sysexBytesCount) {
  case 1:
    // EMU_printf("sysex status byte: %#02x\n", byte);
    // ignored in payload
    break;
  case 2:
    // EMU_printf("sysex id: %#02x\n", byte);
    sysexPayload.id = byte;
    break;
  case 3:
    // EMU_printf("sysex device_id: %#02x\n", byte);
    sysexPayload.device_id = byte;
    break;
  case 4:
    // EMU_printf("sysex channel: %#02x\n", byte);
    sysexPayload.channel = byte;
    break;

  default:
    // EMU_printf("sysex data: %#02x\n", byte);
    sysexBuffer[bufferIndex] = byte;
    break;
  }
}

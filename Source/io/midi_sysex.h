#pragma once

#include <gbdk/platform.h>
#include <stdbool.h>

#define MIDI_STATUS_SYSEX 0xF0
#define SYSEX_UNIVERSAL_REALTIME 0x7F
#define SYSEX_UNIVERSAL_NON_REALTIME 0x7E
#define SYSEX_NON_COMMERCIAL 0x7D
#define SYSEX_EOF 0xF7

#define SYSEX_MGB_ID 0x69

// The size of the SysEx message header before the payload begins
#define SYSEX_HEADER_SIZE 5

extern uint8_t sysexBytesCount;

typedef struct sysex_payload {
  // the sysex message id (or manufacturer id)
  uint8_t id;
  // the payload device id (0xBB for mGB)
  uint8_t device_id;
  // the device or channel id
  uint8_t channel;
  // the size of the data
  uint8_t size;
  // the data payload (24 raw sysex bytes = 21 (3*7) + 3 header blocks)
  uint8_t data[21];
  // true if the payload has finished reading from the buffer (has seen an EOF)
  bool ready;
} sysex_payload_t;

// A SysEx payload, captures up to 16 bytes of sysex data. Currently this is
// used to modify the active WAV channel waveform.
extern sysex_payload_t sysexPayload;

// The buffer to hold the sysexBytes before decoding
// contains enough space to hold 16 bytes = MSB, 7 bytes, MSB, 7 bytes, MSB, 2
// bytes = 19 bytes
extern uint8_t sysexBuffer[24];

// Reads SysEx messages (F0) up until a SysEx EOF packet
// (F7). Data is stored in `sysexPayload` and will set `sysexPayload.ready` when
// it sees the EOF
void captureSysexByte(uint8_t byte);

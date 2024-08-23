#pragma once

#include <gbdk/platform.h>

// the position in SRAM where the Version sentinel sits
#define SRAM_SENTINEL_INDEX 512

// SRAM sentinel value to determine if anything is initialized
#define SRAM_INITIALIZED 0xF7

// 512 + sentinel
extern volatile uint8_t saveData[513];

// SRAM version, so that we can determine if a partial migration/init is needed
extern volatile uint8_t sram_version;

// SRAM stored wav data, can be modified at runtime
extern volatile uint8_t sram_wavData[256];

void saveDataSet(uint8_t synth);
void loadDataSet(uint8_t synth);

// initialises the SRAM
void initMemory(void);

// Copies a wave from `wavData` into SRAM stored `sram_wavData`
void sramStoreWaveform(uint8_t index);

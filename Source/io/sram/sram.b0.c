/// @file Bank 0 SRAM structure

#include <gbdk/platform.h>

uint8_t saveData[513];

// SRAM version
uint8_t sram_version;

// SRAM stored wav data, can be modified at runtime
uint8_t sram_wavData[256];

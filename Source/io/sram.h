#pragma once

#include <gbdk/platform.h>

#define SRAM_INITIALIZED 0xF7

// 512 + sentinel
extern uint8_t saveData[513U];

void saveDataSet(uint8_t synth);
void loadDataSet(uint8_t synth);
void checkMemory(void);
#include "sram.h"
#include "../mGB.h"
#include "../screen/main.h"
#include "../synth/data.h"
#include "../synth/wav.h"
#include <gbdk/platform.h>
#include <string.h>

void saveDataSet(uint8_t synth) {
  // EMU_printf("saveDataSet(synth %d)\n", synth);

  ENABLE_RAM_MBC1;
  x = (synth + 24U);
  x = dataSet[x] * 8;
  i = 0;
  switch (synth) {
  case 0:
    for (j = 0; j != 7; j++) {
      saveData[x + i] = dataSet[j];
      i++;
    }
    break;
  case 1:
    for (j = 7; j != 13; j++) {
      saveData[128U + x + i] = dataSet[j];
      i++;
    }
    break;
  case 2:
    for (j = 13; j != 20; j++) {
      saveData[256U + x + i] = dataSet[j];
      i++;
    }
    break;
  case 3:
    for (j = 20; j != 24; j++) {
      saveData[384U + x + i] = dataSet[j];
      i++;
    }
    break;
  }
  DISABLE_RAM_MBC1;
}

void loadDataSet(uint8_t synth) {
  // EMU_printf("loadDataSet(synth %d)\n", synth);

  ENABLE_RAM_MBC1;
  x = dataSet[(synth + 24U)] * 8U;
  i = 0;
  switch (synth) {
  case 0U:
    for (j = 0; j != 7; j++) {
      if (!parameterLock[j])
        dataSet[j] = saveData[x + i];
      i++;
    }
    break;
  case 1U:
    for (j = 7; j != 13; j++) {
      if (!parameterLock[j])
        dataSet[j] = saveData[128U + x + i];
      i++;
    }
    break;
  case 2U:
    for (j = 13; j != 20; j++) {
      if (!parameterLock[j])
        dataSet[j] = saveData[256U + x + i];
      i++;
    }
    break;
  case 3U:
    j = 20;
    for (j = 20; j != 24; j++) {
      if (!parameterLock[j])
        dataSet[j] = saveData[384U + x + i];
      i++;
    }
    break;
  }
  DISABLE_RAM_MBC1;
}

// Init V1 SRAM data (`saveData`)
static void initV1(void) {
  for (x = 0; x != 128; x += 8) {
    l = 0;
    for (i = 0; i < 7; i++) {
      saveData[(x + l)] = dataSet[i];
      l++;
    }
    l = 0;
    for (i = 7; i != 13; i++) {
      saveData[(x + 128U + l)] = dataSet[i];
      l++;
    }
    l = 0;
    for (i = 13; i != 20; i++) {
      saveData[(x + 256U + l)] = dataSet[i];
      l++;
    }
    l = 0;
    for (i = 20; i != 24; i++) {
      saveData[(x + 384U + l)] = dataSet[i];
      l++;
    }
  }
  saveData[SRAM_SENTINEL_INDEX] = SRAM_INITIALIZED;
  sram_version = 1;
}

// Init V2 SRAM data (`sram_wavData`)
static void initV2(void) {
  memcpy(&sram_wavData, &wavData, sizeof(sram_wavData));
  sram_version = 2;
}

// load the SRAM `sram_wavData` data into the runtime `wavData`
static void sramLoadAllWavData(void) {
  memcpy(&wavData, &sram_wavData, sizeof(wavData));
}

void initMemory(void) {
  ENABLE_RAM_MBC1;

  // fixes some SRAM Bugs
  SWITCH_ROM(1);
  SWITCH_RAM(0);
  // end fix

  if (sram_version == 2) {
    // do nothing
  } else if (saveData[SRAM_SENTINEL_INDEX] == SRAM_INITIALIZED) {
    // legacy SRAM format (== V1)
    initV2();
  } else {
    // no SRAM initialized
    saveData[SRAM_SENTINEL_INDEX] = SRAM_INITIALIZED;
    initV1();
    initV2();
  }

  sramLoadAllWavData();
  DISABLE_RAM_MBC1;

  for (j = 0; j != 24; j++)
    dataSetSnap[j] = dataSet[j];
}

void sramStoreWaveform(uint8_t index) {
  const uint8_t offset = index * WAVEFORM_LENGTH;

  ENABLE_RAM_MBC1;
  memcpy(&sram_wavData[offset], &wavData[offset], WAVEFORM_LENGTH);
  DISABLE_RAM_MBC1;
}

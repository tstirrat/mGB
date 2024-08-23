// Script to generate the freq[] array

function noteToFreq(note) {
  const a = 440; // frequency of A (common value is 440Hz)
  return (a / 32) * 2 ** ((note - 9) / 12);
}

const noteFreqs = new Array(128).fill(0).map((_, i) => noteToFreq(i));

// console.log(noteFreqs);

// The pulse channels’ period dividers are clocked at 1048576 Hz, once per four
// dots, and their waveform is 8 samples long. This makes their sample rate
// equal to `1048576 / (2048 - period_value)` Hz.
//
// with a resulting tone frequency equal to `131072 / (2048 - period_value)` Hz.

const SAMPLE_SIZE = 2048;

// p = (131072 / f) - SAMPLE_SIZE

// Period value $500 means -$300, or 1 sample per 768 input cycles
// or (1048576 ÷ 768) = 1365.3 Hz sample rate
// or (1048576 ÷ 768 ÷ 8) = 170.67 Hz tone frequency
//
// -1279 = (131072 / 170.67) - 2048
// -1297.34 = (131072 / 174.61) - 2048 // -> ("F3" == 53)

// Period value $740 means -$C0, or 1 sample per 192 input cycles
// or (1048576 ÷ 192) = 5461.3 Hz sample rate
// or (1048576 ÷ 192 ÷ 8) = 682.67 Hz tone frequency

const pulsePeriods = noteFreqs
  // .map((f) => parseFloat(f.toFixed(2)))
  .map((f) => SAMPLE_SIZE - 131072 / f)
  .map(Math.floor);

// console.log({ freq: noteFreqs[36], period: pulsePeriods[36] });
// console.log({ freq: noteFreqs[37], period: pulsePeriods[37] });
// console.log({ freq: noteFreqs[108], period: pulsePeriods[108] });

// console.log(pulsePeriods.slice(36, 36 + 72));
console.log(pulsePeriods);

// const wavPeriods = noteFreqs
//   .map((f) => SAMPLE_SIZE - 65536 / f)
//   .map(Math.round);

// console.log(wavPeriods);

import pytest
from array import array

from bgb_get_snapshot import get


def describe_wav_loading():

    @pytest.mark.parametrize('snapshot', ['build/wav_test_load_and_play.gb'], indirect=True)
    def it_loads_the_correct_waveform(snapshot):
        _AUD3WAVERAM = get(snapshot, 'IO_REG', '__AUD3WAVERAM', 16)

        expected = array('B')
        expected.frombytes(bytearray([0x22, 0x55, 0x77, 0xAA, 0xBB, 0xDD, 0xEE, 0xFF,
                                      0xEE, 0xDD, 0xBB, 0xAA, 0x77, 0x66, 0x44, 0x00]))
        assert _AUD3WAVERAM == expected

    @pytest.mark.parametrize('snapshot', ['build/wav_test_load_and_play.gb'], indirect=True)
    def it_plays_a_note(snapshot):
        rAUD3LOW = get(snapshot, 'IO_REG', '_NR33_REG')
        rAUD3HIGH = get(snapshot, 'IO_REG', '_NR34_REG')

        # noteIndex = 64 - 24 == 40
        # freq[noteIndex] == 1849U == 0x0739

        retrig = 0x80
        assert rAUD3LOW == 0x39
        assert rAUD3HIGH == 0x07 | retrig

    # TODO: check frequency, env, vol etc

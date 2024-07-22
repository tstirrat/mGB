import pytest

from bgb_get_snapshot import get


def describe_pu1():

    @pytest.mark.parametrize('snapshot', ['build/pu1_plays_note.gb'], indirect=True)
    def it_plays_a_note(snapshot):
        rAUD1LOW = get(snapshot, 'IO_REG', '_NR13_REG')
        rAUD1HIGH = get(snapshot, 'IO_REG', '_NR14_REG')

        # noteIndex = 64U - 36U == 28U
        # f = freq[noteIndex] == 1650U == 0x0672

        assert rAUD1LOW == 0x72
        retrig = 0x80
        assert rAUD1HIGH == 0x06 | retrig

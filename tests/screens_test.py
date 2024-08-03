import pytest

from bgb_get_snapshot import CHECKSCREEN


def describe_screens():

    @pytest.mark.parametrize('snapshot', ['build/splash_screen.gb'], indirect=True)
    def it_matches_snapshot(snapshot):
        assert CHECKSCREEN(snapshot, "splash_screen.png")

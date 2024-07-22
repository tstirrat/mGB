import pytest

from bgb_get_snapshot import load_rom_snapshot


@pytest.fixture
def snapshot(request):
    return load_rom_snapshot(request.param)

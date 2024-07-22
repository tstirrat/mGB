import subprocess
from array import array
import os
from pathlib import Path
from PIL import Image, ImageChops

from gbdk_unit_test.framework.BGB_toolkit import load_noi, read_bgb_snspshot

base_dir = os.path.dirname(os.path.realpath(__file__))
base_path = Path(base_dir)


def load_rom_snapshot(rom_relative):

    make_and_run(rom_relative)

    snapshot_file = base_path.joinpath(rom_relative).with_suffix('.sna')

    if not snapshot_file.is_file():
        raise Exception("Cannot load snapshot: " + snapshot_file)

    noi_file = base_path.joinpath(rom_relative).with_suffix('.noi')
    if not snapshot_file.is_file():
        raise Exception("Cannot load symbols: " + noi_file)

    screenshot = base_path.joinpath(rom_relative).with_suffix('.bmp')

    snapshot = read_bgb_snspshot(snapshot_file)
    symbols = load_noi(noi_file)
    symbols = {value: key for key, value in symbols.items()}

    snapshot['symbols'] = symbols
    snapshot['screenshot'] = screenshot

    return snapshot


def make_and_run(rom_relative):
    make_rom(rom_relative)

    rom_path_full = base_path.joinpath(rom_relative)
    screenshot_path = rom_path_full.with_suffix('.bmp')

    bgb_exe = ["wine", "../bgb/bgb64.exe"]

    # windows
    if os.name == 'nt':
        bgb_exe = ["../bgb/bgb64.exe"]

    cmd = [
        *bgb_exe,
        "-set \"DebugSrcBrk=1\"",
        "-autoexit",
        "-hf",
        "-stateonexit",
        # "-screenonexit", screenshot_path.absolute().as_posix(),
        rom_relative,
    ]

    print("executing bgb: ", cmd)
    my_env = os.environ.copy()
    my_env["WINEDEBUG"] = "fixme-all"

    subprocess.call(cmd,
                    cwd=base_dir,
                    env=my_env,
                    # stdout=subprocess.DEVNULL,
                    # stderr=subprocess.DEVNULL,
                    )

    print("bgb: finished")

    if not rom_path_full.with_suffix(".sna").is_file():
        raise Exception("Tried to run rom, failed " + rom_relative)


def make_rom(rom_relative):
    print("calling make", rom_relative)
    result = subprocess.call(
        ["make", rom_relative],
        cwd=base_dir,
        # stdout=subprocess.DEVNULL
    )

    print("make: finished", rom_relative)

    if result != 0:
        raise Exception("Rom make failed " + rom_relative)


# The following code is repurposed from unit_checker.py by untoxa (MIT License)

# WRAM = 49152
mem_map = {
    'VRAM': 0x8000,
    'WRAM': 0xC000,
    'OAM': 0xFE00,
    'IO_REG': 0xFF00,
    'HRAM': 0xFF80,
}


def symbol_addr(snapshot, symbol, base):
    if type(base) is str:
        base = mem_map[base.upper()]
    return snapshot['symbols'].get(symbol) - base


def get(snapshot, section, address, len=0):
    if isinstance(address, str):
        address = symbol_addr(snapshot, address, section)

    if len > 1:
        return snapshot[section][address:address + len]
    else:
        return snapshot[section][address]


def ASCIIZ(snapshot, section, address):
    ofs = address
    data = snapshot[section]
    fin = ofs
    while data[fin] != 0:
        fin += 1
    return str(data[ofs:fin], 'ascii') if fin - ofs > 0 else ''


def CHECKSCREEN(snapshot, file_name):
    image_one = Image.open(base_path.joinpath(file_name)).convert('RGB')
    image_two = Image.open(snapshot['screenshot']).convert('RGB')

    diff = ImageChops.difference(image_one, image_two)

    return (diff.getbbox() is None)


def find(input: dict | array, val: str | int, parent_key: str | None):
    if isinstance(input, array):
        for i, v in enumerate(input):
            if v == val:
                print("found", [parent_key, i])
                return [parent_key, i]
            elif isinstance(v, array):
                find(v, val, i)
        return

    for k, v in input.items():
        if v == val:
            print("found", [parent_key, k])
            return [parent_key, k]
        elif isinstance(v, array) or isinstance(v, dict):
            find(v, val, k)

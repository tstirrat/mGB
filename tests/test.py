import subprocess


result = subprocess.run([
    "wine",
    "../bgb/bgb.exe",
    "-autoexit",
    "-hf",
    "-stateonexit",
    # "-screenonexit", "build/wav_test_load_and_play.bmp",
    "-rom", "build/wav_test_load_and_play.gb"
])

print(result)

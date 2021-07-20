# cardeaduino

Microcontroller that makes discouraging sounds when a doorknob is touched (like racking the slide on a shotgun, or a dog growling). The original PCM library [1] only allowed playing a single sound once. My modified version that removes this restriction is in this repo.

I'll use an Arduino Uno for the first iteration, because the PCM library is currently AVR only.

Two very useful utilities for generating audio input for PCM are SoX [2] and wave2c [3]. The latter needed some tweaking so I'm including my version in this repo, named "wav2h" (tucked away in a subdir to prevent the Arduino IDE from trying to link it into the sketch).

Cardea is the "Roman goddess of door hinges and handles who prevented evil spirits from crossing thresholds" [4].

The output on Pin 11 is a 62.5 kHz square wave with amplitude signal modulation in 256 (2^8) discrete steps. It is an ugly signal, and its impedance is also quite high. If you connect a speaker directly across ground and Pin 11, the speaker, as well as your ear, does do low-pass filtering. However, even if you can't directly hear the ultrasonic carrier wave, it interferes with the speakers ability to produce fidelitous sound. A reasonable effort to improve the sound quality would therefore be to put a low-pass filter on Pin 11 (at 4 kHz, or just above) and to use a transistor for current amplification (to reduce the signal impedance to something closer to the input impedance of the speaker, which is typically 4 to 16 Ohms).

[1] https://github.com/damellis/PCM

[2] http://sox.sourceforge.net/

[3] http://thieumsweb.free.fr/english/gbacss.html

[4] https://www.brooklynmuseum.org/eascfa/dinner_party/heritage_floor/cardea

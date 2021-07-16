# cardeaduino

Microcontroller that makes discouraging sounds when a doorknob is touched. Depends on the PCM library [1].

I'll use an Arduino Uno for the first iteration, because PCM is currently AVR only.

Two very useful utilities for generating audio input for PCM are SoX [2] and wave2c [3]. The latter needed some tweaking so I'm including my version in this repo, named "wav2h" (tucked away in a subdir to prevent the Arduino IDE from trying to link it into the sketch).

Cardea is the "Roman goddess of door hinges and handles who prevented evil spirits from crossing thresholds" [4].

[1] https://github.com/damellis/PCM

[2] http://sox.sourceforge.net/

[3] http://thieumsweb.free.fr/english/gbacss.html

[4] https://www.brooklynmuseum.org/eascfa/dinner_party/heritage_floor/cardea

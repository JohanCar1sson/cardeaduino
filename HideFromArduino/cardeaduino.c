/* Microcontroller that makes discouraging sounds when a doorknob is touched
 * https://github.com/JohanCar1sson/cardeaduino
 * Copyright 2021, 2022 Johan Carlsson
 */

#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "PCM.h"

#define TESTING 1

#if TESTING

/* Avoid name clashes when multiple raudio headers are included by prefixing */
#define RAUDIO_PREFIX full_rez_
#include "sine1kHz8b.h"
#define RAUDIO_PREFIX half_rez_
#include "sine1kHz4b.h"
#define RAUDIO_PREFIX quarter_rez_
#include "sine1kHz2b.h"
#define RAUDIO_PREFIX eighth_rez_
#include "sine1kHz1b.h"

#else

#include "../shotgun.h"
/* Avoid name clashes when a second raudio header is included by prefixing */
#define RAUDIO_PREFIX dog_
#include "../dog.h"

#endif

void random_delay()
{
  unsigned char n, nsleep;

  /* nsleep = (unsigned char)(rand() >> 8); */ /* Use most-significant byte as an unsigned char */
  nsleep = (unsigned char)(rand() >> 12); /* Use the 3 most-significant bits to make PRN in range [0, 7] */
  for (n = 0; n <= nsleep; n++) _delay_ms(1000);

  return;
}

int main()
{
  unsigned char nloop = 0;

  pcm_init();
  srand(31337);

  while (1)
  {
#if TESTING
    pcm_play(full_rez_raudio_data, full_rez_raudio_length, full_rez_raudio_bitdepth); /* 8-bit */
    pcm_play(half_rez_raudio_data, half_rez_raudio_length, half_rez_raudio_bitdepth); /* 4-bit */
    pcm_play(quarter_rez_raudio_data, quarter_rez_raudio_length, quarter_rez_raudio_bitdepth); /* 2-bit */
    pcm_play(eighth_rez_raudio_data, eighth_rez_raudio_length, eighth_rez_raudio_bitdepth); /* 1-bit */
#else
    pcm_play(raudio_data, raudio_length, raudio_bitdepth); /* shotgun */
    random_delay(); /* Random wait to build suspense */
    pcm_play(dog_raudio_data, dog_raudio_length, dog_raudio_bitdepth); /* dog */
    random_delay();
#endif
    if (3 == ++nloop) pcm_final();
  }
}

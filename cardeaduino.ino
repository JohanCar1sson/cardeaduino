/* Microcontroller that makes discouraging sounds when a doorknob is touched
 * https://github.com/JohanCar1sson/cardeaduino
 * Copyright 2021, 2022 Johan Carlsson
 */

#include <stdlib.h>

#include "PCM.h"

#include "shotgun.h"
/* Avoid name clashes when a second raudio header is included by prefixing */
#define RAUDIO_PREFIX dog_
#include "dog.h"

//#include "lovecake.h"
//#include "duemilanove.h"

unsigned short nloop = 0;

void random_delay()
{
  unsigned char n, nsleep;

  /* nsleep = (unsigned char)(rand() >> 8); */ /* Use most-significant byte as an unsigned char */
  nsleep = (unsigned char)(rand() >> 12); /* Use the 3 most-significant bits to make PRN in range [0, 7] */
  for (n = 0; n <= nsleep; n++) _delay_ms(1000);

  return;
}

void setup()
{
  pcm_init();
  srand(31337);
}

void loop()
{
  pcm_play(raudio_data, raudio_length, raudio_bitdepth);
  random_delay(); /* Random wait to build suspense */
  pcm_play(dog_raudio_data, dog_raudio_length, dog_raudio_bitdepth);
  random_delay();

  if (3 == ++nloop) pcm_final();
}

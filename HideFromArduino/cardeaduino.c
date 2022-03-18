/* Microcontroller that makes discouraging sounds when a doorknob is touched
 * https://github.com/JohanCar1sson/cardeaduino
 */

#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "PCM.h"

#include "../sine1kHz.h"
/* #include "../duemilanove.h" */
/* #include "../shotgun.h" */
/* #include "../dog.h" */

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
    pcm_play(sine1kHz_data, 8000, 8);
    /* pcm_play(duemilanove_data, duemilanove_length, 8); */
#if 0
    random_delay(); /* Random wait to build suspense */
    pcm_play(shotgun_data, shotgun_length, 8);

    random_delay();
    pcm_play(dog_data, dog_length, 8);
#endif
    if (3 == ++nloop) pcm_final();
  }
}

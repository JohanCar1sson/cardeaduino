/* Microcontroller that makes discouraging sounds when a doorknob is touched
 * https://github.com/JohanCar1sson/cardeaduino
 */

#include <stdlib.h>

#include "PCM.h"

//#include "sine1kHz.h"
//#include "duemilanove.h"
#include "lovecake.h"
//#include "shotgun.h"
//#include "dog.h"

unsigned short nloop = 0;

void setup()
{
  pcm_init();
}

void loop()
{
  //pcm_play(sine1kHz_data, 8000);
  //pcm_play(duemilanove_data, duemilanove_length);
  pcm_play(lovecake_data, lovecake_length);
  //pcm_play(shotgun_data, shotgun_length);
  //pcm_play(dog_data, dog_length);

  if (3 == ++nloop) pcm_final();
}

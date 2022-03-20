/* A version of the PCM library [1] that was modified by Johan Carlsson
 * (@JohanCar1sson) to allow playing multiple sounds, or the same sound multiple
 * times. Support for bit-crushed audio has also been added (with bit depth
 * reduced to 4, 2 or 1).
 *
 * Copyright 2022 Johan Carlsson
 *
 * [1] https://github.com/damellis/PCM
 */

/*
 * speaker_pcm
 *
 * Plays 8-bit PCM audio on pin 11 using pulse-width modulation (PWM).
 * For Arduino with Atmega168 at 16 MHz.
 *
 * Uses two timers. The first changes the sample value 8000 times a second.
 * The second holds pin 11 high for 0-255 ticks out of a 256-tick cycle,
 * depending on sample value. The second timer repeats 62500 times per second
 * (16000000 / 256), much faster than the playback rate (8000 Hz), so
 * it almost sounds halfway decent, just really quiet on a PC speaker.
 *
 * Takes over Timer 1 (16-bit) for the 8000 Hz timer. This breaks PWM
 * (analogWrite()) for Arduino pins 9 and 10. Takes Timer 2 (8-bit)
 * for the pulse width modulation, breaking PWM for pins 11 & 3.
 *
 * References:
 *     http://www.uchobby.com/index.php/2007/11/11/arduino-sound-part-1/
 *     http://www.atmel.com/dyn/resources/prod_documents/doc2542.pdf
 *     http://www.evilmadscientist.com/article.php/avrdac
 *     http://gonium.net/md/2006/12/27/i-will-think-before-i-code/
 *     http://fly.cc.fer.hr/GDM/articles/sndmus/speaker2.html
 *     http://www.gamedev.net/reference/articles/article442.asp
 *
 * Michael Smith <michael@hurts.ca>
 */

#include <stdint.h>
#include <avr/interrupt.h>
/* #include <util/atomic.h> */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define SAMPLE_RATE 8000
/* #define _BV(bit) (1 << (bit)) */

const unsigned char *audio_data = 0;
unsigned int audio_length = 0;
unsigned char audio_bitdepth = 8;
volatile uint16_t sample;

/* This is called at 8000 Hz to load the next sample */
ISR(TIMER1_COMPA_vect)
{
  static unsigned char databyte;

  if (sample < audio_length) 
  {
#if 0
    databyte = pgm_read_byte(audio_data + (sample / audio_bitdepth)) >> (sample % (8 / audio_bitdepth));
    databyte <<= (8 - audio_bitdepth);
#else
    switch (audio_bitdepth)
    {
      case 1:
        databyte = pgm_read_byte(audio_data + (sample >> 3)) >> (sample % 8);
        databyte <<= 7;
        break;
      case 2:
        databyte = pgm_read_byte(audio_data + (sample >> 2)) >> 2 * (sample % 4);
        databyte <<= 6;
        break;
      case 4:
        databyte = pgm_read_byte(audio_data + (sample >> 1)) >> 4 * (sample % 2);
        databyte <<= 4;
        break;
      case 8:
        databyte = pgm_read_byte(audio_data + sample);
    }
#endif
    OCR2A = databyte;
    sample++;
  }
}

void pcm_init()
{
  /* pinMode(11, OUTPUT); */
  /* Set the 3rd bit of DDRB to one to make digital pin 11 (PB3 on Atmega328p) an output pin */
  DDRB |= _BV(PB3);

  sample = 0;

  /* Set up Timer 2 to do pulse width modulation on the speaker pin */

  /* Use internal clock (datasheet p.160) */
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));

  /* Set fast PWM mode  (p.157) */
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);

  /* Do non-inverting PWM on pin OC2A (p.155)
     On the Arduino this is pin 11 */
  TCCR2A = (TCCR2A | _BV(COM2A1)) & ~_BV(COM2A0);
  TCCR2A &= ~(_BV(COM2B1) | _BV(COM2B0));

  /* No prescaler (p.158) */
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

  /* Set up Timer 1 to send a sample every interrupt */
  cli();

  /* Set CTC mode (Clear Timer on Compare Match) (p.133)
     Have to set OCR1A *after*, otherwise it gets reset to 0! */
  TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
  TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));

  /* No prescaler (p.134) */
  TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

  /* Set the compare register (OCR1A).
     OCR1A is a 16-bit register, so we have to do this with
     interrupts disabled to be safe. */
  OCR1A = F_CPU / SAMPLE_RATE; /* 16e6 / 8000 = 2000 */

  /* Enable interrupt when TCNT1 == OCR1A (p.136) */
  TIMSK1 |= _BV(OCIE1A);

  sei();
}

void pcm_final()
{
  /* Disable playback per-sample interrupt */
  TIMSK1 &= ~_BV(OCIE1A);

  /* Disable the per-sample timer completely */
  TCCR1B &= ~_BV(CS10);

  /* Disable the PWM timer */
  TCCR2B &= ~_BV(CS10);

  /* digitalWrite(11, LOW); */
  /* Set the 3rd bit of PORTB to zero to make the voltage on pin 11 go low */
  PORTB &= ~_BV(PB3);
}

void pcm_play(const unsigned char *data, unsigned int nsamples, unsigned char nbitdepth)
{
  unsigned short n, nquarterseconds;
  unsigned char sregRestore = SREG;

  /* Temporarily turn off interrupts while writing to global variables used in ISR */
  /* ATOMIC_BLOCK(ATOMIC_RESTORESTATE) */
  cli(); /* Clear the global interrupt enable flag */
  {
    audio_data = data;
    audio_length = nsamples;
    audio_bitdepth = nbitdepth;
    sample = 0;
  }
  SREG = sregRestore; /* Restore the status register to its previous value */

  /* The sound is now playing asynchronously. Don't return until it's done! */

  /* Hint: nsamples >> 3 = nsamples / 8 = duration of sound in ms */
  nquarterseconds = nsamples >> 11;
  nquarterseconds++; /* Add one to have some margin */
  for (n = 0; n <= nquarterseconds; n++) _delay_ms(256);
}

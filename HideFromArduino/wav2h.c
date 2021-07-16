/* wav2h: converts mono (single channel), 8 kHz sample rate WAV file into a
 * data array in a header file that can be used by the Arduino PCM library.
 *
 * SoX can be used to produce the input audio file, for example:
 * sox Growling.mp3 -c 1 -r 8k -b 8 -e unsigned-integer dog.wav speed 0.9 norm
 *
 * Author: Johan Carlsson (who claims no copyright on his trivial changes)
 */

/* wave2c, a WAV file to GBA C source converter.
 * Copyright (c) 2003 by Mathieu Brethes.
 *
 * Contact : thieumsweb@free.fr
 * Website : http://thieumsweb.free.fr/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <stdio.h>

typedef struct
{
	int sampleRate;
	int numChannels;
	int bitsPerSample;
	int dataLength;
} wavSound;

/* Loads a wave header in memory, and checks for its validity. */
/* returns NULL on error, a malloced() wavSound* otherwise.    */
wavSound * loadWaveHeader(FILE * fp)
{
	char c[5];
	int nbRead;
	int chunkSize;
	int subChunk1Size;
	int subChunk2Size;
	short int audFormat;
	short int nbChannels;
	int sampleRate;
	int byteRate;
	short int blockAlign;
	short int bitsPerSample;
	wavSound *w;

	c[4] = 0;

	nbRead=fread(c, sizeof(char), 4, fp);
	
	/* EOF ? */
	if (nbRead < 4) return NULL;
	
	/* Not a RIFF ? */
	if (strcmp(c, "RIFF") != 0)
	{
		printf("Not a RIFF: %s\n", c);
		return NULL;
	}
	nbRead=fread(&chunkSize, sizeof(int), 1, fp);
	
	/* EOF ? */
	if (nbRead < 1) return NULL;

	nbRead=fread(c, sizeof(char), 4, fp);
	
	/* EOF ? */
	if (nbRead < 4) return NULL;
	
	/* Not a WAVE riff ? */
	if (strcmp(c, "WAVE") != 0)
	{
		printf("Not a WAVE: %s\n", c);
		return NULL;
	}
	nbRead=fread(c, sizeof(char), 4, fp);
	
	/* EOF ? */
	if (nbRead < 4) return NULL;
	
	/* Not a "fmt " subchunk ? */
	if (strcmp(c, "fmt ") != 0)
	{
		printf("No fmt subchunk: %s\n", c);
		return NULL;
	}
	/* read size of chunk. */
	nbRead=fread(&subChunk1Size, sizeof(int), 1, fp);
	if (nbRead < 1) return NULL;

	/* is it a PCM ? */
	if (subChunk1Size != 16)
	{
		printf("Not PCM fmt chunk size: %x\n", subChunk1Size);
		return NULL;
	}
	nbRead=fread(&audFormat, sizeof(short int), 1, fp);
	if (nbRead < 1) return NULL;

	/* is it PCM ? */
	if (audFormat != 1)
	{
		printf("No PCM format (1): %x\n", audFormat);
		return NULL;
	}
	nbRead=fread(&nbChannels, sizeof(short int), 1, fp);
	if (nbRead < 1) return NULL;

	/* is it mono ? */
	if (nbChannels != 1)
	{
		printf("Number of channels invalid: %x (must be mono)\n", nbChannels);
		return NULL;
	}
	nbRead=fread(&sampleRate, sizeof(int), 1, fp);
	if (nbRead < 1) return NULL;

	/* is the sample rate 8 kHz ? */
	if (sampleRate != 8000)
	{
		printf("The sample rate is invalid: %x (must be 8 kHz)\n", sampleRate);
		return NULL;
	}
	nbRead=fread(&byteRate, sizeof(int), 1, fp);
	if (nbRead < 1) return NULL;

	nbRead=fread(&blockAlign, sizeof(short int), 1, fp);
	if (nbRead < 1) return NULL;

	nbRead=fread(&bitsPerSample, sizeof(short int), 1, fp);
	if (nbRead < 1) return NULL;
	
	nbRead=fread(c, sizeof(char), 4, fp);
	
	/* EOF ? */
	if (nbRead < 4) return NULL;
	
	/* Not a data section ? */
	if (strcmp(c, "data") != 0)
	{
		printf("Not a data subchunk: %s\n", c);
		return NULL;
	}
	nbRead=fread(&subChunk2Size, sizeof(int), 1, fp);
	if (nbRead < 1) return NULL;

	/* Now we can generate the structure... */

	w = (wavSound *) malloc(sizeof(wavSound));
	/* out of memory ? */
	if (w == NULL)
	{
		printf("Out of memory, sorry\n");	
		return w;
	}
	w->sampleRate = 8000 /* sampleRate */;
	w->numChannels = 1 /* nbChannels */;
	w->bitsPerSample = bitsPerSample;
	w->dataLength = subChunk2Size;

	return w;
}

/* Loads the actual wave data into the data structure. */
void saveWave(FILE * fpI, wavSound *s, FILE * fpO, char * name)
{
	long filepos;
	int i;
	int realLength;
	unsigned char stuff8;

	filepos = ftell(fpI);
	
	/* Print general information) */
	fprintf(fpO, "/* %s sound made by wav2h */\n\n", name);
	fprintf(fpO, "const int %s_sampleRate=%d;\n", name, s->sampleRate);

	realLength = (s->dataLength / s->numChannels / s->bitsPerSample * 8);

	fprintf(fpO, "const int %s_length=%d;\n\n", name, realLength);

	/* 8-bit ? */
	fprintf(fpO, "const unsigned char %s_data[] PROGMEM = {\n", name);
	if (s->bitsPerSample == 8)
	{
		for (i = 0 ; i < realLength ; i++)
		{
			fread(&stuff8, sizeof(unsigned char), 1, fpI);
			fprintf(fpO, "%3d%s", stuff8, (i < realLength - 1) ? ", " : "");
			if (i < realLength - 1 && (i + 1) % 16 == 0) fprintf(fpO, "\n");
		}
		fprintf(fpO, "};\n");
	/* 16-bit ? convert signed 16 to unsigned 8 */
	}
	else
	{
		for (i = 0 ; i < realLength ; i++)
		{
		    /* We take only MSB of wave data... */
		    /* Will this work on little-endian computers only? Johan 7/15/21 */
			fread(&stuff8, sizeof(char), 1, fpI);
			fread(&stuff8, sizeof(char), 1, fpI);
			fprintf(fpO, "%4d%s", 128 + (signed char)stuff8, (i < realLength - 1) ? ", " : "");
			if ((i % 12) == 0) fprintf(fpO, "\n");
		}
		fprintf(fpO, "};\n");
	}
}

int main(int argc, char *argv[])
{
	wavSound *s;
	FILE *fin;
	FILE *fout;
	int namelen, idot;
	char *name;

	if (argc != 2)
	{
		printf("Usage: ./%s <file.wav>\n", argv[0]);
		exit(0);
	}
    namelen = strlen(argv[1]);
    idot = namelen - 4;
    name = (char *)alloca(namelen);
    strcpy(name, argv[1]);
    /* printf("idot = %d, ->%s<- ->%s<-\n", idot, name, name + idot); exit(0); */
    if (strcmp(name + idot, ".wav"))
    {
		printf("The input file %s does not have the required \".wav\" suffix\n", argv[1]);
		exit(0);
    }
    name[idot] = '\0';
    /* printf("->%s<-\n", name); exit(0); */
	fin = fopen(argv[1], "r");

	if ((s = loadWaveHeader(fin)) == NULL)
	{
		printf("The input file %s does not have the correct format\n", argv[1]);
		exit(0);
	}
    strcat(name, ".h");
    /* printf("->%s<-\n", name); exit(0); */
	fout = fopen(name, "w");
	name[idot] = '\0';

	saveWave(fin, s, fout, name);

	return 0;	
}

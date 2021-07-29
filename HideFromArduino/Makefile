FILES = cardeaduino.c PCM.h PCH.c Makefile

.PHONY : all
all : cardeaduino.hex

cardeaduino.hex : cardeaduino
	avr-objcopy -O ihex -R .eeprom cardeaduino cardeaduino.hex

# Using avr-gcc to invoke the linker seems broken for gcc 11.1.0
# Here's an ugly workaround:
#cardeaduino : cardeaduino.o crtatmega328p.o
#	avr-gcc –std=c99 -mmcu=atmega328p cardeaduino.o -L/home/johan/local/avr/avr-libc-2.0.0/avr/lib -L/home/johan/local/avr/avr-libc-2.0.0/avr/lib/avr5 -o cardeaduino
#crtatmega328p.o :
#	ln -s /home/johan/local/avr/avr-libc-2.0.0/avr/lib/avr5/crtatmega328p.o .
# But it's probably better to link using collect2 directly:
cardeaduino : cardeaduino.o PCM.o
	/home/johan/local/avr/gcc-11.1.0/lib/gcc/avr/11.1.0/collect2 -plugin /home/johan/local/avr/gcc-11.1.0/lib/gcc/avr/11.1.0/liblto_plugin.so -plugin-opt=/home/johan/local/avr/gcc-11.1.0/lib/gcc/avr/11.1.0/lto-wrapper -plugin-opt=-fresolution=/tmp/ccSBEnaS.res -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lm -plugin-opt=-pass-through=-lc -plugin-opt=-pass-through=-latmega328p -mavr5 -Tdata 0x800100 -o cardeaduino /home/johan/local/avr/avr-libc-2.0.0/avr/lib/avr5/crtatmega328p.o -L/home/johan/local/avr/avr-libc-2.0.0/avr/lib -L/home/johan/local/avr/avr-libc-2.0.0/avr/lib/avr5 -L/home/johan/local/avr/gcc-11.1.0/lib64/gcc/avr/11.1.0 cardeaduino.o PCM.o --start-group -lgcc -lm -lc -latmega328p --end-group

cardeaduino.o : cardeaduino.c
	avr-gcc -c cardeaduino.c -I.. -I/home/johan/local/avr/avr-libc-2.0.0/avr/include -Os -DF_CPU=16000000UL -mmcu=atmega328p -o cardeaduino.o

PCM.o : ../PCM.c
	avr-gcc -c ../PCM.c -I/home/johan/local/avr/avr-libc-2.0.0/avr/include -Os -DF_CPU=16000000UL -mmcu=atmega328p -o PCM.o

cardeaduino.tgz : $(FILES)
	tar cfvz cardeaduino.tgz $(FILES)

.PHONY : flash
flash : cardeaduino.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 115200 -U flash:w:cardeaduino.hex
# avrdude -v -patmega328p -carduino -P/dev/ttyUSB0 -b115200 -D -Uflash:w:cardeaduino.hex:i

.PHONY : sizecheck
sizecheck :
	avr-size cardeaduino

.PHONY : dist
dist : cardeaduino.tgz

.PHONY : linecount
linecount :
	wc -l $(FILES)

.PHONY : clean
clean :
	rm -f cardeaduino.hex cardeaduino *.o

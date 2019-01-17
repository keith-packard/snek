vpath %.c .:..
vpath %.h .:..

INC = \
	../newt.h \
	newt-builtin.h \
	newt-duino.h \
	newt-gram.h

NEWT_SRC = \
	newt-builtin.c \
	newt-code.c \
	newt-error.c \
	newt-for.c \
	newt-frame.c \
	newt-func.c \
	newt-list.c \
	newt-lex.c \
	newt-memory.c \
	newt-name.c \
	newt-parse.c \
	newt-poly.c \
	newt-string.c

SRC = \
	newt-duino.c $(NEWT_SRC)

OBJ = $(SRC:.c=.o)

CC=avr-gcc
CFLAGS=-Os -DF_CPU=16000000UL -mmcu=atmega328p -I. -I..
LDFLAGS=-Wl,-uvfprintf -lprintf_flt -lm \
	-Wl,--defsym -Wl,__TEXT_REGION_LENGTH__=0x8000 \
	-Wl,--defsym -Wl,__DATA_REGION_LENGTH__=0x7c0 \
	-Wl,--defsym -Wl,__EEPROM_REGION_LENGTH__=0x400 \
	-Wl,-Map=newt-duino.map

all: newt-duino.hex

load: newt-duino.hex
	avrdude -F -V -c usbtiny -p ATMEGA328P -U flash:w:newt-duino.hex

newt-duino.hex: newt-duino
	avr-objcopy -O ihex -R .eeprom newt-duino $@

newt-duino: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

BUILTINS = \
	../newt-keyword.builtin \
	../newt-base.builtin \
	newt-duino.builtin

newt-builtin.h: ../newt-builtin.py $(BUILTINS)
	python3 ../newt-builtin.py -o $@ $(BUILTINS)

newt-gram.h: ../newt-gram.ll
	lola -o $@ ../newt-gram.ll

$(OBJ): $(INC)

clean:
	rm -f $(OBJ) newt-gram.h newt-builtin.h newt-duino newt-duino.hex

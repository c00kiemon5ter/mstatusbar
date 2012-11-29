# Makefile for mstatusbar - see LICENSE for license and copyright information

PREFIX ?= /usr/local
BINDIR ?= ${PREFIX}/bin
MANPREFIX = ${PREFIX}/share/man

ALSALIB = -lasound
MPDLIB  = -lmpdclient

INCS = -I. -I/usr/include
LIBS = -L/usr/lib -lc ${ALSALIB}

XINERAMA ?= 0
MPD		 ?= 0
DDBF	 ?= 0
BATT	 ?= 0

CFLAGS   = -std=c99 ${INCS}
CFLAGS  += -DXINERAMA=${XINERAMA} -DMPD=${MPD} -DDDBF=${DDBF} -DBATT=${BATT}
LDFLAGS  = ${LIBS}

ifneq "$(MPD)" "0"
	LIBS += ${MPDLIB}
endif

CC 	 = cc
EXEC = mstatusbar

SRC = ${EXEC}.c
OBJ = ${SRC:.c=.o}

all: CFLAGS += -Os
all: LDFLAGS += -s
all: mstatusbar

debug: CFLAGS += -O0 -g -pedantic -Wall -Wextra
debug: options mstatusbar

options:
	@echo mstatusbar build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.h

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

mstatusbar: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -fv ${EXEC} ${OBJ}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@install -Dm755 ${EXEC} ${DESTDIR}${PREFIX}/bin/${EXEC}

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/${EXEC}

.PHONY: all options clean install uninstall

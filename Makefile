# Makefile for mstatusbar - see LICENSE for license and copyright information

PREFIX ?= /usr/local
BINDIR ?= ${PREFIX}/bin
MANPREFIX = ${PREFIX}/share/man

ALSALIB = -lasound
MPDLIB  = -lmpdclient

INCS = -I. -I/usr/include
LIBS = -L/usr/lib -lc ${ALSALIB} ${MPDLIB}

XINERAMA ?= 0

CFLAGS   = -std=c99 -pedantic -Wall -Wextra -DXINERAMA=${XINERAMA}
LDFLAGS  = ${LIBS}

CC 	 = cc
EXEC = mstatusbar

SRC = ${EXEC}.c
OBJ = ${SRC:.c=.o}

all: CFLAGS += -Os
all: LDFLAGS += -s
all: mstatusbar

debug: CFLAGS += -O0 -g
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

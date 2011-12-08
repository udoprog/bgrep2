PREFIX=/usr/local

SOURCES+=bgrep.c
SOURCES+=result_list.c
SOURCES+=hexutils.c

CFLAGS=-Wall -pedantic -std=c99
LDFLAGS=

OBJECTS=${SOURCES:.c=.o}

all: bgrep

install: bgrep
	install -m 755 bgrep ${PREFIX}/bin

clean:
	${RM} ${OBJECTS}
	${RM} bgrep

bgrep: ${OBJECTS}
	${CC} ${CFLAGS} ${LDFLAGS} ${OBJECTS} -g -o $@

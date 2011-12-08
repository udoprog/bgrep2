SOURCES+=bgrep.c
SOURCES+=result_list.c
SOURCES+=hexutils.c

CFLAGS=-Wall -pedantic -std=c99
LDFLAGS=

OBJECTS=${SOURCES:.c=.o}

all: bgrep

clean:
	${RM} ${OBJECTS}
	${RM} bgrep

bgrep: ${OBJECTS}
	${CC} ${CFLAGS} ${LDFLAGS} ${OBJECTS} -g -o $@

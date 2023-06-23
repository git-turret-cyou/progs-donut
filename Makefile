include config.mk

SRC = donut.c
OBJ = ${SRC:.c=.o}

all: options donut

options:
	@echo donut build options:
	@echo "CFLAGS = ${CFLAGS}"
	@echo "LDFLAGS = ${LDFLAGS}"
	@echo "CC = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

config.h:
	cp config.def.h $@

donut: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f donut ${OBJ}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f donut ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/donut

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/donut

.PHONY: all options clean install uninstall

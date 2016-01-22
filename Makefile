# snore - simple executor
# See LICENSE file for copyright and license details.

include config.mk

SRC = snore.c
OBJ = ${SRC:.c=.o}

all: options snore

options:
	@echo snore build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

snore: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f snore ${OBJ} snore-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p snore-${VERSION}
	@cp -R LICENSE Makefile README config.mk \
		snore.1 ${SRC} snore-${VERSION}
	@tar -cf snore-${VERSION}.tar snore-${VERSION}
	@gzip snore-${VERSION}.tar
	@rm -rf snore-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f snore ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/snore
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < snore.1 > ${DESTDIR}${MANPREFIX}/man1/snore.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/snore.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/snore
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/snore.1

.PHONY: all options clean dist install uninstall

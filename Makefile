# snore - sleep with feedback
# See LICENSE file for copyright and license details.

include config.mk

APPNAME=snore
SRC = ${APPNAME}.c
OBJ = ${SRC:.c=.o}

all: options ${APPNAME}

options:
	@echo ${APPNAME} build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

${APPNAME}: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f ${APPNAME} ${OBJ} ${APPNAME}-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p ${APPNAME}-${VERSION}
	@cp -R LICENSE Makefile README config.mk \
		${APPNAME}.1 ${SRC} ${APPNAME}-${VERSION}
	@tar -cf ${APPNAME}-${VERSION}.tar ${APPNAME}-${VERSION}
	@gzip ${APPNAME}-${VERSION}.tar
	@rm -rf ${APPNAME}-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f ${APPNAME} ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/${APPNAME}
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < ${APPNAME}.1 > ${DESTDIR}${MANPREFIX}/man1/${APPNAME}.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/${APPNAME}.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/${APPNAME}
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/${APPNAME}.1

.PHONY: all options clean dist install uninstall

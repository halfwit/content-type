# content-type - return the content type of a url
# See LICENSE file for copyright and license details.

SRC = content-type.c
OBJ = ${SRC:.c=.o}
LDFLAGS = -lcurl

all: content-type

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

content-type: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f content-type ${OBJ}

install: all
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f content-type ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/content-type
uninstall:
	@rm -f ${DESTDIR}${PREFIX}/bin/content-type

.PHONY: all clean install uninstall

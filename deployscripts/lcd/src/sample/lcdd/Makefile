STD				= -std=gnu99 -D_GNU_SOURCE -D_REENTRANT -DEZIO_300_CONFIG
PREFIX			= /usr/local
CC				= gcc
RANLIB			= ranlib
CFLAGS			= -g -O2
LDFLAGS			= 

SOURCES			= main/lcdd.c ../../lcd_ctrl.c
OBJS			= main/lcdd.o ../../lcd_ctrl.o
PROGS			= bin/lcdd
INCLUDES		= -I../../../ -I./include
LIBS			= 

DESTDIR			= $(PREFIX)

all: $(PROGS)

MAKEDIR = || exec false; \
	if test -d "$$dir"; then :; else \
		echo MD "$$dir"; \
		mkdir -p "$$dir" > /dev/null 2>&1 || \
		test -d "$$dir" || \
		mkdir "$$dir" || \
		exec false; \
	fi || exec false

.c.o:
	@echo CC $*.c
	@$(CC) $(CFLAGS) -c $(INCLUDES) $(STD) $< -o $@

bin/.dummy:
	@if (: >> $@ || : > $@) > /dev/null 2>&1; then :; else \
		dir=bin $(MAKEDIR); fi
	@: >> $@ || : > $@

bin/lcdd: $(OBJS) bin/.dummy
	@echo LO lcdd
	@$(CC) $(OBJS) $(LIBS) $(INCLUDES) -o bin/lcdd

clean:
	@echo RM $(OBJS) bin
	@rm -rf $(OBJS) bin

distclean: clean
	@echo RM config.log config.status include/config.h Makefile config.cache \
				tags autom4te.cache
	@rm -rf config.log config.status include/config.h Makefile config.cache \
				tags autom4te.cache

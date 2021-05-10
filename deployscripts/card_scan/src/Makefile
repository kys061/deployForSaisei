STD = -std=gnu99 -D_GNU_SOURCE -D_REENTRANT
CC = gcc
CFLAGS =
SOURCES = util/card_scan.c util/card_name_set.c util/ip.c util/network_rename.c util/slot_name_set.c util/dev_access.c util/free.c util/mb_info_load.c util/slot_ports_table_make.c util/net_table.c util/command_parse.c util/show.c
OBJS = util/card_scan.o util/card_name_set.o util/ip.o util/network_rename.o util/slot_name_set.o util/dev_access.o util/free.o util/mb_info_load.o util/slot_ports_table_make.o util/net_table.o util/command_parse.o util/show.o
PROGS = bin/card_scan
INCLUDES = -I ./include

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
	@$(CC) $(CFLAGS) -c -Wall -Werror $(INCLUDES) $(STD) $< -o $@

bin/.dummy:
	@if (: >> $@ || : > $@) > /dev/null 2>&1; then :; else \
		dir=bin $(MAKEDIR); fi
	@: >> $@ || : > $@

bin/card_scan: $(OBJS) bin/.dummy
	@echo LO card_scan
	@$(CC) $(OBJS) $(INCLUDES) -o bin/card_scan

clean:
	@echo RM $(OBJS) bin
	@rm -rf $(OBJS) bin

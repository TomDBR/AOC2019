CC		:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 

PREFIX 		:= /usr/local
BINPREFIX 	:= $(PREFIX)/bin

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

SRC 		:= intcode.c
OBJ 		:= $(SRC:.c=.o)

VALG 		:= $(wildcard vgcore.*)

all: 		intcode

$(OBJ): 	Makefile 

intcode:	$(OBJ)

install:
	cp -p intcode "$(DESTDIR)$(BINPREFIX)"

uninstall:
	rm -f "$(DESTDIR)$(BINPREFIX)/intcode"

clean: 	
	rm -f intcode $(OBJ) $(VALG)

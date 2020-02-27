CC		:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 
LIBS 		:= -lpthread -lm $(shell pkg-config --libs-only-l ncurses)

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

SRC 	:= day13.c 
SRC 	+= $(wildcard ../UTILS/*.c)
OBJ 	:= $(SRC:.c=.o)
VALG 	:= $(wildcard vgcore.*)

all: 		day13

$(OBJ): 	Makefile 

day13: 		$(OBJ)
	$(CC) $(LIBS) $(OBJ) -o $@

clean: 	
	rm -f day13 $(OBJ) $(VALG)

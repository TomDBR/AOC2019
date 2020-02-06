CC		:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

SRC 		:= $(wildcard *.c)
SRC 		+= ../UTILS/util.c
OBJ 		:= $(SRC:.c=.o)

VALG 		:= $(wildcard vgcore.*)

all: 		day03

$(OBJ): 	Makefile 

day03:	$(OBJ)

clean: 	
	rm -f day03 $(OBJ) $(VALG)

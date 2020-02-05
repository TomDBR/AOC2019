CC		:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

SRC_INTCODE	:= intcode.c
OBJ_INTCODE 	:= $(SRC_INTCODE:.c=.o)

SRC_DAY11	:= day11.c shell_cmd.c util.c 
OBJ_DAY11 	:= $(SRC_DAY11:.c=.o)

VALG 		:= $(wildcard vgcore.*)

all: 		intcode day11

$(OBJ_DAY11): 	Makefile 

intcode:	$(OBJ_INTCODE)
day11: 		$(OBJ_DAY11)


clean: 	
	rm -f day11 intcode $(OBJ_INTCODE) $(OBJ_DAY11) $(VALG)

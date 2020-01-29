CC		:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

SRC_INTCODE	:= intcode.c
OBJ_INTCODE 	:= $(SRC_INTCODE:.c=.o)

SRC_DAY07	:= day07.c shell_cmd.c util.c 
OBJ_DAY07 	:= $(SRC_DAY07:.c=.o)

VALG 		:= $(wildcard vgcore.*)

all: 		intcode day07

$(OBJ_DAY07): 	Makefile 

intcode.bin:	$(OBJ_INTCODE)
day07: 		$(OBJ_DAY07)


clean: 	
	rm -f day07 intcode $(OBJ_INTCODE) $(OBJ_DAY07) $(VALG)

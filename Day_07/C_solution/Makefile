CC		:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

SRC_DAY07	:= day07.c 
SRC_DAY07	+= $(wildcard ../../UTILS/*.c)
OBJ_DAY07 	:= $(SRC_DAY07:.c=.o)

VALG 		:= $(wildcard vgcore.*)

all: 		day07

$(OBJ_DAY07): 	Makefile 

intcode.bin:	$(OBJ_INTCODE)
day07: 		$(OBJ_DAY07)


clean: 	
	rm -f day07 $(OBJ_DAY07) $(VALG)

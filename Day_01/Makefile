CC		:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

SRC 		:= $(wildcard *.c)
OBJ 		:= $(SRC:.c=.o)

VALG 		:= $(wildcard vgcore.*)

all: 		day01

$(OBJ): 	Makefile 

day01:	$(OBJ)

clean: 	
	rm -f day01 $(OBJ) $(VALG)

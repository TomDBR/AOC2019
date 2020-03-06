CC		:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 
LIBS 		:= -lm 

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

SRC 	:= day14.c 
OBJ 	:= $(SRC:.c=.o)
VALG 	:= $(wildcard vgcore.*)

all: 		day14

$(OBJ): 	Makefile 

day14: 		$(OBJ)
	$(CC) $(LIBS) $(OBJ) -o $@

clean: 	
	rm -f day14 $(OBJ) $(VALG)

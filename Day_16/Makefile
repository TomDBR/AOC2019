CC		:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 
LIBS 		:= -lm

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

SRC 	:= day16.c 
#SRC 	+= $(wildcard ../UTILS/*.c)
OBJ 	:= $(SRC:.c=.o)
VALG 	:= $(wildcard vgcore.*)

all: 		day16

$(OBJ): 	Makefile 

day16: 		$(OBJ)
	$(CC) $(LIBS) $(OBJ) -o $@

clean: 	
	rm -f day16 $(OBJ) $(VALG)

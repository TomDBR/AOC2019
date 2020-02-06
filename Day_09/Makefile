CC		:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

SRC 		:= $(wildcard *.c)
SRC 		+= $(wildcard ../UTILS/*.c)
OBJ 		:= $(SRC:.c=.o)

VALG 		:= $(wildcard vgcore.*)

all: 		09

$(OBJ): 	Makefile 

09:	$(OBJ)

clean: 	
	rm -f 09 $(OBJ) $(VALG)

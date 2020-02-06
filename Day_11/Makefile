CC		:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

SRC_DAY11	:= 11.c 
SRC_DAY11	+= $(wildcard ../UTILS/*.c)
OBJ_DAY11 	:= $(SRC_DAY11:.c=.o)

VALG 		:= $(wildcard vgcore.*)

all: 		11

$(OBJ_DAY11): 	Makefile 

11: 		$(OBJ_DAY11)


clean: 	
	rm -f 11 $(OBJ_DAY11) $(VALG)

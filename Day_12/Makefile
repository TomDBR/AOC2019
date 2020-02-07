CC		:= gcc
CFLAGS		:= -Wall -Wextra -Wpedantic 
LIBS 		:= -lm

ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

SRC_DAY12	:= day12.c 
SRC_DAY12	+= $(wildcard ../UTILS/*.c)
OBJ_DAY12 	:= $(SRC_DAY12:.c=.o)

VALG 		:= $(wildcard vgcore.*)

all: 		day12

$(OBJ_DAY12): 	Makefile 

day12: 		$(OBJ_DAY12)
	$(CC) $(LIBS) $(OBJ_DAY12) -o $@

clean: 	
	rm -f day12 $(OBJ_DAY12) $(VALG)

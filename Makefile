OBJS	= main.o utils.o objective_functions.o shade.o mts_ls1.o
SOURCE	= main.c utils.c objective_functions.c shade.c mts_ls1.c
HEADER	= settings.h utils.h objective_functions.h shade.h mts_ls1.h
OUT	= a.out
CC	 = gcc
FLAGS	 = -g -c -Wall -Werror
LFLAGS	 = -lm

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c settings.h
	$(CC) $(FLAGS) main.c 

utils.o: utils.c settings.h
	$(CC) $(FLAGS) utils.c 

objective_functions.o: objective_functions.c settings.h
	$(CC) $(FLAGS) objective_functions.c 

shade.o: shade.c settings.h
	$(CC) $(FLAGS) shade.c 

mts_ls1.o: mts_ls1.c settings.h
	$(CC) $(FLAGS) mts_ls1.c 


clean:
	rm -f $(OBJS) $(OUT)

run:
	make all
	./$(OUT)
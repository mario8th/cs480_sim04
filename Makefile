CC = gcc -pthread

CFLAGS=-O0 -Wall -Wextra -std=c99 -pedantic

sim04: sim04.o helper_lib.o linked_list.o input_data_management.o simtimer.o simulator.o mmu.o

sim04.o: sim04.c helper_lib.h linked_list.h input_data_management.h simtimer.h

helper_lib.o: helper_lib.c helper_lib.h

linked_list.o: linked_list.c linked_list.h

input_data_management.o: input_data_management.c input_data_management.h

simtimer.o: simtimer.c simtimer.h

simulator.o: simulator.c simulator.h

mmu.o: mmu.c mmu.h

.PHONY: clean, run

clean:
	rm -f *.o sim04

run: sim04
	./sim04 config1.cnf

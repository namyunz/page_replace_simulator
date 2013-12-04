msimulate: msimulate.o
	gcc -o $@ $@.o
.c.o:
	gcc -g -c $<

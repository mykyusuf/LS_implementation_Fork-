all: myk

myk:
	gcc -c main.c
	gcc -o buNeDu main.o

clean:
	rm *.o
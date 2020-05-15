INCLUDES=../../include
LIBS=../../lib

all: client server 

client: main2.o
	gcc main2.o -o client
main2.o: main2.c
	gcc -c main2.c 
clean:
	rm -f client*.o

server: main.o
	gcc main.o -o server -L $(LIBS) -lsense -lm
main.o: main.c
	gcc -c main.c -I $(INCLUDES)
clean1:
	rm -c main.c

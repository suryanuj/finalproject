all: client 
client: main.o
	gcc main.o -o client
main.o: main.c
	gcc -c main.c 
clean:
	rm -f client*.o

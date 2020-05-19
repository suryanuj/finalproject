INCLUDES=../../include
LIBS=../../lib

all: InfoWeather

InfoWeather: main.o
	gcc main.o -o InfoWeather -L $(LIBS) -lsense -lm
main.o: main.c
	gcc -c main.c -I $(INCLUDES)
clean:
	rm -f InfoWeather *.o

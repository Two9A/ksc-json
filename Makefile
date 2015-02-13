CC = gcc -c
LD = gcc
LIBS = -lc -ljansson

all: ksc-json

ksc-json: main.o ksc.o hash.o log.o
	$(LD) -o $@ $^ $(LIBS)

%.o: %.cpp %.c %.h
	$(CC) -o $@ $<

clean:
	rm *.o ksc-json

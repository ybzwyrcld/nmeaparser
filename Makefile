.PHONY: clean

CC=
CFLAGS=-Wall -std=c++11 -pthread

INC=-I./include
LDFLAGS= -lpthread


all: nmea_parse


nmea_parse: examples/nmea_parse.o \
	src/nmea_parser.o
	$(CC)g++ $^ ${LDFLAGS} -o $@


%.o:%.cc
	$(CC)g++ $(CFLAGS) $(INC) $(LDFLAGS) -o $@ -c $<

install:
	$(CC)strip nmea_parse


clean:
	rm -rf src/*.o examples/*.o 
	rm -rf nmea_parse

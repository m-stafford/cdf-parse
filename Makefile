CC=gcc
CFLAGS=-c -Wall
CDFINC=/Applications/cdf33_1-dist/include
CDFLIB=/Applications/cdf33_1-dist/lib/libcdf.dylib -lm

all: cdf-parse

cdf-parse: main.o
	$(CC) main.o $(CDFLIB) -o cdf-parse

main.o: main.c
	$(CC) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE -I$(CDFINC) $(CFLAGS) main.c 

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)

.PHONY: all

all: assembler.exe

assembler.exe: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o assembler.exe


.PHONY: test

test: tests/test_parser.c src/parser.c src/parser.h src/instruction.h
	$(CC) $(CFLAGS) -Isrc tests/test_parser.c src/parser.c -o test_parser.exe
	./test_parser.exe

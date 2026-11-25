CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

ASSEMBLER_SOURCES = src/main.c src/parser.c src/encoder.c
INTERPRETER_SOURCES = src/interpreter_main.c src/decoder.c src/vm.c
HEADERS = $(wildcard src/*.h)

.PHONY: all test

all: assembler.exe interpreter.exe

assembler.exe: $(ASSEMBLER_SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(ASSEMBLER_SOURCES) -o $@

interpreter.exe: $(INTERPRETER_SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(INTERPRETER_SOURCES) -o $@

test: test_parser.exe test_encoder.exe test_decoder.exe test_vm.exe
	./test_parser.exe
	./test_encoder.exe
	./test_decoder.exe
	./test_vm.exe

test_parser.exe: tests/test_parser.c src/parser.c $(HEADERS)
	$(CC) $(CFLAGS) -Isrc tests/test_parser.c src/parser.c -o $@

test_encoder.exe: tests/test_encoder.c src/encoder.c $(HEADERS)
	$(CC) $(CFLAGS) -Isrc tests/test_encoder.c src/encoder.c -o $@

test_decoder.exe: tests/test_decoder.c src/decoder.c $(HEADERS)
	$(CC) $(CFLAGS) -Isrc tests/test_decoder.c src/decoder.c -o $@

test_vm.exe: tests/test_vm.c src/vm.c $(HEADERS)
	$(CC) $(CFLAGS) -Isrc tests/test_vm.c src/vm.c -o $@

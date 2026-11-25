#ifndef PARSER_H
#define PARSER_H

#include "instruction.h"

typedef enum { PARSE_ERROR = -1, PARSE_EMPTY = 0, PARSE_OK = 1 } ParseResult;

ParseResult parse_instruction(const char *line, Instruction *output);

#endif

#include "parser.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static const char *skip_spaces(const char *text) {
  while (isspace((unsigned char)*text)) {
    ++text;
  }

  return text;
}

static int parse_register(const char **cursor, uint32_t *value) {
  const char *text = skip_spaces(*cursor);

  if (*text != 'r') {
    return 0;
  }
  ++text;

  if (*text < '0' || *text > '7') {
    return 0;
  }

  *value = (uint32_t)(*text - '0');
  *cursor = text + 1;
  return 1;
}

static int parse_number(const char **cursor, uint32_t *value) {
  const char *text = skip_spaces(*cursor);

  if (!isdigit((unsigned char)*text)) {
    return 0;
  }

  int base = 10;
  if (text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
    base = 16;
  }

  errno = 0;
  char *end = NULL;
  unsigned long number = strtoul(text, &end, base);

  if (end == text || errno == ERANGE || number > UINT32_C(0x1FFFFF)) {
    return 0;
  }

  *value = (uint32_t)number;
  *cursor = end;
  return 1;
}

static int parse_comma(const char **cursor) {
  const char *text = skip_spaces(*cursor);

  if (*text != ',') {
    return 0;
  }

  *cursor = text + 1;
  return 1;
}

ParseResult parse_instruction(const char *line, Instruction *output) {
  const char *cursor = skip_spaces(line);

  if (*cursor == '\0' || *cursor == '#') {
    return PARSE_EMPTY;
  }

  const char *mnemonic = cursor;
  while (*cursor != '\0' && !isspace((unsigned char)*cursor)) {
    ++cursor;
  }

  size_t length = (size_t)(cursor - mnemonic);
  Opcode opcode;

  if (length == 2 && strncmp(mnemonic, "li", length) == 0) {
    opcode = OP_LI;
  } else if (length == 2 && strncmp(mnemonic, "lw", length) == 0) {
    opcode = OP_LW;
  } else if (length == 2 && strncmp(mnemonic, "sw", length) == 0) {
    opcode = OP_SW;
  } else if (length == 3 && strncmp(mnemonic, "ror", length) == 0) {
    opcode = OP_ROR;
  } else {
    return PARSE_ERROR;
  }

  uint32_t reg;
  uint32_t number;

  if (!parse_register(&cursor, &reg) || !parse_comma(&cursor) ||
      !parse_number(&cursor, &number)) {
    return PARSE_ERROR;
  }

  Instruction instruction = {.a = opcode};

  switch (opcode) {
  case OP_LI:
  case OP_SW:
    instruction.b = number;
    instruction.c = reg;
    break;

  case OP_LW:
    instruction.b = reg;
    instruction.c = number;
    break;

  case OP_ROR:
    instruction.c = number;
    instruction.d = reg;

    if (!parse_comma(&cursor) || !parse_register(&cursor, &instruction.b)) {
      return PARSE_ERROR;
    }
    break;

  default:
    return PARSE_ERROR;
  }

  cursor = skip_spaces(cursor);

  if (*cursor != '\0' && *cursor != '#') {
    return PARSE_ERROR;
  }

  *output = instruction;
  return PARSE_OK;
}

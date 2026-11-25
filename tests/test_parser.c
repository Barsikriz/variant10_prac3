#include "parser.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
  Instruction instruction = {0};

  assert(parse_instruction("li r0, 941", &instruction) == PARSE_OK);
  assert(instruction.a == OP_LI);
  assert(instruction.b == 941);
  assert(instruction.c == 0);
  assert(instruction.d == 0);

  assert(parse_instruction("  li r7, 0x3AD # comment", &instruction) ==
         PARSE_OK);
  assert(instruction.b == 941);
  assert(instruction.c == 7);

  assert(parse_instruction("li r0, 2097151", &instruction) == PARSE_OK);

  assert(parse_instruction("", &instruction) == PARSE_EMPTY);
  assert(parse_instruction("  # comment", &instruction) == PARSE_EMPTY);

  assert(parse_instruction("li r8, 1", &instruction) == PARSE_ERROR);
  assert(parse_instruction("li r0 941", &instruction) == PARSE_ERROR);
  assert(parse_instruction("li r0, -1", &instruction) == PARSE_ERROR);
  assert(parse_instruction("li r0, 2097152", &instruction) == PARSE_ERROR);
  assert(parse_instruction("li r0, 941 garbage", &instruction) == PARSE_ERROR);
  assert(parse_instruction("unknown r0, 1", &instruction) == PARSE_ERROR);

  assert(parse_instruction("lw r0, 508", &instruction) == PARSE_OK);
  assert(instruction.a == OP_LW);
  assert(instruction.b == 0);
  assert(instruction.c == 508);
  assert(instruction.d == 0);

  assert(parse_instruction("sw r2, 393", &instruction) == PARSE_OK);
  assert(instruction.a == OP_SW);
  assert(instruction.b == 393);
  assert(instruction.c == 2);
  assert(instruction.d == 0);

  assert(parse_instruction("ror r7, 569, r7", &instruction) == PARSE_OK);
  assert(instruction.a == OP_ROR);
  assert(instruction.b == 7);
  assert(instruction.c == 569);
  assert(instruction.d == 7);

  assert(parse_instruction("lw r7, 2097151", &instruction) == PARSE_OK);
  assert(parse_instruction("sw r7, 2097151", &instruction) == PARSE_OK);

  assert(parse_instruction("lw r8, 1", &instruction) == PARSE_ERROR);
  assert(parse_instruction("sw r0, 2097152", &instruction) == PARSE_ERROR);
  assert(parse_instruction("ror r0, 1", &instruction) == PARSE_ERROR);
  assert(parse_instruction("ror r0, 1, r8", &instruction) == PARSE_ERROR);
  assert(parse_instruction("ror r0, 1, r1 extra", &instruction) == PARSE_ERROR);
  assert(parse_instruction("li r00, 1", &instruction) == PARSE_ERROR);

  Instruction unchanged = {.a = OP_SW, .b = 123, .c = 2, .d = 0};

  assert(parse_instruction("ror r0, 1, r8", &unchanged) == PARSE_ERROR);
  assert(unchanged.a == OP_SW);
  assert(unchanged.b == 123);
  assert(unchanged.c == 2);
  assert(unchanged.d == 0);

  puts("Parser tests passed.");

  return 0;
}

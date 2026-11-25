#include "encoder.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  const Instruction instructions[] = {{.a = OP_LI, .b = 941, .c = 0, .d = 0},
                                      {.a = OP_LW, .b = 0, .c = 508, .d = 0},
                                      {.a = OP_SW, .b = 393, .c = 2, .d = 0},
                                      {.a = OP_ROR, .b = 7, .c = 569, .d = 7}};

  const uint8_t expected[][4] = {{0xD2, 0x3A, 0x00, 0x00},
                                 {0x0E, 0xFE, 0x00, 0x00},
                                 {0x97, 0x18, 0x00, 0x04},
                                 {0xF5, 0x1C, 0x01, 0x70}};

  size_t count = sizeof(instructions) / sizeof(instructions[0]);

  for (size_t i = 0; i < count; ++i) {
    uint8_t bytes[4] = {0};

    assert(encode_instruction(&instructions[i], bytes));
    assert(memcmp(bytes, expected[i], sizeof(bytes)) == 0);
  }

  uint8_t bytes[4] = {0};

  Instruction maximum = {.a = OP_ROR, .b = 7, .c = UINT32_C(0x1FFFFF), .d = 7};

  const uint8_t maximum_expected[4] = {0xF5, 0xFF, 0xFF, 0x7F};

  assert(encode_instruction(&maximum, bytes));
  assert(memcmp(bytes, maximum_expected, sizeof(bytes)) == 0);

  Instruction invalid = {.a = OP_LI, .b = UINT32_C(0x200000)};
  assert(!encode_instruction(&invalid, bytes));

  invalid = (Instruction){.a = OP_LW, .b = 8};
  assert(!encode_instruction(&invalid, bytes));

  invalid = (Instruction){.a = OP_SW, .c = 8};
  assert(!encode_instruction(&invalid, bytes));

  invalid = (Instruction){.a = OP_ROR, .d = 8};
  assert(!encode_instruction(&invalid, bytes));

  invalid = (Instruction){.a = OP_LI, .d = 1};
  assert(!encode_instruction(&invalid, bytes));

  invalid = (Instruction){.a = (Opcode)15};
  assert(!encode_instruction(&invalid, bytes));

  puts("Encoder tests passed.");
  return 0;
}

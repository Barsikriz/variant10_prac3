#include "decoder.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

int main(void) {
  const uint8_t bytes[][4] = {{0xD2, 0x3A, 0x00, 0x00},
                              {0x0E, 0xFE, 0x00, 0x00},
                              {0x97, 0x18, 0x00, 0x04},
                              {0xF5, 0x1C, 0x01, 0x70}};

  const Instruction expected[] = {{.a = OP_LI, .b = 941, .c = 0, .d = 0},
                                  {.a = OP_LW, .b = 0, .c = 508, .d = 0},
                                  {.a = OP_SW, .b = 393, .c = 2, .d = 0},
                                  {.a = OP_ROR, .b = 7, .c = 569, .d = 7}};

  size_t count = sizeof(expected) / sizeof(expected[0]);

  for (size_t i = 0; i < count; ++i) {
    Instruction instruction = {0};

    assert(decode_instruction(bytes[i], &instruction));
    assert(instruction.a == expected[i].a);
    assert(instruction.b == expected[i].b);
    assert(instruction.c == expected[i].c);
    assert(instruction.d == expected[i].d);
  }

  const uint8_t maximum[4] = {0xF5, 0xFF, 0xFF, 0x7F};
  Instruction instruction = {0};

  assert(decode_instruction(maximum, &instruction));
  assert(instruction.a == OP_ROR);
  assert(instruction.b == 7);
  assert(instruction.c == UINT32_C(0x1FFFFF));
  assert(instruction.d == 7);

  const uint8_t unknown[4] = {0x00, 0x00, 0x00, 0x00};
  const uint8_t invalid_li[4] = {0x02, 0x00, 0x00, 0x10};
  const uint8_t invalid_ror[4] = {0x05, 0x00, 0x00, 0x80};

  assert(!decode_instruction(unknown, &instruction));
  assert(!decode_instruction(invalid_li, &instruction));
  assert(!decode_instruction(invalid_ror, &instruction));

  puts("Decoder tests passed.");
  return 0;
}

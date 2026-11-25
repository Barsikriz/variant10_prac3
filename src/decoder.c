#include "decoder.h"

int decode_instruction(const uint8_t bytes[4], Instruction *output) {
  uint32_t word = (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
                  ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);

  Instruction instruction = {0};
  instruction.a = (Opcode)(word & UINT32_C(0xF));

  switch (instruction.a) {
  case OP_LI:
  case OP_SW:
    if ((word & UINT32_C(0xF0000000)) != 0) {
      return 0;
    }

    instruction.b = (word >> 4) & UINT32_C(0x1FFFFF);
    instruction.c = (word >> 25) & UINT32_C(0x7);
    break;

  case OP_LW:
    if ((word & UINT32_C(0xF0000000)) != 0) {
      return 0;
    }

    instruction.b = (word >> 4) & UINT32_C(0x7);
    instruction.c = (word >> 7) & UINT32_C(0x1FFFFF);
    break;

  case OP_ROR:
    if ((word & UINT32_C(0x80000000)) != 0) {
      return 0;
    }

    instruction.b = (word >> 4) & UINT32_C(0x7);
    instruction.c = (word >> 7) & UINT32_C(0x1FFFFF);
    instruction.d = (word >> 28) & UINT32_C(0x7);
    break;

  default:
    return 0;
  }

  *output = instruction;
  return 1;
}

#include "encoder.h"

int encode_instruction(const Instruction *instruction, uint8_t bytes[4]) {
  uint32_t word;
  const uint32_t max_operand = UINT32_C(0x1FFFFF);

  switch (instruction->a) {
  case OP_LI:
  case OP_SW:
    if (instruction->b > max_operand || instruction->c > 7 ||
        instruction->d != 0) {
      return 0;
    }

    word = (uint32_t)instruction->a | (instruction->b << 4) |
           (instruction->c << 25);
    break;

  case OP_LW:
    if (instruction->b > 7 || instruction->c > max_operand ||
        instruction->d != 0) {
      return 0;
    }

    word = (uint32_t)instruction->a | (instruction->b << 4) |
           (instruction->c << 7);
    break;

  case OP_ROR:
    if (instruction->b > 7 || instruction->c > max_operand ||
        instruction->d > 7) {
      return 0;
    }

    word = (uint32_t)instruction->a | (instruction->b << 4) |
           (instruction->c << 7) | (instruction->d << 28);
    break;

  default:
    return 0;
  }

  for (unsigned i = 0; i < 4; ++i) {
    bytes[i] = (uint8_t)((word >> (8 * i)) & UINT32_C(0xFF));
  }

  return 1;
}

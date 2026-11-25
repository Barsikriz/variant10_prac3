#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

typedef enum { OP_LI = 2, OP_LW = 14, OP_SW = 7, OP_ROR = 5 } Opcode;

typedef struct {
  Opcode a;
  uint32_t b;
  uint32_t c;
  uint32_t d;
} Instruction;

#endif

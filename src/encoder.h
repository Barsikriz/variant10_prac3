#ifndef ENCODER_H
#define ENCODER_H

#include "instruction.h"

int encode_instruction(const Instruction *instruction, uint8_t bytes[4]);

#endif

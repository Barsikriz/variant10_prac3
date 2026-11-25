#ifndef DECODER_H
#define DECODER_H

#include "instruction.h"

int decode_instruction(const uint8_t bytes[4], Instruction *output);

#endif

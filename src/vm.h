#ifndef VM_H
#define VM_H

#include "instruction.h"

#define VM_REGISTER_COUNT 8
#define VM_MEMORY_SIZE (UINT32_C(1) << 21)

typedef struct {
    uint32_t registers[VM_REGISTER_COUNT];
    uint32_t *memory;
} Vm;

int vm_init(Vm *vm);
void vm_free(Vm *vm);
int vm_execute(Vm *vm, const Instruction *instruction);

#endif

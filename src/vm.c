#include "vm.h"

#include <stdlib.h>

int vm_init(Vm *vm) {
  *vm = (Vm){0};

  vm->memory = calloc(VM_MEMORY_SIZE, sizeof(*vm->memory));

  return vm->memory != NULL;
}

void vm_free(Vm *vm) {
  free(vm->memory);
  *vm = (Vm){0};
}

int vm_execute(Vm *vm, const Instruction *instruction) {
  if (vm->memory == NULL) {
    return 0;
  }

  switch (instruction->a) {
  case OP_LI:
    if (instruction->b >= VM_MEMORY_SIZE ||
        instruction->c >= VM_REGISTER_COUNT || instruction->d != 0) {
      return 0;
    }

    vm->registers[instruction->c] = instruction->b;
    return 1;

  case OP_LW:
    if (instruction->b >= VM_REGISTER_COUNT ||
        instruction->c >= VM_MEMORY_SIZE || instruction->d != 0) {
      return 0;
    }

    vm->registers[instruction->b] = vm->memory[instruction->c];
    return 1;

  case OP_SW:
    if (instruction->b >= VM_MEMORY_SIZE ||
        instruction->c >= VM_REGISTER_COUNT || instruction->d != 0) {
      return 0;
    }

    vm->memory[instruction->b] = vm->registers[instruction->c];
    return 1;

  default:
    return 0;
  }
}

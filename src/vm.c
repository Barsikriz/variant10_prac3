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

static uint32_t rotate_right(uint32_t value, uint32_t count) {
  count %= 32;

  if (count == 0) {
    return value;
  }

  return (value >> count) | (value << (32 - count));
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

  case OP_ROR: {
    if (instruction->b >= VM_REGISTER_COUNT ||
        instruction->c >= VM_MEMORY_SIZE ||
        instruction->d >= VM_REGISTER_COUNT) {
      return 0;
    }

    uint32_t count_address = vm->registers[instruction->b];

    if (count_address >= VM_MEMORY_SIZE) {
      return 0;
    }

    uint32_t value = vm->memory[instruction->c];
    uint32_t count = vm->memory[count_address];

    vm->registers[instruction->d] = rotate_right(value, count);
    return 1;
  }

  default:
    return 0;
  }
}

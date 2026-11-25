#include "vm.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

int main(void) {
  Vm vm = {0};

  assert(vm_init(&vm));

  for (size_t i = 0; i < VM_REGISTER_COUNT; ++i) {
    assert(vm.registers[i] == 0);
  }

  assert(vm.memory[100] == 0);
  assert(vm.memory[VM_MEMORY_SIZE - 1] == 0);

  const Instruction program[] = {
      /* Initialize source array at addresses 100..102. */
      {.a = OP_LI, .b = 10, .c = 0},
      {.a = OP_SW, .b = 100, .c = 0},
      {.a = OP_LI, .b = 20, .c = 0},
      {.a = OP_SW, .b = 101, .c = 0},
      {.a = OP_LI, .b = 30, .c = 0},
      {.a = OP_SW, .b = 102, .c = 0},

      /* Copy source array to addresses 200..202. */
      {.a = OP_LW, .b = 1, .c = 100},
      {.a = OP_SW, .b = 200, .c = 1},
      {.a = OP_LW, .b = 1, .c = 101},
      {.a = OP_SW, .b = 201, .c = 1},
      {.a = OP_LW, .b = 1, .c = 102},
      {.a = OP_SW, .b = 202, .c = 1}};

  size_t count = sizeof(program) / sizeof(program[0]);

  for (size_t i = 0; i < count; ++i) {
    assert(vm_execute(&vm, &program[i]));
  }

  assert(vm.registers[0] == 30);
  assert(vm.registers[1] == 30);

  const uint32_t expected[] = {10, 20, 30};

  for (size_t i = 0; i < 3; ++i) {
    assert(vm.memory[100 + i] == expected[i]);
    assert(vm.memory[200 + i] == expected[i]);
  }

  const Instruction boundary[] = {
      {.a = OP_LI, .b = VM_MEMORY_SIZE - 1, .c = 7},
      {.a = OP_SW, .b = VM_MEMORY_SIZE - 1, .c = 7},
      {.a = OP_LW, .b = 6, .c = VM_MEMORY_SIZE - 1}};

  for (size_t i = 0; i < 3; ++i) {
    assert(vm_execute(&vm, &boundary[i]));
  }

  assert(vm.registers[6] == VM_MEMORY_SIZE - 1);

  Instruction invalid = {.a = OP_LW, .b = 8, .c = 100};
  assert(!vm_execute(&vm, &invalid));

  invalid = (Instruction){.a = OP_SW, .b = VM_MEMORY_SIZE, .c = 0};
  assert(!vm_execute(&vm, &invalid));

  invalid = (Instruction){.a = OP_LI, .b = VM_MEMORY_SIZE, .c = 0};
  assert(!vm_execute(&vm, &invalid));

  /* Rotate a known 32-bit value by different counts. */
  vm.memory[300] = UINT32_C(0x12345678);
  vm.registers[1] = 301;

  const uint32_t counts[] = {0, 4, 32, 36, 1, 31};

  const uint32_t rotated[] = {UINT32_C(0x12345678), UINT32_C(0x81234567),
                              UINT32_C(0x12345678), UINT32_C(0x81234567),
                              UINT32_C(0x091A2B3C), UINT32_C(0x2468ACF0)};

  const Instruction rotate = {.a = OP_ROR, .b = 1, .c = 300, .d = 2};

  const Instruction store = {.a = OP_SW, .b = 302, .c = 2};

  for (size_t i = 0; i < sizeof(counts) / sizeof(counts[0]); ++i) {
    vm.memory[301] = counts[i];

    assert(vm_execute(&vm, &rotate));
    assert(vm.registers[2] == rotated[i]);

    assert(vm_execute(&vm, &store));
    assert(vm.memory[302] == rotated[i]);
  }

  /* The destination may also be the address register. */
  vm.memory[301] = 4;
  vm.registers[1] = 301;

  Instruction overlap = {.a = OP_ROR, .b = 1, .c = 300, .d = 1};

  assert(vm_execute(&vm, &overlap));
  assert(vm.registers[1] == UINT32_C(0x81234567));

  /* Reject an indirect address outside data memory. */
  vm.registers[1] = VM_MEMORY_SIZE;
  vm.registers[2] = 123;

  assert(!vm_execute(&vm, &rotate));
  assert(vm.registers[2] == 123);

  vm_free(&vm);

  assert(vm.memory == NULL);

  puts("VM tests passed.");
  return 0;
}

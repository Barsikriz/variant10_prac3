#include "decoder.h"
#include "vm.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *input;
  const char *output;
  uint32_t start;
  uint32_t end;
} Config;

static int parse_address(const char *text, uint32_t *address) {
  if (!isdigit((unsigned char)*text)) {
    return 0;
  }

  errno = 0;
  char *end = NULL;
  unsigned long value = strtoul(text, &end, 10);

  if (end == text || *end != '\0' || errno == ERANGE ||
      value >= VM_MEMORY_SIZE) {
    return 0;
  }

  *address = (uint32_t)value;
  return 1;
}

static int parse_arguments(int argc, char **argv, Config *config) {
  unsigned seen = 0;

  for (int i = 1; i < argc; ++i) {
    unsigned option;

    if (strcmp(argv[i], "--input") == 0) {
      option = 1;
    } else if (strcmp(argv[i], "--output") == 0) {
      option = 2;
    } else if (strcmp(argv[i], "--start") == 0) {
      option = 4;
    } else if (strcmp(argv[i], "--end") == 0) {
      option = 8;
    } else {
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      return 0;
    }

    if ((seen & option) != 0) {
      fprintf(stderr, "Duplicate option: %s\n", argv[i]);
      return 0;
    }

    if (i + 1 >= argc || strncmp(argv[i + 1], "--", 2) == 0) {
      fprintf(stderr, "Missing value for %s\n", argv[i]);
      return 0;
    }

    const char *value = argv[++i];

    if (*value == '\0') {
      fprintf(stderr, "Empty option value\n");
      return 0;
    }

    switch (option) {
    case 1:
      config->input = value;
      break;

    case 2:
      config->output = value;
      break;

    case 4:
      if (!parse_address(value, &config->start)) {
        fprintf(stderr, "Invalid start address: %s\n", value);
        return 0;
      }
      break;

    case 8:
      if (!parse_address(value, &config->end)) {
        fprintf(stderr, "Invalid end address: %s\n", value);
        return 0;
      }
      break;
    }

    seen |= option;
  }

  if (seen != 15) {
    fprintf(stderr, "--input, --output, --start and --end are required\n");
    return 0;
  }

  if (config->start > config->end) {
    fprintf(stderr, "Start address must not exceed end address\n");
    return 0;
  }

  if (strcmp(config->input, config->output) == 0) {
    fprintf(stderr, "Input and output paths must differ\n");
    return 0;
  }

  return 1;
}

static int run_program(FILE *input, Vm *vm) {
  size_t pc = 0;

  for (;;) {
    uint8_t bytes[4];
    size_t count = fread(bytes, 1, sizeof(bytes), input);

    if (ferror(input)) {
      fprintf(stderr, "Cannot read machine code\n");
      return 0;
    }

    if (count == 0) {
      break;
    }

    if (count != sizeof(bytes)) {
      fprintf(stderr, "Incomplete instruction at byte %zu\n", pc);
      return 0;
    }

    Instruction instruction = {0};

    if (!decode_instruction(bytes, &instruction)) {
      fprintf(stderr, "Invalid instruction at byte %zu\n", pc);
      return 0;
    }

    if (!vm_execute(vm, &instruction)) {
      fprintf(stderr, "Cannot execute instruction at byte %zu\n", pc);
      return 0;
    }

    pc += sizeof(bytes);
  }

  printf("Executed instructions: %zu\n", pc / 4);
  return 1;
}

static int write_dump(const Config *config, const Vm *vm) {
  FILE *output = fopen(config->output, "w");

  if (output == NULL) {
    perror(config->output);
    return 0;
  }

  int success = fprintf(output,
                        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                        "<memory start=\"%" PRIu32 "\" end=\"%" PRIu32 "\">\n",
                        config->start, config->end) >= 0;

  for (uint32_t address = config->start; success && address <= config->end;
       ++address) {
    success =
        fprintf(output,
                "  <cell address=\"%" PRIu32 "\" value=\"%" PRIu32 "\"/>\n",
                address, vm->memory[address]) >= 0;
  }

  if (success) {
    success = fprintf(output, "</memory>\n") >= 0;
  }

  if (fclose(output) != 0) {
    success = 0;
  }

  if (!success) {
    fprintf(stderr, "Cannot write memory dump\n");

    if (remove(config->output) != 0) {
      perror(config->output);
    }
  }

  return success;
}

int main(int argc, char **argv) {
  Config config = {0};

  if (!parse_arguments(argc, argv, &config)) {
    fprintf(stderr, "Usage: %s --input FILE --output FILE --start N --end N\n",
            argv[0]);
    return 1;
  }

  FILE *input = fopen(config.input, "rb");
  if (input == NULL) {
    perror(config.input);
    return 1;
  }

  Vm vm = {0};

  if (!vm_init(&vm)) {
    fprintf(stderr, "Cannot allocate VM memory\n");
    fclose(input);
    return 1;
  }

  int success = run_program(input, &vm);
  fclose(input);

  if (success) {
    success = write_dump(&config, &vm);
  }

  vm_free(&vm);
  return success ? 0 : 1;
}

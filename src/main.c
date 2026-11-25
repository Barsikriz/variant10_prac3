#include "encoder.h"
#include "parser.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  const char *input;
  const char *output;
  bool test;
} Config;

static void print_usage(const char *program) {
  printf("Usage: %s --input FILE --output FILE [--test]\n", program);
}

static int parse_arguments(int argc, char **argv, Config *config) {
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--test") == 0) {
      if (config->test) {
        fprintf(stderr, "Duplicate option: --test\n");
        return 0;
      }

      config->test = true;
      continue;
    }

    const char **destination;

    if (strcmp(argv[i], "--input") == 0) {
      destination = &config->input;
    } else if (strcmp(argv[i], "--output") == 0) {
      destination = &config->output;
    } else {
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      return 0;
    }

    if (*destination != NULL) {
      fprintf(stderr, "Duplicate option: %s\n", argv[i]);
      return 0;
    }

    if (i + 1 >= argc || strncmp(argv[i + 1], "--", 2) == 0) {
      fprintf(stderr, "Missing value for %s\n", argv[i]);
      return 0;
    }

    *destination = argv[++i];

    if (**destination == '\0') {
      fprintf(stderr, "Empty option value\n");
      return 0;
    }
  }

  if (config->input == NULL || config->output == NULL) {
    fprintf(stderr, "--input and --output are required\n");
    return 0;
  }

  if (strcmp(config->input, config->output) == 0) {
    fprintf(stderr, "Input and output paths must differ\n");
    return 0;
  }

  return 1;
}

static int assemble(FILE *input, FILE *output, const Config *config) {
  char line[1024];
  size_t line_number = 0;
  size_t instruction_count = 0;

  while (fgets(line, sizeof(line), input) != NULL) {
    ++line_number;

    if (strchr(line, '\n') == NULL && !feof(input)) {
      int next = fgetc(input);

      if (next != '\n' && next != EOF) {
        fprintf(stderr, "%s:%zu: line is too long\n", config->input,
                line_number);
        return 0;
      }
    }

    Instruction instruction = {0};
    ParseResult result = parse_instruction(line, &instruction);

    if (result == PARSE_EMPTY) {
      continue;
    }

    if (result == PARSE_ERROR) {
      fprintf(stderr, "%s:%zu: invalid instruction\n", config->input,
              line_number);
      return 0;
    }

    uint8_t bytes[4];

    if (!encode_instruction(&instruction, bytes)) {
      fprintf(stderr, "%s:%zu: cannot encode instruction\n", config->input,
              line_number);
      return 0;
    }

    if (fwrite(bytes, 1, sizeof(bytes), output) != sizeof(bytes)) {
      fprintf(stderr, "Cannot write machine code\n");
      return 0;
    }

    if (config->test) {
      printf("%02X %02X %02X %02X\n", (unsigned)bytes[0], (unsigned)bytes[1],
             (unsigned)bytes[2], (unsigned)bytes[3]);
    }

    ++instruction_count;
  }

  if (ferror(input)) {
    fprintf(stderr, "Cannot read input file: %s\n", config->input);
    return 0;
  }

  printf("Encoded instructions: %zu\n", instruction_count);
  return 1;
}

int main(int argc, char **argv) {
  Config config = {0};

  if (!parse_arguments(argc, argv, &config)) {
    print_usage(argv[0]);
    return 1;
  }

  FILE *input = fopen(config.input, "r");
  if (input == NULL) {
    perror(config.input);
    return 1;
  }

  FILE *output = fopen(config.output, "wb");
  if (output == NULL) {
    perror(config.output);
    fclose(input);
    return 1;
  }

  int success = assemble(input, output, &config);

  fclose(input);

  if (fclose(output) != 0) {
    fprintf(stderr, "Cannot finish writing: %s\n", config.output);
    success = 0;
  }

  if (!success) {
    if (remove(config.output) != 0) {
      perror(config.output);
    }
    return 1;
  }

  return 0;
}

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

static void print_instruction(const Instruction *instruction) {
  printf("A=%u, B=%" PRIu32 ", C=%" PRIu32, (unsigned)instruction->a,
         instruction->b, instruction->c);

  if (instruction->a == OP_ROR) {
    printf(", D=%" PRIu32, instruction->d);
  }

  putchar('\n');
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

  char line[1024];
  size_t line_number = 0;
  size_t instruction_count = 0;

  while (fgets(line, sizeof(line), input) != NULL) {
    ++line_number;

    if (strchr(line, '\n') == NULL && !feof(input)) {
      int next = fgetc(input);

      if (next != '\n' && next != EOF) {
        fprintf(stderr, "%s:%zu: line is too long\n", config.input,
                line_number);
        fclose(input);
        return 1;
      }
    }

    Instruction instruction = {0};
    ParseResult result = parse_instruction(line, &instruction);

    if (result == PARSE_EMPTY) {
      continue;
    }

    if (result == PARSE_ERROR) {
      fprintf(stderr, "%s:%zu: invalid instruction\n", config.input,
              line_number);
      fclose(input);
      return 1;
    }

    ++instruction_count;

    if (config.test) {
      print_instruction(&instruction);
    }
  }

  if (ferror(input)) {
    fprintf(stderr, "Cannot read input file: %s\n", config.input);
    fclose(input);
    return 1;
  }

  fclose(input);
  printf("Parsed instructions: %zu\n", instruction_count);
  return 0;
}

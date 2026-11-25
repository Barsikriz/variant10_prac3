# Variant 10: Educational VM Toolchain

C11 assembler and interpreter project for variant 10.

## Implemented functionality

Stage 1:

- Assembler command-line arguments.
- Assembly source parsing into instruction fields.
- Operand validation.
- Instruction field output in test mode.

Binary encoding and execution are not implemented at this stage.
The `--output` argument is accepted, but no binary file is created.

## Requirements

- GCC with C11 support.
- GNU Make.

## Build

```powershell
make
```

## Run

```powershell
.\assembler.exe --input tests/instructions.asm --output program.bin --test
```

| Argument | Description |
|---|---|
| `--input FILE` | Assembly source file |
| `--output FILE` | Binary output path, reserved for stage 2 |
| `--test` | Print parsed instruction fields |

Both file arguments are required.

## Assembly syntax

One instruction per line. Blank lines are ignored.
Comments start with `#`.
Mnemonics and register names are lowercase.

Registers: `r0` through `r7`.
Integer operands accept decimal and hexadecimal (`0x`) notation.
Immediate values and memory addresses range from 0 to 2097151.

| Syntax | Operation |
|---|---|
| `li rd, imm` | `R[rd] = imm` |
| `lw rd, addr` | `R[rd] = M[addr]` |
| `sw rs, addr` | `M[addr] = R[rs]` |
| `ror rd, addr, rp` | `R[rd] = ROR(M[addr], M[R[rp]])` |

All eight registers are writable.
Each source line supports up to 1023 characters, excluding the line ending.

## Tests

```powershell
make test
```

Parser tests cover instruction fields, numeric limits, comments,
invalid syntax, and preservation of the output structure on errors.

`tests/instructions.asm` contains the four instruction examples
from the variant specification.

Expected instruction fields:

```text
A=2, B=941, C=0
A=14, B=0, C=508
A=7, B=393, C=2
A=5, B=7, C=569, D=7
Parsed instructions: 4
```

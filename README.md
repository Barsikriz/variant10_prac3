# Variant 10: Educational VM Toolchain

C11 assembler and interpreter project for variant 10.

## Implemented functionality

- Assembler command-line arguments.
- Assembly source parsing into instruction fields.
- Operand validation.
- Encoding of all four variant 10 instructions.
- Binary output in little-endian byte order.
- Machine code output in test mode.

Each instruction occupies four bytes.
Execution is not implemented at this stage.

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
| --- | --- |
| `--input FILE` | Assembly source file |
| `--test` | Print encoded instruction bytes |
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
| --- | --- |
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
Encoder tests verify the four specification examples, maximum operand
values, and rejection of invalid instruction fields.

`tests/instructions.asm` contains the four instruction examples
from the variant specification.

Expected instruction fields:

```text
D2 3A 00 00
0E FE 00 00
97 18 00 04
F5 1C 01 70
Encoded instructions: 4
```

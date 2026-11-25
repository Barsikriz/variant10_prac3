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


## Interpreter

The interpreter executes `li`, `lw`, and `sw`.
The `ror` instruction is decoded but is not executable at this stage.

The VM has eight writable 32-bit unsigned registers and 2097152
32-bit unsigned data memory cells. Registers and data memory start at zero.
Instruction input is separate from data memory.

Instructions execute sequentially. Execution stops at the end of the
binary file. Invalid opcodes, nonzero unused bits, and incomplete
instructions are rejected.

```powershell
.\assembler.exe --input tests/copy.asm --output copy.bin
.\interpreter.exe --input copy.bin --output memory.xml --start 200 --end 202
```

| Argument | Description |
|---|---|
| `--input FILE` | Binary program |
| `--output FILE` | XML memory dump |
| `--start N` | First data memory address, inclusive |
| `--end N` | Last data memory address, inclusive |

All interpreter arguments are required.
Addresses are decimal integers from 0 to 2097151.

The copy example initializes addresses 100–102 with 10, 20, and 30,
then copies them to addresses 200–202.

VM tests cover array copying, boundary addresses, and invalid operands.
Decoder tests cover the specification examples and invalid encodings.

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

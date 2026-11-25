# Variant 10: Educational VM Toolchain

C11 assembler and interpreter project for variant 10.

## Implemented functionality

- Assembler command-line arguments.
- Assembly source parsing into instruction fields.
- Operand validation.
- Encoding of all four variant 10 instructions.
- Binary output in little-endian byte order.
- Machine code output in test mode.
- Execution of all four instructions.
- XML export of an inclusive data memory range.
- Elementwise rotation of two eight-element vectors.

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

The interpreter executes all four variant 10 instructions:
`li`, `lw`, `sw`, and `ror`.

Data values are unsigned 32-bit integers.
Rotation counts are reduced modulo 32.
The indirect address used by `ror` must be within data memory.

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
| --- | --- |
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

## Rotation example

```powershell
.\assembler.exe --input tests/rotate.asm --output rotate.bin
.\interpreter.exe --input rotate.bin --output memory.xml --start 200 --end 202
```

Expected values at addresses 200–202: 2147483652, 9, 2147483652.

VM rotation tests cover zero counts, counts modulo 32, result storage,
overlapping source and destination registers, and invalid indirect addresses.

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

## Vector examples

Each example initializes two eight-element vectors at addresses 100–107
and 200–207, then computes:

```text
M[100 + i] = ROR(M[100 + i], M[200 + i]), i = 0..7
```

The result overwrites the first vector. The second vector is preserved.
The processing sequence is unrolled because the instruction set has
no branch instructions.

| Source | Test data |
|---|---|
| `tests/vector_basic.asm` | Powers of two rotated by one position |
| `tests/vector_mixed.asm` | Different values and rotation counts |
| `tests/vector_counts.asm` | Counts 0, 1, 2, 3, 4, 31, 32, and 33 |

Each example contains 56 instructions and produces 224 bytes of machine code.

## Integration tests

On Windows, with Windows PowerShell available:

```powershell
make test-integration
```

The tests assemble and execute all three vector examples, verify binary
sizes, validate XML dump addresses, compare results with expected values,
and check that the second vector is preserved.

Generated binaries and XML dumps are stored in `build/`.

## Manual vector execution

```powershell
New-Item -ItemType Directory -Force .\build | Out-Null
.\assembler.exe --input tests/vector_basic.asm --output build/vector_basic.bin
.\interpreter.exe --input build/vector_basic.bin --output build/result.xml --start 100 --end 107
Get-Content .\build\result.xml
```

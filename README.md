# Compiler New

A custom programming language compiler built in C++ using LLVM as the backend. This project demonstrates parsing, AST generation, and LLVM IR code generation for a simple language.

## Features
- **Imports**: Simple module import system.
- **Primitive Types**: `void`, `int`, `float`, `char`, `bool`, `string`, `list`, `var`.
- **Variables**: Mutable, immutable (`const`), and dynamic (`var`).
- **Control Flow**: `if/else`, `while`, `for` (C‑style, range, collection), `switch`.
- **Functions**: Standard `fn` syntax, arrow functions, default parameters, multiple return values.
- **Collections**: Fixed‑size arrays and dynamic `list` with push/pop operations.
- **Strings**: Literals, interpolation, escape sequences.
- **Basic I/O**: `print`, `println`, `input`.
- **Operators**: Arithmetic, comparison, logical, bitwise, ternary.
- **Type System**: Explicit types, type inference for `const`, dynamic typing via `var`.

## Build Instructions



1. **Prerequisites**
   - C++ compiler (MSVC, GCC, or Clang)
   - CMake (>= 3.15)
   - LLVM development libraries (compatible with your compiler)

2. **Build Steps**
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```
   The resulting executable will be placed in the `build` directory.

## Running the Compiler

```bash
./compiler_new <source_file>.lang
```

Replace `<source_file>.lang` with a source file written in the custom language (see `examples/` for sample programs).

## Examples

Run the provided example programs:

```bash
./compiler_new examples/expressions_test.lang
./compiler_new examples/unary_test.lang
```


## Project Structure

```
compiler_new/
├─ src/                # Source code
│   ├─ CodeGen/        # Code generation components
│   └─ ...
├─ include/            # Header files
├─ examples/           # Sample language programs
├─ CMakeLists.txt      # Build configuration
└─ README.md           # This file
```

Additional documentation is available in the `docs/` directory.

## Contributing

Feel free to open issues or submit pull requests. Contributions are welcome!

## License

This project is licensed under the MIT License – see the `LICENSE` file for details.

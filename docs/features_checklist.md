# language Feature Checklist & Implementation Status

> **Completion Status**: 🔴 **15%** (Alpha Prototype)
>
> This document tracks the implementation status of the language compiler against a production-grade standard (comparable to G++/Clang/Rustc). It is "harsh and unforgiving" — a feature is only marked "Done" if it works flawlessly, handles edge cases, generates correct LLVM IR, and has no pending TODOs.

## 🚀 Priority 1: Core Compilation Pipeline (MVP)
*Must be 100% complete for the compiler to be usable for basic integer math.*

### Infrastructure
- [x] **Project Structure**: CMake build system defined and working.
- [x] **Command Line Interface**: Basic input file reading (`./compiler <file>`).
- [ ] **Error Reporting**:
    - [ ] Line/Column tracking (Lexer/Parser/CodeGen).
    - [ ] Source Snippet Display (show the code line with `^` pointing to error).
    - [ ] Compiler Panic recovery (don't segfault on error).

### Lexical Analysis (Lexer)
- [x] **Keywords**: `fn, return, if, else, while, for` etc.
- [x] **Identifiers**: Standard `[a-zA-Z_][a-zA-Z0-9_]*`.
- [ ] **String Literals**:
    - [x] Basic double-quoted strings (`"hello"`).
    - [x] Escape Sequences (`\n`, `\t`, `\\`, etc.).
    - [ ] **Formatted Strings**: String interpolation (`"Val: {x}"`) is parsed as string but formatting logic is **MISSING**.
- [ ] **Numeric Literals**:
    - [x] Integers (Decimal).
    - [x] Hex (`0x..`) and Binary (`0b..`).
    - [x] Floats (`3.14`).
    - [ ] Scientific Notation (`1e10`) - **MISSING**.

### AST & Parsing
- [x] **Expressions**: Precedence handled correctly for arithmetic/logic.
- [x] **Variables**: Declaration (`type x = val`) and Assignment.
- [x] **Control Flow**: `if/else`, `while`, `for`, `switch`.
- [ ] **Arrays/Collections**:
    - [ ] Array Literals (`[1, 2, 3]`) - **MISSING** in `parsePrimary`.
    - [ ] Range Literals (`[0:10]`) - Parsed into `RangeLiteralExpr` but no CodeGen.
- [ ] **Error Recovery**: Parser stops on first error (panic mode) or basic sync only. Needs improvement.

### Semantic Analysis (The "Brain")
*Critical Gap: No dedicated semantic analysis pass exists. Type checking is ad-hoc or non-existent.*
- [ ] **Symbol Table**:
    - [ ] Global/Local Scoping rules.
    - [ ] Shadowing checks (warn or error).
- [ ] **Type Checker**:
    - [ ] Type inference for `auto`/`const`.
    - [ ] Assignment compatibility check (`int = string` should fail).
    - [ ] Function argument type validation.
    - [ ] Binary Operator type reconciliation (e.g., `float + int` promotion).

---

## ⚙️ Priority 2: Code Generation & Runtime (Basic)
*Turning AST into executable code.*

### LLVM IR Generation
- [x] **Module Setup**: Preamble, formatting options.
- [x] **Integers**: `i32` math (`+`, `-`, `*`, `/`, `%`) using correct instructions (`add`, `sub`...).
- [ ] **Floating Point**:
    - [ ] **CRITICAL**: `genBinaryExpr` uses integer instructions (`add` instead of `fadd`) for all types. **BROKEN**.
    - [ ] Float comparisons (`fcmp` vs `icmp`).
    - [ ] Casting (`siture`, `fptosi` etc.) - Helper functions exist but logic to trigger them is weak.
- [ ] **Booleans**:
    - [x] `true`/`false` literals.
    - [x] Short-circuit logic for `&&`, `||` (`phi` nodes used).
- [ ] **Control Flow**:
    - [x] `if/else` branching.
    - [x] `while` loops.
    - [x] `for` loops (iterator style logic needs check).
    - [x] `break`/`continue` stack management.
- [ ] **Functions**:
    - [x] Definition & Calling convention.
    - [x] Recursion support.
    - [x] Stack allocation for parameters (mutable args).
    - [ ] **Return Analysis**: `isTerminator` check is basic; void functions might miss `ret void` in complex paths.

### Memory & Variables
- [ ] **Variable Storage**:
    - [x] Stack allocation (`alloca`) for local variables.
    - [ ] Global variables - **Partially implemented** (strings are global constants).
- [ ] **Strings**:
    - [x] Global string pooling for literals.
    - [ ] String concatenation (`+` operator) - **MISSING** in CodeGen.
    - [ ] String Memory Management (Leak hazard - currently no cleanup).

---

## 🏗️ Priority 3: Language Features (Standard)
*Features expected in a modern language.*

### Data Structures
- [ ] **Arrays**:
    - [ ] Static Arrays (`int[10]`) implementation (codegen: `alloca [10 x i32]`).
    - [ ] Bounds checking (runtime/compile time).
- [ ] **Dynamic Lists** (`list`):
    - [ ] Resizeable vector implementation in runtime.
    - [ ] Methods: `push`, `pop`, `len`.
- [ ] **Structs/Classes**:
    - [ ] Definition syntax.
    - [ ] Member access (`obj.field`).
    - [ ] Methods.

### Advanced Flow
- [ ] **Pattern Matching**: `switch` is basic; destructuring missing.
- [ ] **Exceptions**: `try/catch/throw` syntax and unwinding (landingpad).

---

## 🛠️ Priority 4: Runtime Library & Tooling
*Required for "Production Grade".*

### Runtime (Standard Library)
- [ ] **Memory Manager**:
    - [ ] "Unique Memory System": Need a GC or Reference Counting or Region-based system. Current: none (stack only).
    - [ ] `malloc`/`free` wrappers.
- [ ] **IO Library**:
    - [x] `print`/`println` (Basic wrappers).
    - [ ] `input` (Basic read).
    - [ ] File I/O (`open`, `read`, `write`).
- [ ] **String Library**:
    - [ ] Length, substring, find, split.

### Optimizations
- [ ] **IR Optimization**:
    - [ ] `mem2reg` pass (promote `alloca` to registers).
    - [ ] Constant Folding (compile-time math).
    - [ ] Dead Code Elimination.

---

## 📝 Summary of Critical Failures to Address Immediately
1.  **Float Arithmetic**: Fixed `genBinaryExpr` to use `fadd`, `fsub`, `fmul`, `fdiv` when operands are floats.
2.  **Type Checker**: Must implement a pass *before* CodeGen to assign types to expressions and handle errors nicely.
3.  **Arrays**: Implement parsing and generation for array literals.

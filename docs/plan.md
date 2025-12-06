# Compiler Implementation Plan

This document outlines the roadmap for implementing the compiler for our new language, based on the [Syntax Specification](syntax.md).

### We start with scalable infrastructure and lexer. Then we move to parser and AST. Then we move to code generation. afterwards we add features and optimize the code generation.

## Phase 1: Infrastructure & Lexer
- [x] **Project Setup**
    - [x] Initialize CMake/Make build system
    - [x] Set up directory structure (`src/`, `include/`, `tests/`, `examples/`)
    - [x] Add FileReader utility for reading source files
    - [x] Command-line argument support for file input
- [x] **Lexer Implementation**
    - [x] Define `Token` structure (Type, Value, Location)
    - [x] Implement state machine/regex for tokenization
    - [x] Support all keywords: `fn`, `return`, `if`, `else`, `while`, `for`, `var`, `const`, `import`
    - [x] Support types: `int`, `float`, `string`, `bool`, `void`, `list`
    - [x] Support literals: Integers, Floats, Strings (with interpolation), Booleans
    - [x] Support operators & delimiters
- [x] **Testing**
    - [x] Create comprehensive test files (lexer, control flow, functions, expressions, collections)

## Phase 2: AST & Basic Parser
- [x] **AST Design**
    - [x] Define node hierarchy: `Expr`, `Stmt`, `Decl`
- [x] **Parser Core**
    - [x] Implement Recursive Descent scaffolding
- [x] **Expression Parsing**
    - [x] Binary/Unary operations with precedence
    - [x] Parentheses and grouping
- [x] **Statement Parsing**
    - [x] Variable declarations (`int x = 5`)
    - [x] Assignments (`x = 5`)
    - [x] Expression statements

## Phase 3: Control Flow & Basic Functions
- [x] **Control Flow**
    - [x] `if`, `else`, `else if`
    - [x] `while` loops
    - [x] `for` loops (C-style, Range, Collection)
    - [x] `switch` statements
    - [x] `break`, `continue`, `return`
- [x] **Functions**
    - [x] Function definitions (`fn name(args) type { ... }`)
    - [x] Function calls

## Phase 4: Code Generation
- [x] **Backend Setup**
    - [x] LLVM IR Generation (Text-based)
- [ ] **Implementation**
    - [x] Generate LLVM IR code for basic AST nodes
    - [ ] **Missing Operators**:
        - [x] Arithmetic: `%`
        - [] Comparison: `!=`, `<=`, `>=`
        - [ ] Logical: `&&`, `||`, `!`
        - [ ] Bitwise: `&`, `|`, `^`, `~`, `<<`, `>>`
    - [x] **Control Flow**:
        - [x] `break`, `continue` implementation
    - [x] **Variadic Print**:
        - [x] Support `print(a, b, c)` syntax by unrolling to multiple runtime calls
- [x] **Runtime**
    - [x] Implement basic I/O (`print`, `input`)
    - [ ] Memory management for dynamic types (Basic stack allocation implemented)
    - [ ] Type checking

## Phase 5: Advanced Features
- [x] **Functions**
    - [x] Arrow functions (`=>`)
    - [x] Default parameters
- [x] **Ternary Operator**
    - [x] Ternary expressions (`condition ? true : false`)
- [x] **Array & Range Literals**
    - [x] Range syntax (`[start:stop]` and `[start:stop:step]`)
    - [ ] Array literals (`[1, 2, 3]`)
- [ ] **Collections**
    - [ ] Fixed-size arrays (`int[]`)
    - [ ] Dynamic lists (`list`)
    - [ ] Indexing `arr[i]`
- [ ] **Strings**
    - [ ] String interpolation parsing
- [ ] **Modules**
    - [x] `import` statement parsing
    - [x] multiple files support
- [ ] **Error Handling**
    - [ ] Better error messages with line numbers
- [ ] **Type Conversions**
    - [ ] `int()`, `float()`, `string()` syntax
    - [ ] Type inference
    - [ ] Type checking in parser

## Phase 6: Semantic Analysis
- [ ] **Symbol Table**
    - [ ] Scope management (global, function, block)
- [ ] **Type Checking**
    - [ ] Static type verification
    - [ ] `const` correctness
    - [ ] Type inference for `const`
- [ ] **Validation**
    - [ ] Unreachable code detection
    - [ ] Return path validation

## Phase 7: Polish & Future
- [ ] **Optimization**
    - [ ] Basic optimizations
- [ ] **Future Features**
    - [ ] Classes, Structs, Enums (as per `syntax.md` future notes)
    - [ ] Overloading function names based on parameters types

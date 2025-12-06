# Language Syntax Specification

> **Developer Reference**: This document defines the complete syntax for implementing the compiler.

---

## 1. Imports

```cpp
// Import standard library modules
import math;
import io;

// Import local files (relative paths)
import "../inc/file.lang";
import "./utils.lang";
```

---

## 2. Comments

```cpp
// Line comment

/* Block comment
   spanning multiple lines */
```

---

## 3. Primitive Types

### Core Types
- `void` - Void type (return type)
- `int` - Integer (default size)
- `float` - Floating point number
- `char` - Single character
- `bool` - Boolean (`true` or `false`)
- `string` - String type
- `list` - Dynamic array/list type
- `var` - Dynamic type (pointer in core)

> **Future**: Support for integer sizes (`int8`, `int16`, `int32`, `int64`) and unsigned variants (`uint`, etc.) will be added later.

---

## 4. Variables

### Type Inference Rules
- **Type inference**: Only allowed with `const` keyword
- **Explicit types required**: All mutable variables and function parameters/returns

### Declaration Syntax

```cpp
// Mutable variables (explicit type required)
int x = 10;
float pi = 3.14;
string name = "Alice";
bool flag = true;

// Immutable variables (type inference allowed)
const y = .42;              // Type inferred as float (0.42)
const message = "Hello";   // Type inferred as string
const string z = "World";  // Explicit type also allowed

// Dynamic type variable
var dynamic = 10;          // Type determined at runtime
dynamic = "text";          // Can change type
```

### Null Values
- `null` is supported for all types

---

## 5. Operators

### Arithmetic
`+` `-` `*` `/` `%`

### Comparison
`==` `!=` `<` `>` `<=` `>=`

### Logical
`&&` `||` `!`

### Bitwise
`&` `|` `^` `~` `<<` `>>`

### Assignment
`=` `+=` `-=` `*=` `/=` `%=`

### Increment/Decrement
`++`
`--`

### Ternary Operator
`condition ? true_value : false_value`

```cpp
int max = (a > b) ? a : b;
string status = (age >= 18) ? "adult" : "minor";
```

---

## 6. Control Flow

### If-Else
```cpp
// With braces
if (condition) {
    // code
} else if (other_condition) {
    // code
} else {
    // code
}

// Single statement (no braces required)
if (x > 10) return 1;
if (condition) doSomething();
```

### While Loop
```cpp
// With braces
while (condition) {
    // code
}

// Single statement
while (running) update();
```

### For Loops
```cpp
// C-style for loop
for (int i = 0; i < 10; i++) {
    // code
}

// Range with parameters
for (int i : [0:10:2]) { // start, end, step (step is default to 1)
    // code
}

// Collection iteration
for (var item : collection) {
    // code
}

// Single statement
for (int i : 10) print(i);
```

### Break and Continue
```cpp
// Break - exit loop immediately
for (int i : 10) {
    if (i == 5) break;
    print(i);
}

// Continue - skip to next iteration
for (int i : 10) {
    if (i % 2 == 0) continue;
    print(i);  // Only prints odd numbers
}
```

### Switch Statement
**Modern style** (automatic break, no fallthrough by default)
```cpp
switch (value) {
    case 1 {
        // code
    }
    case 2 {
        // code
    }
    default {
        // code
    }
}
```

---

## 7. Functions

### Standard Syntax
**Use `fn` keyword with `returnType`/`(returnType1, returnType2)` syntax. Default return type is `void`.**

```cpp
// Function with return type
fn add(int a, int b) int {
    return a + b;
}

// Multiple return values
fn swap(var a, var b) (var, var) {
    return b, a;
}

// Function with void return (default, can omit : void)
fn greet(string name) {
    print("Hello, " + name);
}

// Explicit void return
fn doSomething() void {
    // code
}
```

### Arrow Functions
**Optional shorter syntax** for simple expressions:
```cpp
fn square(int x) int => x * x;
```

### Default Parameters
```cpp
fn multiply(int a, int b = 1) int {
    return a * b;
}
```

> **Future**: Lambda/anonymous functions will be added later.

---

## 8. Arrays & Lists

### Fixed-Size Arrays
**C-style arrays with fixed size (no dynamic operations)**

```cpp
// Mutable fixed-size array (size inferred from literal)
int[] arr = [1, 2, 3];

// Immutable array (type and size inferred)
const arr = [0, 2, 4];
const arr2 = [0:10:2];

// Explicit fixed-size array
int[10] numbers;  // Array of 10 ints, default initialized to 0

// Multidimensional fixed-size arrays
int[10][10] table;  // 10x10 array, default 0 for all elements
```

### Dynamic Lists
**Dynamic-size collections with `list` keyword**

```cpp
// Mutable dynamic list (var type - can hold mixed types)
list l = [];

// Dynamic list with initial values
list items = [1, 2, 3];  // Type defaults to var

// Dynamic list with explicit type
list<int> numbers = [0, 2, 4];  // Type explicit int
```

### Access & Modification
```cpp
// Arrays - fixed size, no dynamic operations
int[] arr = [1, 2, 3];
int x = arr[0];     // Access element
arr[1] = 5;         // Modify element (if mutable)

// Lists - dynamic size, supports operations
list l = [1, 2, 3];
l += 4;             // Push back
l += [5, 6];        // Push back multiple elements
l.remove(2);        // Remove by index
int y = l[0];       // Access element
```

> **Future**: Additional array operations, slicing, typed lists, and specialized collection types will be added later.

---

## 9. Strings

### String Literals
```cpp
string text = "Hello, World!";
char c = 'a';
```

### String Interpolation
**Python-style f-strings formatting but default:**
```cpp
string name = "Alice";
int age = 30;
string message = "Name: {name}, Age: {age}, \{1+2\}";
println(message, " = 3");
```

### Escape Sequences
Standard C-style escape sequences: `\n` `\t` `\\` `\"` `\'` `\{` `\}`

> **Future**: More escape sequences such as hex and unicode will be added later.

---

## 10. Type Conversions

### Built-in Conversion Functions
- **basic conversion**: `type(value)` for similar number types, always works
- **complex conversion**: `value.toType()` with strings mainly. may fail and return `null`

```cpp
// Basic conversion
int(3.14)      // 3
int('A')       // 65 (ASCII value)
int(true)      // 1

float(13)      // 13.0
float('A')     // 65.0 (ASCII value)
float(true)    // 1.0

bool(1)        // true
bool(0)        // false

char(72)       // 'H' (from ASCII)

// Complex conversion
"42".toInteger()      // 42
"3.14".toFloat()  // 3.14

(135).toString()    // "135"
(3.14).toString()   // "3.14"

"false".toBoolean()       // false
"true".toBoolean()   // true
```

---

## 11. Basic I/O

### Output Functions
```cpp
// Print without newline
print("Hello");
print(42);
print(3.14);

// Print with newline
println("Hello, World!");
println(x + y);

// Print multiple values
println("Value: ", x, ", Type: ", typeof(x));

// Print formatted (with f-strings)
println(f"Value: {x}");
```

### Input Functions
```cpp
// Read input as string
string name = input();
string line = input("Enter your name: ");  // With prompt

// Read and convert
int age = int(input("Enter age: "));
float price = float(input("Enter price: "));
```

---

## 12. Type System Summary

### Type Inference Rules
| Declaration | Type Inference | Example |
|------------|----------------|---------|
| `const` | ✅ Allowed | `const x = 10;` |
| `int`, `float`, etc. | ❌ Explicit only | `int x = 10;` |
| `var` | ✅ Dynamic | `var x = 10;` |
| Function params | ❌ Explicit required | `fn add(int a, int b) int` |
| Function return | ❌ Explicit required | `fn getValue() int` |

### Mutability
- **Mutable by default**: Variables declared with type (`int x = 10;`)
- **Immutable**: Variables declared with `const` (`const x = 10;`)
- **Dynamic**: Variables declared with `var` (runtime type)

---

## 13. Program Structure

### Basic Program
```cpp
// Imports
import math;
import "utils.lang";

// Global variables
int counter = 0;

// Functions
fn main() int {
    // Entry point
    println("Hello, World!");
    return 0;
}
```

---

## 14. Token Reference

### Keywords

**Control Flow:**
`if` `else` `while` `for` `switch` `case` `default` `break` `continue` `return`

**Type Keywords:**
`void` `int` `float` `char` `bool` `string` `list` `var` `const`

**Function:**
`fn`

**Module System:**
`import`

**Literals:**
`true` `false` `null`

**Built-in Functions:**
`print` `println` `input`

### Operators

**Arithmetic:**
`+` `-` `*` `/` `%`

**Comparison:**
`==` `!=` `<` `>` `<=` `>=`

**Logical:**
`&&` `||` `!`

**Bitwise:**
`&` `|` `^` `~` `<<` `>>`

**Assignment:**
`=` `+=` `-=` `*=` `/=` `%=`

**Increment/Decrement:**
`++` `--`

**Ternary:**
`?` `:`

**Arrow (Function):**
`=>`

**Range:**
`:`

### Delimiters & Symbols

**Parentheses:**
`(` `)` - Grouping, function calls, function parameters

**Braces:**
`{` `}` - Code blocks, switch cases, string formatting values

**Brackets:**
`[` `]` - Array/list literals, indexing, array type declaration

**Angle Brackets:**
`<` `>` - Generic type parameters (e.g., `list<int>`)

**Separators:**
`;` - Statement terminator
`,` - Parameter/argument separator
`.` - Member access (e.g., `.toInteger()`)

**Escape:**
`\` - Escape character in strings

---

## 15. Statement Syntax Reference

### Basic Statements (Fully Specified)

#### Variable Declaration (Expression because returns identifier)
```rust
<type> <identifier> = <expression>;
const <type> <identifier> = <expression>;
```

#### Assignment
```rust
<identifier> <compound_op> <expression>;  // =, +=, -=, *=, /=, %=
<identifier>++;
<identifier>--;
```

#### Function Declaration
```rust
fn <identifier>(<parameters?>) <return_type?> { <statements> }
fn <identifier>(<parameters?>) <return_type?> => <expression>;
```

#### If Statement
```rust
if (<expression>) <statement> // statement can be code block {<statements>}
if (<expression>) <statement> else <statement> // second statement can be if statement (if () {} else if () {})
```

#### While Loop
```rust
while (<expression>) <statement>
```

#### For Loop
```rust
for (<expression>; <expression>; <expression>) <statement>
for (<identifier> : <expression>) <statement> // identifier can be declaration, expression must be integer
for (<identifier> : <identifier>) <statement> // identifier can be declaration, identifier must be iterable
```

#### Switch Statement
```cpp
switch (<expression>) {
    case <expression> <statement>
    case <expression> <statement>
    default <statement>
}
```

#### Control Flow
```rust
break;
continue;
return;
return <expression>;
return <expression>, <expression>;  // Multiple return values
```

#### Import
```python
import <module_name>;
import "<file_path>";
```

#### Function Call
```rust
<identifier>(<arguments?>);
<expression>.<method>(<arguments?>);  // Method call
```

#### Expression Statement
```rust
<expression>;
```

### Expressions (Basic Patterns)

```rust
<literal>                                    // 42, 3.14, "hello", true, null
<identifier>                                 // variable name
<expression> <binary_op> <expression>        // a + b, x == y
<unary_op> <expression>                      // !flag, -value
<expression> ? <expression> : <expression>   // ternary
<function>(<arguments?>)                      // function call
<type>(<expression>)                         // type conversion
<expression>.<method>(<arguments?>)           // method call
<expression>[<expression>]                   // array/list indexing
[<elements>]                                 // array/list literal
"<string with {<expression>}>"             // format string
(<expression>)                               // grouping
<type> <identifier> = <expression>;         // variable declaration (expression because returns identifier)
<identifier> = <expression>;                // assignment (expression because returns identifier)
```

### Not Yet Fully Specified

> **Note**: The following statement types exist in the language but their complete syntax specifications will be added as they are implemented:

- **Array/List operations**: Advanced indexing, slicing, comprehensions
- **Pattern matching**: Advanced switch with destructuring
- **Exception handling**: try/catch/finally blocks
- **Class declarations**: class, interface, enum syntax
- **Struct declarations**: struct syntax
- **Lambda expressions**: Anonymous function syntax
- **Async/await**: Asynchronous programming constructs
- **Attribute/annotation**: Decorator syntax
- **Type declarations**: Type aliases, union types

---

## Notes for Future Features

### Type System & Data Structures
- **Not yet implemented**: Classes, enums, interfaces
- **Not yet implemented**: Structs/records
- **Not yet implemented**: Tuples (distinct from multiple return values)
- **Not yet implemented**: Integer size variants (int8, int16, int32, int64, uint, etc.)
- **Not yet implemented**: Generics/templates (note: basic list<T> syntax added)
- **Not yet implemented**: Maps/dictionaries/hashmaps
- **Not yet implemented**: Sets
- **Not yet implemented**: Union types
- **Not yet implemented**: Type aliases

### Functions & Closures
- **Not yet implemented**: Lambda/anonymous functions
- **Not yet implemented**: Closures with capture semantics
- **Not yet implemented**: Function overloading
- **Not yet implemented**: Operator overloading
- **Not yet implemented**: Variadic functions (variable arguments)

### Control Flow & Error Handling
- **Not yet implemented**: Exception handling (try/catch/finally/throw)
- **Not yet implemented**: Result/Option types for error handling
- **Not yet implemented**: Defer statement (Go-style)
- **Not yet implemented**: Pattern matching (advanced switch)
- **Not yet implemented**: Guard clauses

### Collections & Iterators
- **Not yet implemented**: Advanced collection operations (map, filter, reduce, etc.)
- **Not yet implemented**: List/array slicing and range operations
- **Not yet implemented**: List comprehensions
- **Not yet implemented**: Destructuring assignments
- **Not yet implemented**: Spread/rest operators

### Memory & Pointers
- **Not yet implemented**: Explicit pointer and reference syntax
- **Not yet implemented**: Memory management keywords (new, delete, free, etc.)
- **Not yet implemented**: Ownership/borrowing system (Rust-style)
- **Not yet implemented**: Smart pointers
- **Not yet implemented**: Type casting and conversion operators

### Concurrency & Async
- **Not yet implemented**: Threading primitives
- **Not yet implemented**: Async/await syntax
- **Not yet implemented**: Promises/futures
- **Not yet implemented**: Channels (Go-style)
- **Not yet implemented**: Atomic operations
- **Not yet implemented**: Mutex/lock primitives

### Modules & Organization
- **Not yet implemented**: Namespaces
- **Not yet implemented**: Module system (beyond basic imports)
- **Not yet implemented**: Visibility/access modifiers (public, private, protected)
- **Not yet implemented**: Package management

### Utilities & Built-ins
- **Not yet implemented**: Regular expressions
- **Not yet implemented**: Assertions and debugging tools
- **Not yet implemented**: Reflection/introspection
- **Not yet implemented**: Attributes/annotations/decorators
- **Not yet implemented**: Macros/metaprogramming
- **Not yet implemented**: JSON/serialization support
- **Not yet implemented**: File I/O operations
- **Not yet implemented**: Standard library documentation
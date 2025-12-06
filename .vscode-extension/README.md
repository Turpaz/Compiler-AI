# Custom Language Syntax Highlighting

Syntax highlighting extension for `.lang` files in Visual Studio Code.

## Features

- **Syntax Highlighting**: Full syntax highlighting for the custom language
- **Auto-completion**: Bracket and quote auto-closing
- **Comments**: Line (`//`) and block (`/* */`) comment support
- **Code Folding**: Support for code blocks

## Installation

### Option 1: Copy to Extensions Folder (Recommended)
1. Copy the `.vscode-extension` folder to your VSCode extensions directory:
   - **Windows**: `%USERPROFILE%\.vscode\extensions\custom-lang-syntax-1.0.0\`
   - **macOS/Linux**: `~/.vscode/extensions/custom-lang-syntax-1.0.0/`
2. Restart VSCode
3. Open any `.lang` file to see syntax highlighting

### Option 2: Load as Development Extension
1. Open VSCode
2. Press `F5` to open a new Extension Development Host window
3. Open any `.lang` file to test the syntax highlighting

## Customization

The extension can be easily customized by editing the following files:

- **`syntaxes/customlang.tmLanguage.json`**: Modify syntax patterns and scopes
- **`language-configuration.json`**: Change bracket pairs, comments, etc.
- **`package.json`**: Update extension metadata

### Customizing Colors

To customize syntax colors, add theme overrides to your `settings.json`:

```json
{
  "editor.tokenColorCustomizations": {
    "textMateRules": [
      {
        "scope": "keyword.control.customlang",
        "settings": {
          "foreground": "#C586C0"
        }
      },
      {
        "scope": "entity.name.function.customlang",
        "settings": {
          "foreground": "#DCDCAA"
        }
      }
    ]
  }
}
```

## Language Features

Supported syntax elements:

- Keywords: `if`, `else`, `while`, `for`, `fn`, `return`, etc.
- Types: `int`, `float`, `bool`, `string`, `void`, etc.
- Modifiers: `const`, `var`
- Built-in functions: `print`, `println`, `input`
- Comments: `//` and `/* */`
- String interpolation: `{expression}`
- Arrow functions: `=>`
- Operators: Arithmetic, logical, comparison, bitwise

## License

MIT

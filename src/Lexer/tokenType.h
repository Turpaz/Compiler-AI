#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include <string>

using std::string;

enum class TokenType {
  // Keywords
  KW_FN,
  KW_RETURN,
  KW_IF,
  KW_ELSE,
  KW_WHILE,
  KW_FOR,
  KW_SWITCH,
  KW_CASE,
  KW_DEFAULT,
  KW_BREAK,
  KW_CONTINUE,
  KW_IMPORT,

  // Types
  TYPE_VOID,
  TYPE_CONST,
  TYPE_VAR,
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_CHAR,
  TYPE_BOOL,
  TYPE_STRING,
  TYPE_LIST,

  // Literals
  LIT_INTEGER,
  LIT_FLOAT,
  LIT_STRING,
  LIT_CHAR,
  LIT_TRUE,
  LIT_FALSE,
  LIT_NULL,

  // Identifiers
  IDENTIFIER,

  // Operators
  OP_PLUS,         // +
  OP_MINUS,        // -
  OP_MULTIPLY,     // *
  OP_DIVIDE,       // /
  OP_MODULO,       // %
  OP_ASSIGN,       // =
  OP_PLUS_ASSIGN,  // +=
  OP_MINUS_ASSIGN, // -=
  OP_MULT_ASSIGN,  // *=
  OP_DIV_ASSIGN,   // /=
  OP_MOD_ASSIGN,   // %=
  OP_INCREMENT,    // ++
  OP_DECREMENT,    // --

  // Comparison
  OP_EQUAL,         // ==
  OP_NOT_EQUAL,     // !=
  OP_LESS,          // <
  OP_GREATER,       // >
  OP_LESS_EQUAL,    // <=
  OP_GREATER_EQUAL, // >=

  // Logical
  OP_AND, // &&
  OP_OR,  // ||
  OP_NOT, // !

  // Bitwise
  OP_BIT_AND,     // &
  OP_BIT_OR,      // |
  OP_BIT_XOR,     // ^
  OP_BIT_NOT,     // ~
  OP_SHIFT_LEFT,  // <<
  OP_SHIFT_RIGHT, // >>

  // Ternary
  QUESTION, // ?
  COLON,    // : // Also range operator

  // Delimiters
  LPAREN,    // (
  RPAREN,    // )
  LBRACE,    // {
  RBRACE,    // }
  LBRACKET,  // [
  RBRACKET,  // ]
  SEMICOLON, // ;
  COMMA,     // ,
  DOT,       // .
  ARROW,     // =>

  // Special
  END_OF_FILE,
  UNKNOWN,
  ERROR,
};

inline string TokenTypeToString(TokenType type) {
  switch (type) {
  case TokenType::KW_FN:
    return "KW_FN";
  case TokenType::KW_RETURN:
    return "KW_RETURN";
  case TokenType::KW_IF:
    return "KW_IF";
  case TokenType::KW_ELSE:
    return "KW_ELSE";
  case TokenType::KW_WHILE:
    return "KW_WHILE";
  case TokenType::KW_FOR:
    return "KW_FOR";
  case TokenType::KW_SWITCH:
    return "KW_SWITCH";
  case TokenType::KW_CASE:
    return "KW_CASE";
  case TokenType::KW_DEFAULT:
    return "KW_DEFAULT";
  case TokenType::KW_BREAK:
    return "KW_BREAK";
  case TokenType::KW_CONTINUE:
    return "KW_CONTINUE";
  case TokenType::KW_IMPORT:
    return "KW_IMPORT";
  case TokenType::TYPE_VOID:
    return "TYPE_VOID";
  case TokenType::TYPE_CONST:
    return "TYPE_CONST";
  case TokenType::TYPE_VAR:
    return "TYPE_VAR";
  case TokenType::TYPE_INT:
    return "TYPE_INT";
  case TokenType::TYPE_FLOAT:
    return "TYPE_FLOAT";
  case TokenType::TYPE_CHAR:
    return "TYPE_CHAR";
  case TokenType::TYPE_BOOL:
    return "TYPE_BOOL";
  case TokenType::TYPE_STRING:
    return "TYPE_STRING";
  case TokenType::TYPE_LIST:
    return "TYPE_LIST";
  case TokenType::LIT_INTEGER:
    return "LIT_INTEGER";
  case TokenType::LIT_FLOAT:
    return "LIT_FLOAT";
  case TokenType::LIT_STRING:
    return "LIT_STRING";
  case TokenType::LIT_CHAR:
    return "LIT_CHAR";
  case TokenType::LIT_TRUE:
    return "LIT_TRUE";
  case TokenType::LIT_FALSE:
    return "LIT_FALSE";
  case TokenType::LIT_NULL:
    return "LIT_NULL";
  case TokenType::IDENTIFIER:
    return "IDENTIFIER";
  case TokenType::OP_PLUS:
    return "OP_PLUS";
  case TokenType::OP_MINUS:
    return "OP_MINUS";
  case TokenType::OP_MULTIPLY:
    return "OP_MULTIPLY";
  case TokenType::OP_DIVIDE:
    return "OP_DIVIDE";
  case TokenType::OP_MODULO:
    return "OP_MODULO";
  case TokenType::OP_ASSIGN:
    return "OP_ASSIGN";
  case TokenType::OP_PLUS_ASSIGN:
    return "OP_PLUS_ASSIGN";
  case TokenType::OP_MINUS_ASSIGN:
    return "OP_MINUS_ASSIGN";
  case TokenType::OP_MULT_ASSIGN:
    return "OP_MULT_ASSIGN";
  case TokenType::OP_DIV_ASSIGN:
    return "OP_DIV_ASSIGN";
  case TokenType::OP_MOD_ASSIGN:
    return "OP_MOD_ASSIGN";
  case TokenType::OP_INCREMENT:
    return "OP_INCREMENT";
  case TokenType::OP_DECREMENT:
    return "OP_DECREMENT";
  case TokenType::OP_EQUAL:
    return "OP_EQUAL";
  case TokenType::OP_NOT_EQUAL:
    return "OP_NOT_EQUAL";
  case TokenType::OP_LESS:
    return "OP_LESS";
  case TokenType::OP_GREATER:
    return "OP_GREATER";
  case TokenType::OP_LESS_EQUAL:
    return "OP_LESS_EQUAL";
  case TokenType::OP_GREATER_EQUAL:
    return "OP_GREATER_EQUAL";
  case TokenType::OP_AND:
    return "OP_AND";
  case TokenType::OP_OR:
    return "OP_OR";
  case TokenType::OP_NOT:
    return "OP_NOT";
  case TokenType::OP_BIT_AND:
    return "OP_BIT_AND";
  case TokenType::OP_BIT_OR:
    return "OP_BIT_OR";
  case TokenType::OP_BIT_XOR:
    return "OP_BIT_XOR";
  case TokenType::OP_BIT_NOT:
    return "OP_BIT_NOT";
  case TokenType::OP_SHIFT_LEFT:
    return "OP_SHIFT_LEFT";
  case TokenType::OP_SHIFT_RIGHT:
    return "OP_SHIFT_RIGHT";
  case TokenType::QUESTION:
    return "QUESTION";
  case TokenType::COLON:
    return "COLON";
  case TokenType::LPAREN:
    return "LPAREN";
  case TokenType::RPAREN:
    return "RPAREN";
  case TokenType::LBRACE:
    return "LBRACE";
  case TokenType::RBRACE:
    return "RBRACE";
  case TokenType::LBRACKET:
    return "LBRACKET";
  case TokenType::RBRACKET:
    return "RBRACKET";
  case TokenType::SEMICOLON:
    return "SEMICOLON";
  case TokenType::COMMA:
    return "COMMA";
  case TokenType::DOT:
    return "DOT";
  case TokenType::ARROW:
    return "ARROW";
  case TokenType::END_OF_FILE:
    return "END_OF_FILE";
  case TokenType::UNKNOWN:
    return "UNKNOWN";
  case TokenType::ERROR:
    return "ERROR";
  default:
    return "UNKNOWN";
  }
}

#endif
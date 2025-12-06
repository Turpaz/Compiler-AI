#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class Lexer {
public:
  Lexer(const string &source);
  vector<Token> tokenize();
  
  // Utility method to print tokens
  static void printTokens(const vector<Token>& tokens);

private:
  string source;
  size_t position;
  char currentChar;
  Location location;

  Token nextToken();
  void advance();
  char peek(int offset = 1);
  void skipWhitespace();
  void skipLineComment();
  void skipBlockComment();

  Token makeToken(TokenType type, const string &value);
  Token readNumber();
  Token readString();
  Token readChar();
  Token readIdentifierOrKeyword();
  Token readOther();

  bool isAtEnd(int offset = 0);
  bool isDigit(char c);
  bool isAlpha(char c);
  bool isAlphaNumeric(char c);

  string account_for_special_characters(const string &og);
};

#endif

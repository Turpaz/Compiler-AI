#ifndef TOKEN_H
#define TOKEN_H

#include <string>

#include "tokenType.h"

using std::string;

struct Location {
  int line;
  int column;

  Location(int l = 1, int c = 1) : line(l), column(c) {}
};

struct Token {
  TokenType type;
  string value;
  Location location;

  Token(TokenType t, const string &v, const Location &loc)
      : type(t), value(v), location(loc) {}

  Token() : type(TokenType::UNKNOWN), value(""), location() {}
};

#endif

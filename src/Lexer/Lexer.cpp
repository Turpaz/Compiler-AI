#include "Lexer.h"
#include <cctype>
#include <unordered_map>
#include <iostream>

static const std::unordered_map<string, TokenType> keywords = {
    {"fn", TokenType::KW_FN},
    {"return", TokenType::KW_RETURN},
    {"if", TokenType::KW_IF},
    {"else", TokenType::KW_ELSE},
    {"while", TokenType::KW_WHILE},
    {"for", TokenType::KW_FOR},
    {"switch", TokenType::KW_SWITCH},
    {"case", TokenType::KW_CASE},
    {"default", TokenType::KW_DEFAULT},
    {"break", TokenType::KW_BREAK},
    {"continue", TokenType::KW_CONTINUE},
    {"import", TokenType::KW_IMPORT},
    {"void", TokenType::TYPE_VOID},
    {"const", TokenType::TYPE_CONST},
    {"var", TokenType::TYPE_VAR},
    {"int", TokenType::TYPE_INT},
    {"float", TokenType::TYPE_FLOAT},
    {"char", TokenType::TYPE_CHAR},
    {"bool", TokenType::TYPE_BOOL},
    {"string", TokenType::TYPE_STRING},
    {"list", TokenType::TYPE_LIST},
    {"true", TokenType::LIT_TRUE},
    {"false", TokenType::LIT_FALSE},
    {"null", TokenType::LIT_NULL}};

Lexer::Lexer(const string &src)
    : source(src), position(0), location(1, 1) {
  currentChar = source.empty() ? '\0' : source[0];
}

vector<Token> Lexer::tokenize() {
  vector<Token> tokens;

  Token token = nextToken();
  while (token.type != TokenType::END_OF_FILE) {
    tokens.push_back(token);
    token = nextToken();
  }
  tokens.push_back(token); // Add EOF token

  return tokens;
}

Token Lexer::nextToken() {
  skipWhitespace();

  if (isAtEnd()) {
    return makeToken(TokenType::END_OF_FILE, "");
  }

  char c = currentChar;

  // Comments
  if (c == '/' && peek() == '/') {
    skipLineComment();
    return nextToken();
  }
  if (c == '/' && peek() == '*') {
    advance(); // skip /
    skipBlockComment();
    return nextToken();
  }
  // Number literal
  if (isDigit(c) || (c == '.' && isDigit(peek()))) {
    return readNumber();
  }
  // String literal
  if (c == '"') {
    return readString();
  }
  // Character literal
  if (c == '\'') {
    return readChar();
  }
  // Identifier or keyword
  if (isAlpha(c) || c == '_') {
    return readIdentifierOrKeyword();
  }

  return readOther();
}


void Lexer::advance() {
  if (isAtEnd()) {
    currentChar = '\0';
    return;
  }

  if (currentChar == '\n') {
    location.line++;
    location.column = 1;
  } else {
    location.column++;
  }

  position++;
  currentChar = (!isAtEnd()) ? source[position] : '\0';
}

char Lexer::peek(int offset) {
  size_t peekPos = position + offset;
  return (!isAtEnd(offset)) ? source[peekPos] : '\0';
}

bool Lexer::isAtEnd(int offset) { return position + offset >= source.length(); }

bool Lexer::isDigit(char c) { return c >= '0' && c <= '9'; }

bool Lexer::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

void Lexer::skipWhitespace() {
  while (currentChar == ' ' || currentChar == '\t' || currentChar == '\n' ||
         currentChar == '\r') {
    advance();
  }
}

void Lexer::skipLineComment() {
  // Skip //
  while (currentChar != '\n' && !isAtEnd()) {
    advance();
  }
}

void Lexer::skipBlockComment() {
  // Skip /* ... */
  advance(); // skip *
  while (!isAtEnd()) {
    if (currentChar == '*' && peek() == '/') {
      advance(); // skip *
      advance(); // skip /
      break;
    }
    advance();
  }
}

Token Lexer::makeToken(TokenType type, const string &value) {
  return Token(type, value, location);
}

Token Lexer::readNumber() {
  string number;
  Location startLoc = location;

  if (currentChar == '.')
	{
		number += '0';
	}

  while (isDigit(currentChar)) {
    number += currentChar;
    advance();
  }

  // Check for float
  if (currentChar == '.' && isDigit(peek())) {
    number += currentChar;
    advance();
    while (isDigit(currentChar)) {
      number += currentChar;
      advance();
    }

    return Token(TokenType::LIT_FLOAT, number, startLoc);
  }
  else if ((currentChar == 'x' || currentChar == 'X') && number == "0") // Support for hex
	{
		number += currentChar;
		advance();
		while (isDigit(currentChar) || (currentChar >= 'a' && currentChar <= 'f') || (currentChar >= 'A' && currentChar <= 'F'))
		{
			number += currentChar;
			advance();
		}
		return Token(TokenType::LIT_INTEGER, number, startLoc);
	}
	else if ((currentChar == 'b' || currentChar == 'B') && number == "0") // Support for binary
	{
		number += currentChar;
		advance();
		while (currentChar == '0' || currentChar == '1')
		{
			number += currentChar;
			advance();
		}
		return Token(TokenType::LIT_INTEGER, number, startLoc);
	}

  return Token(TokenType::LIT_INTEGER, number, startLoc);
}

Token Lexer::readString() {
  string str;
  Location startLoc = location;
  advance(); // skip opening "

  while (!isAtEnd() && (currentChar != '"' || (currentChar == '"' && peek(-1) == '\\'))) {
    str += currentChar;
    advance();
  }

  if (currentChar == '"') {
    advance(); // skip closing "
    str = account_for_special_characters(str);
    // TODO: Support formatted strings
    return Token(TokenType::LIT_STRING, str, startLoc);
  }

  return Token(TokenType::ERROR, "Unterminated string literal", startLoc);
}

Token Lexer::readChar() {
  string str;
  Location startLoc = location;
  advance(); // skip opening '

  while (!isAtEnd() && (currentChar != '\'' || (currentChar == '\'' && peek(-1) != '\\'))) {
    str += currentChar;
    advance();
  }

  if (currentChar == '\'') {
    advance(); // skip closing '
    str = account_for_special_characters(str);
    if (str.length() != 1) {
      return Token(TokenType::ERROR, "Invalid character literal " + str, startLoc);
    }
    return Token(TokenType::LIT_CHAR, str, startLoc);
  }

  return Token(TokenType::ERROR, "Unterminated character literal", startLoc);
}

Token Lexer::readIdentifierOrKeyword() {
  string identifier;
  Location startLoc = location;

  while (isAlphaNumeric(currentChar)) {
    identifier += currentChar;
    advance();
  }

  // Check if it's a keyword
  auto it = keywords.find(identifier);
  if (it != keywords.end()) {
    return Token(it->second, identifier, startLoc);
  }

  return Token(TokenType::IDENTIFIER, identifier, startLoc);
}

Token Lexer::readOther() {
  Location startLoc = location;

  char c = currentChar;
  advance();

  // ( ) { } [ ] ; , . ? : ~ ^
  switch (c) {
  case '(':
    return makeToken(TokenType::LPAREN, "(");
  case ')':
    return makeToken(TokenType::RPAREN, ")");
  case '{':
    return makeToken(TokenType::LBRACE, "{");
  case '}':
    return makeToken(TokenType::RBRACE, "}");
  case '[':
    return makeToken(TokenType::LBRACKET, "[");
  case ']':
    return makeToken(TokenType::RBRACKET, "]");
  case ';':
    return makeToken(TokenType::SEMICOLON, ";");
  case ',':
    return makeToken(TokenType::COMMA, ",");
  case '.':
    return makeToken(TokenType::DOT, ".");
  case '?':
    return makeToken(TokenType::QUESTION, "?");
  case ':':
    return makeToken(TokenType::COLON, ":");
  case '~':
    return makeToken(TokenType::OP_BIT_NOT, "~");
  case '^':
    return makeToken(TokenType::OP_BIT_XOR, "^");

  // + ++ += - -- -= * *= / /= % %= = == => ! != < << <<= > >> >>= & && | || 
  case '+':
    if (currentChar == '+') {
      advance();
      return Token(TokenType::OP_INCREMENT, "++", startLoc);
    } else if (currentChar == '=') {
      advance();
      return Token(TokenType::OP_PLUS_ASSIGN, "+=", startLoc);
    }
    return Token(TokenType::OP_PLUS, "+", startLoc);

  case '-':
    if (currentChar == '-') {
      advance();
      return Token(TokenType::OP_DECREMENT, "--", startLoc);
    } else if (currentChar == '=') {
      advance();
      return Token(TokenType::OP_MINUS_ASSIGN, "-=" , startLoc);
    }
    return Token(TokenType::OP_MINUS, "-", startLoc);

  case '*':
    if (currentChar == '=') {
      advance();
      return Token(TokenType::OP_MULT_ASSIGN, "*=" , startLoc);
    }
    return Token(TokenType::OP_MULTIPLY, "*", startLoc);

  case '/':
    if (currentChar == '=') {
      advance();
      return Token(TokenType::OP_DIV_ASSIGN, "/=", startLoc);
    }
    return Token(TokenType::OP_DIVIDE, "/", startLoc);

  case '%':
    if (currentChar == '=') {
      advance();
      return Token(TokenType::OP_MOD_ASSIGN, "%=", startLoc);
    }
    return Token(TokenType::OP_MODULO, "%", startLoc);

  case '=':
    if (currentChar == '=') {
      advance();
      return Token(TokenType::OP_EQUAL, "==", startLoc);
    } else if (currentChar == '>') {
      advance();
      return Token(TokenType::ARROW, "=>", startLoc);
    }
    return Token(TokenType::OP_ASSIGN, "=", startLoc);

  case '!':
    if (currentChar == '=') {
      advance();
      return Token(TokenType::OP_NOT_EQUAL, "!=", startLoc);
    }
    return Token(TokenType::OP_NOT, "!", startLoc);

  case '<':
    if (currentChar == '=') {
      advance();
      return Token(TokenType::OP_LESS_EQUAL, "<=", startLoc);
    } else if (currentChar == '<') {
      advance();
      return Token(TokenType::OP_SHIFT_LEFT, "<<", startLoc);
    }
    return Token(TokenType::OP_LESS, "<", startLoc);

  case '>':
    if (currentChar == '=') {
      advance();
      return Token(TokenType::OP_GREATER_EQUAL, ">=", startLoc);
    } else if (currentChar == '>') {
      advance();
      return Token(TokenType::OP_SHIFT_RIGHT, ">>", startLoc);
    }
    return Token(TokenType::OP_GREATER, ">", startLoc);

  case '&':
    if (currentChar == '&') {
      advance();
      return Token(TokenType::OP_AND, "&&", startLoc);
    }
    return Token(TokenType::OP_BIT_AND, "&", startLoc);

  case '|':
    if (currentChar == '|') {
      advance();
      return Token(TokenType::OP_OR, "||", startLoc);
    }
    return Token(TokenType::OP_BIT_OR, "|", startLoc);
  }

  return Token(TokenType::UNKNOWN, std::string(1, c), startLoc);
}

string Lexer::account_for_special_characters(const string &og)
{
	// take a string and replace all the special characters with their escape sequences
	// "\\n" => "\n" ( '\' + 'n' = '\n' all one character)
	// account for \n, \t, \r, \a, \b, \f, \v,
	// \[num] (ascii value to char), \x[hex num] (ascii value to char)
	string ret = "";
	for (size_t i = 0; i < og.size(); i++)
	{
		if (og[i] == '\\')
		{
			i++;
			if (i < og.size())
			{
				string temp = "";
				switch (og[i])
				{
				case 'n':
					ret += '\n';
					break;
				case 't':
					ret += '\t';
					break;
				case 'r':
					ret += '\r';
					break;
				case 'a':
					ret += '\a';
					break;
				case 'b':
					ret += '\b';
					break;
				case 'f':
					ret += '\f';
					break;
				case 'v':
					ret += '\v';
					break;
				case 'x':
					i++;
					while (i < og.size() && isxdigit(og[i]))
					{
						temp += og[i];
						i++;
					}
					ret += (char)std::stoi(temp, nullptr, 16);
					break;
				default:
					// either a number or just a character
					if (isdigit(og[i]))
					{
						while (i < og.size() && isdigit(og[i]))
						{
							temp += og[i];
							i++;
						}
						ret += (char)std::stoi(temp, nullptr, 10);
						break;
					}
					else
					{
						ret += og[i];
					}
				}
			}
		}
		else
		{
			ret += og[i];
		}
	}

	return ret;
}

void Lexer::printTokens(const vector<Token>& tokens) {
  std::cout << "Tokens:" << std::endl;
  std::cout << "-------" << std::endl;
  
  for (const auto &token : tokens) {
    std::cout << "Line " << token.location.line << ", Col "
              << token.location.column
              << ": Type=" << TokenTypeToString(token.type) << ", Value=\""
              << token.value << "\"" << std::endl;
  }
  
  std::cout << std::endl;
  std::cout << "Total tokens: " << tokens.size() << std::endl;
}
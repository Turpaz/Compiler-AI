#ifndef PARSER_H
#define PARSER_H

#include "../Lexer/Token.h"
#include "AST.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class Parser {
public:
  Parser(const vector<Token> &tokens);

  // Main parsing method
  unique_ptr<Program> parse();

  // Static utility to print AST
  static void printAST(const Program *prog);

private:
  vector<Token> tokens;
  size_t current;
  bool hadError;

  // ========== Helper Methods ==========
  Token peek() const;
  Token previous() const;
  bool isAtEnd() const;
  Token advance();
  bool check(TokenType type) const;
  bool match(const vector<TokenType> &types);
  Token consume(TokenType type, const string &message);
  void synchronize();
  void error(const Token &token, const string &message);

  // ========== Type Checking ==========
  bool isType(TokenType type) const;

  // ========== Top-Level Parsing ==========
  unique_ptr<FunctionDecl> parseFunctionDecl();
  unique_ptr<Stmt> parseStatement();

  // ========== Statement Parsing ==========
  unique_ptr<Stmt> parseVarDecl(bool requireSemicolon = true);
  unique_ptr<Stmt> parseExprStmt();
  unique_ptr<Stmt> parseBlock();
  unique_ptr<Stmt> parseIfStmt();
  unique_ptr<Stmt> parseWhileStmt();
  unique_ptr<Stmt> parseForStmt();
  unique_ptr<Stmt> parseSwitchStmt();
  unique_ptr<Stmt> parseReturnStmt();
  unique_ptr<Stmt> parseBreakStmt();
  unique_ptr<Stmt> parseContinueStmt();
  unique_ptr<Stmt> parseImportStmt();

  // ========== Expression Parsing (Precedence Climbing) ==========
  unique_ptr<Expr> parseExpression();
  unique_ptr<Expr> parseAssignment();
  unique_ptr<Expr> parseTernary();
  unique_ptr<Expr> parseLogicalOr();
  unique_ptr<Expr> parseLogicalAnd();
  unique_ptr<Expr> parseBitwiseOr();
  unique_ptr<Expr> parseBitwiseXor();
  unique_ptr<Expr> parseBitwiseAnd();
  unique_ptr<Expr> parseEquality();
  unique_ptr<Expr> parseComparison();
  unique_ptr<Expr> parseShift();
  unique_ptr<Expr> parseAddition();
  unique_ptr<Expr> parseMultiplication();
  unique_ptr<Expr> parseUnary();
  unique_ptr<Expr> parsePostfix();
  unique_ptr<Expr> parsePrimary();
};

#endif // PARSER_H

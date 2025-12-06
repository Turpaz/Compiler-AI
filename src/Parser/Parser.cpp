#include "Parser.h"
#include "AST.h"
#include "Lexer/tokenType.h"
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

Parser::Parser(const vector<Token> &toks)
    : tokens(toks), current(0), hadError(false) {}

unique_ptr<Program> Parser::parse() {
  auto program = std::make_unique<Program>();

  while (!isAtEnd()) {
    // Try to parse function declaration
    if (check(TokenType::KW_FN)) {
      auto func = parseFunctionDecl();
      if (func) {
        program->functions.push_back(std::move(func));
      }
    } else {
      // Parse as statement
      auto stmt = parseStatement();
      if (stmt) {
        program->statements.push_back(std::move(stmt));
      }
    }

    if (hadError) {
      synchronize();
      hadError = false;
    }
  }

  return program;
}

void Parser::printAST(const Program *prog) {
  if (prog) {
    cout << "\n=== AST ===" << endl;
    prog->print(0);
    cout << "===========" << endl;
  }
}

// ============================================================================
// HELPER METHODS
// ============================================================================

Token Parser::peek() const { return tokens[current]; }

Token Parser::previous() const { return tokens[current - 1]; }

bool Parser::isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }

Token Parser::advance() {
  if (!isAtEnd())
    current++;
  return previous();
}

bool Parser::check(TokenType type) const {
  if (isAtEnd())
    return false;
  return peek().type == type;
}

bool Parser::match(const vector<TokenType> &types) {
  for (TokenType type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

Token Parser::consume(TokenType type, const string &message) {
  if (check(type))
    return advance();

  error(peek(), message + ". (Expected " + TokenTypeToString(type) + ")");
  return peek();
}

void Parser::synchronize() {
  advance();

  while (!isAtEnd()) {
    if (previous().type == TokenType::SEMICOLON)
      return;

    switch (peek().type) {
    case TokenType::KW_FN:
    case TokenType::KW_IF:
    case TokenType::KW_WHILE:
    case TokenType::KW_FOR:
    case TokenType::KW_RETURN:
      return;
    default:
      break;
    }

    advance();
  }
}

void Parser::error(const Token &token, const string &message) {
  cerr << "Parse Error at " << token.location.line << ":"
       << token.location.column << ": [" << TokenTypeToString(token.type)
       << "] " << message << endl;
  hadError = true;
}

bool Parser::isType(TokenType type) const {
  return type == TokenType::TYPE_INT || type == TokenType::TYPE_FLOAT ||
         type == TokenType::TYPE_STRING || type == TokenType::TYPE_BOOL ||
         type == TokenType::TYPE_CHAR || type == TokenType::TYPE_VOID ||
         type == TokenType::TYPE_LIST || type == TokenType::TYPE_CONST ||
         type == TokenType::TYPE_VAR;
}

// ============================================================================
// TOP-LEVEL PARSING
// ============================================================================

unique_ptr<FunctionDecl> Parser::parseFunctionDecl() {
  Token fnToken = consume(TokenType::KW_FN, "Expected 'fn'");
  Token name = consume(TokenType::IDENTIFIER, "Expected function name");
  consume(TokenType::LPAREN, "Expected '(' after function name");

  // Parse parameters
  vector<FunctionDecl::Parameter> params;
  if (!check(TokenType::RPAREN)) {
    do {
      if (!isType(peek().type)) {
        error(peek(), "Expected parameter type");
        break;
      }
      Token paramType = advance();
      Token paramName =
          consume(TokenType::IDENTIFIER, "Expected parameter name");

      // Check for default parameter value
      unique_ptr<Expr> defaultValue = nullptr;
      if (match({TokenType::OP_ASSIGN})) {
        defaultValue = parseExpression();
      }

      FunctionDecl::Parameter param;
      param.type = paramType.value;
      param.name = paramName.value;
      param.defaultValue = std::move(defaultValue);
      params.push_back(std::move(param));
    } while (match({TokenType::COMMA}));
  }

  consume(TokenType::RPAREN, "Expected ')' after parameters");

  // Parse return type
  string returnType = "void";
  if (isType(peek().type)) {
    returnType = advance().value;
  }

  // Check for arrow function
  if (match({TokenType::ARROW})) {
    // Arrow function: fn name(params) type => expression;
    auto arrowBody = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after arrow function body");

    return std::make_unique<FunctionDecl>(
        returnType, name.value, std::move(params), nullptr, fnToken.location,
        true, std::move(arrowBody));
  }

  // Parse body
  auto body = parseBlock();
  if (!body)
    return nullptr;

  return std::make_unique<FunctionDecl>(
      returnType, name.value, std::move(params),
      unique_ptr<BlockStmt>(static_cast<BlockStmt *>(body.release())),
      fnToken.location);
}

unique_ptr<Stmt> Parser::parseStatement() {
  try {
    if (match({TokenType::SEMICOLON})) {
      return std::make_unique<EmptyStmt>(peek().location); // Empty statement
    }
    if (isType(peek().type)) {
      return parseVarDecl();
    }
    if (match({TokenType::LBRACE})) {
      current--; // Put back the brace
      return parseBlock();
    }
    if (match({TokenType::KW_IF})) {
      current--;
      return parseIfStmt();
    }
    if (match({TokenType::KW_WHILE})) {
      current--;
      return parseWhileStmt();
    }
    if (match({TokenType::KW_FOR})) {
      current--;
      return parseForStmt();
    }
    if (match({TokenType::KW_SWITCH})) {
      current--;
      return parseSwitchStmt();
    }
    if (match({TokenType::KW_RETURN})) {
      current--;
      return parseReturnStmt();
    }
    if (match({TokenType::KW_BREAK})) {
      current--;
      return parseBreakStmt();
    }
    if (match({TokenType::KW_CONTINUE})) {
      current--;
      return parseContinueStmt();
    }

    return parseExprStmt();
  } catch (...) {
    return nullptr;
  }
}

// ============================================================================
// STATEMENT PARSING
// ============================================================================

unique_ptr<Stmt> Parser::parseVarDecl(bool requireSemicolon) {
  bool isConst = false;
  Token typeToken = peek();

  if (typeToken.type == TokenType::TYPE_CONST) {
    isConst = true;
    advance();
    if (isType(peek().type)) {
      advance();
      typeToken.value = "auto"; // "auto" if should infer type
    }
  } else {
    advance();
  }

  Token name = consume(TokenType::IDENTIFIER, "Expected variable name");

  unique_ptr<Expr> initializer = nullptr;
  if (match({TokenType::OP_ASSIGN})) {
    initializer = parseExpression();
  }

  if (requireSemicolon)
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");

  return std::make_unique<VarDeclStmt>(typeToken.value, name.value,
                                       std::move(initializer), isConst,
                                       typeToken.location);
}

unique_ptr<Stmt> Parser::parseExprStmt() {
  Token start = peek();
  auto expr = parseExpression();
  consume(TokenType::SEMICOLON, "Expected ';' after expression");
  return std::make_unique<ExprStmt>(std::move(expr), start.location);
}

unique_ptr<Stmt> Parser::parseBlock() {
  Token lbrace = consume(TokenType::LBRACE, "Expected '{'");
  vector<unique_ptr<Stmt>> statements;

  while (!check(TokenType::RBRACE) && !isAtEnd()) {
    auto stmt = parseStatement();
    if (stmt) {
      statements.push_back(std::move(stmt));
    }
  }

  consume(TokenType::RBRACE, "Expected '}' after block");
  return std::make_unique<BlockStmt>(std::move(statements), lbrace.location);
}

unique_ptr<Stmt> Parser::parseIfStmt() {
  Token ifToken = consume(TokenType::KW_IF, "Expected 'if'");
  consume(TokenType::LPAREN, "Expected '(' after 'if'");
  auto condition = parseExpression();
  consume(TokenType::RPAREN, "Expected ')' after condition");

  auto thenBranch = parseStatement();
  unique_ptr<Stmt> elseBranch = nullptr;

  if (match({TokenType::KW_ELSE})) {
    elseBranch = parseStatement();
  }

  return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch),
                                  std::move(elseBranch), ifToken.location);
}

unique_ptr<Stmt> Parser::parseWhileStmt() {
  Token whileToken = consume(TokenType::KW_WHILE, "Expected 'while'");
  consume(TokenType::LPAREN, "Expected '(' after 'while'");
  auto condition = parseExpression();
  consume(TokenType::RPAREN, "Expected ')' after condition");

  auto body = parseStatement();

  return std::make_unique<WhileStmt>(std::move(condition), std::move(body),
                                     whileToken.location);
}

unique_ptr<Stmt> Parser::parseForStmt() {
  Token forToken = consume(TokenType::KW_FOR, "Expected 'for'");
  consume(TokenType::LPAREN, "Expected '(' after 'for'");

  // Initializer
  unique_ptr<Stmt> initializer = nullptr;
  if (isType(peek().type)) {
    initializer = parseVarDecl(false);
  } else {
    auto expr = parseExpression();
    initializer =
        std::make_unique<ExprStmt>(std::move(expr), forToken.location);
  }

  // For each style for loop
  if (match({TokenType::COLON})) {
    auto collection = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after collection");
    auto body = parseStatement();
    return std::make_unique<ForEachStmt>(std::move(initializer),
                                         std::move(collection), std::move(body),
                                         forToken.location);
  }

  consume(TokenType::SEMICOLON,
          "Expected ';' or ':' after for loop initializer");

  // Condition
  unique_ptr<Expr> condition = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    condition = parseExpression();
  }
  consume(TokenType::SEMICOLON, "Expected ';' after for loop condition");

  // Increment
  unique_ptr<Expr> increment = nullptr;
  if (!check(TokenType::RPAREN)) {
    increment = parseExpression();
  }
  consume(TokenType::RPAREN, "Expected ')' after for clauses");

  auto body = parseStatement();

  return std::make_unique<ForStmt>(std::move(initializer), std::move(condition),
                                   std::move(increment), std::move(body),
                                   forToken.location);
}

unique_ptr<Stmt> Parser::parseSwitchStmt() {
  Token switchToken = consume(TokenType::KW_SWITCH, "Expected 'switch'");
  consume(TokenType::LPAREN, "Expected '(' after 'switch'");
  auto expression = parseExpression();
  consume(TokenType::RPAREN, "Expected ')' after switch expression");
  consume(TokenType::LBRACE, "Expected '{' after switch expression");

  vector<SwitchStmt::CaseClause> cases;

  while (!check(TokenType::RBRACE) && !isAtEnd()) {
    if (match({TokenType::KW_CASE})) {

      auto caseValue = parseExpression();
      auto caseStmt = parseStatement();
      cases.push_back({std::move(caseValue), std::move(caseStmt)});

    } else if (match({TokenType::KW_DEFAULT})) {

      auto defaultStmt = parseStatement();
      cases.push_back({nullptr, std::move(defaultStmt)});

    } else {
      error(peek(), "Expected 'case' or 'default' in switch statement");
      break;
    }
  }

  consume(TokenType::RBRACE, "Expected '}' after switch cases");

  return std::make_unique<SwitchStmt>(std::move(expression), std::move(cases),
                                      switchToken.location);
}

unique_ptr<Stmt> Parser::parseReturnStmt() {
  Token returnToken = consume(TokenType::KW_RETURN, "Expected 'return'");

  unique_ptr<Expr> value = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    value = parseExpression();
  }

  consume(TokenType::SEMICOLON, "Expected ';' after return statement");
  return std::make_unique<ReturnStmt>(std::move(value), returnToken.location);
}

unique_ptr<Stmt> Parser::parseBreakStmt() {
  Token breakToken = consume(TokenType::KW_BREAK, "Expected 'break'");
  consume(TokenType::SEMICOLON, "Expected ';' after 'break'");
  return std::make_unique<BreakStmt>(breakToken.location);
}

unique_ptr<Stmt> Parser::parseContinueStmt() {
  Token continueToken = consume(TokenType::KW_CONTINUE, "Expected 'continue'");
  consume(TokenType::SEMICOLON, "Expected ';' after 'continue'");
  return std::make_unique<ContinueStmt>(continueToken.location);
}

unique_ptr<Stmt> Parser::parseImportStmt() {
  Token importToken = consume(TokenType::KW_IMPORT, "Expected 'import'");

  // Local import (import "dir/file.lang")
  if (check(TokenType::LIT_STRING)) {
    string name = advance().value;
    consume(TokenType::SEMICOLON, "Expected ';' after 'import'");
    return std::make_unique<ImportStmt>(name, true, importToken.location);
  }

  // Global import (import math)
  string name = consume(TokenType::IDENTIFIER,
                        "Expected identifier or string after 'import'")
                    .value;
  consume(TokenType::SEMICOLON, "Expected ';' after 'import'");

  return std::make_unique<ImportStmt>(name, false, importToken.location);
}

// ============================================================================
// EXPRESSION PARSING (with Operator Precedence)
// ============================================================================

unique_ptr<Expr> Parser::parseExpression() { return parseAssignment(); }

unique_ptr<Expr> Parser::parseAssignment() {
  auto expr = parseTernary();

  if (match({TokenType::OP_ASSIGN, TokenType::OP_PLUS_ASSIGN,
             TokenType::OP_MINUS_ASSIGN, TokenType::OP_MULT_ASSIGN,
             TokenType::OP_DIV_ASSIGN, TokenType::OP_MOD_ASSIGN})) {
    Token equals = previous();
    auto value = parseAssignment();

    string name;
    unique_ptr<Expr> indexCopy = nullptr;
    // Check if expr is a variable
    if (auto *varExpr = dynamic_cast<VariableExpr *>(expr.get())) {
      name = varExpr->name;
      expr.release(); // Release ownership before creating new node
    }
    // Check if expr is an index expression (arr[i] = val)
    else if (auto *indexExpr = dynamic_cast<IndexExpr *>(expr.get())) {
      if (auto *arrVar = dynamic_cast<VariableExpr *>(indexExpr->array.get())) {
        name = arrVar->name;
        indexCopy = std::move(indexExpr->index);
        expr.release();
      }
    } else {
      error(equals, "Invalid assignment target");
    }

    if (equals.type == TokenType::OP_ASSIGN) {
      return std::make_unique<AssignExpr>(
          name, std::move(value), equals.location, std::move(indexCopy));
    } else { // += -= *= /= %=
      return std::make_unique<AssignOpExpr>(name, equals, std::move(value),
                                            equals.location,
                                            std::move(indexCopy));
    }
  }

  return expr;
}

unique_ptr<Expr> Parser::parseTernary() {
  auto expr = parseLogicalOr();

  if (match({TokenType::QUESTION})) {
    Token questionToken = previous();
    auto trueExpr = parseExpression();
    consume(TokenType::COLON, "Expected ':' after true expression in ternary");
    auto falseExpr = parseTernary(); // Right-associative

    expr = std::make_unique<TernaryExpr>(std::move(expr), std::move(trueExpr),
                                         std::move(falseExpr),
                                         questionToken.location);
  }

  return expr;
}

unique_ptr<Expr> Parser::parseLogicalOr() {
  auto expr = parseLogicalAnd();

  while (match({TokenType::OP_OR})) {
    Token op = previous();
    auto right = parseLogicalAnd();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, op.value,
                                        std::move(right), op.location);
  }

  return expr;
}

unique_ptr<Expr> Parser::parseLogicalAnd() {
  auto expr = parseBitwiseOr();

  while (match({TokenType::OP_AND})) {
    Token op = previous();
    auto right = parseBitwiseOr();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, op.value,
                                        std::move(right), op.location);
  }

  return expr;
}

unique_ptr<Expr> Parser::parseBitwiseOr() {
  auto expr = parseBitwiseXor();

  while (match({TokenType::OP_BIT_OR})) {
    Token op = previous();
    auto right = parseBitwiseXor();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, op.value,
                                        std::move(right), op.location);
  }

  return expr;
}

unique_ptr<Expr> Parser::parseBitwiseXor() {
  auto expr = parseBitwiseAnd();

  while (match({TokenType::OP_BIT_XOR})) {
    Token op = previous();
    auto right = parseBitwiseAnd();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, op.value,
                                        std::move(right), op.location);
  }

  return expr;
}

unique_ptr<Expr> Parser::parseBitwiseAnd() {
  auto expr = parseEquality();

  while (match({TokenType::OP_BIT_AND})) {
    Token op = previous();
    auto right = parseEquality();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, op.value,
                                        std::move(right), op.location);
  }

  return expr;
}

unique_ptr<Expr> Parser::parseEquality() {
  auto expr = parseComparison();

  while (match({TokenType::OP_EQUAL, TokenType::OP_NOT_EQUAL})) {
    Token op = previous();
    auto right = parseComparison();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, op.value,
                                        std::move(right), op.location);
  }

  return expr;
}

unique_ptr<Expr> Parser::parseComparison() {
  auto expr = parseShift();

  while (match({TokenType::OP_LESS, TokenType::OP_GREATER,
                TokenType::OP_LESS_EQUAL, TokenType::OP_GREATER_EQUAL})) {
    Token op = previous();
    auto right = parseShift();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, op.value,
                                        std::move(right), op.location);
  }

  return expr;
}

unique_ptr<Expr> Parser::parseShift() {
  auto expr = parseAddition();

  while (match({TokenType::OP_SHIFT_LEFT, TokenType::OP_SHIFT_RIGHT})) {
    Token op = previous();
    auto right = parseAddition();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, op.value,
                                        std::move(right), op.location);
  }

  return expr;
}

unique_ptr<Expr> Parser::parseAddition() {
  auto expr = parseMultiplication();

  while (match({TokenType::OP_PLUS, TokenType::OP_MINUS})) {
    Token op = previous();
    auto right = parseMultiplication();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, op.value,
                                        std::move(right), op.location);
  }

  return expr;
}

unique_ptr<Expr> Parser::parseMultiplication() {
  auto expr = parseUnary();

  while (match(
      {TokenType::OP_MULTIPLY, TokenType::OP_DIVIDE, TokenType::OP_MODULO})) {
    Token op = previous();
    auto right = parseUnary();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, op.value,
                                        std::move(right), op.location);
  }

  return expr;
}

unique_ptr<Expr> Parser::parseUnary() {
  if (match({TokenType::OP_NOT, TokenType::OP_MINUS, TokenType::OP_BIT_NOT,
             TokenType::OP_INCREMENT, TokenType::OP_DECREMENT})) {
    Token op = previous();
    auto right = parseUnary();
    return std::make_unique<UnaryExpr>(op.type, op.value, std::move(right),
                                       true, op.location);
  }

  return parsePostfix();
}

unique_ptr<Expr> Parser::parsePostfix() {
  auto expr = parsePrimary();

  while (true) {
    if (match({TokenType::OP_INCREMENT, TokenType::OP_DECREMENT})) {
      Token op = previous();
      expr = std::make_unique<UnaryExpr>(op.type, op.value, std::move(expr),
                                         false, op.location);
    } else if (match({TokenType::LBRACKET})) {
      auto index = parseExpression();
      consume(TokenType::RBRACKET, "Expected ']' after index");
      expr = std::make_unique<IndexExpr>(std::move(expr), std::move(index),
                                         previous().location);
    } else if (match({TokenType::LPAREN})) {
      // Function call
      if (auto *varExpr = dynamic_cast<VariableExpr *>(expr.get())) {
        string funcName = varExpr->name;
        Location loc = varExpr->location;

        vector<unique_ptr<Expr>> args;
        if (!check(TokenType::RPAREN)) {
          do {
            args.push_back(parseExpression());
          } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RPAREN, "Expected ')' after arguments");

        expr.release();
        expr = std::make_unique<CallExpr>(funcName, std::move(args), loc);
      } else {
        error(peek(), ("Expression '" + peek().value) +
                          ("' of type '" + TokenTypeToString(peek().type) +
                           "' is not callable"));
        return nullptr;
      }
    } else {
      break;
    }
  }

  return expr;
}

unique_ptr<Expr> Parser::parsePrimary() {
  if (match({TokenType::LIT_TRUE, TokenType::LIT_FALSE, TokenType::LIT_NULL,
             TokenType::LIT_INTEGER, TokenType::LIT_FLOAT,
             TokenType::LIT_STRING, TokenType::LIT_CHAR})) {
    return std::make_unique<LiteralExpr>(previous());
  }

  if (match({TokenType::IDENTIFIER})) {
    return std::make_unique<VariableExpr>(previous().value,
                                          previous().location);
  }

  if (match({TokenType::LPAREN})) {
    auto expr = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after expression");
    return expr;
  }

  if (match({TokenType::LBRACKET})) {
    // Could be array literal or range literal
    // Look ahead to distinguish: [1, 2, 3] vs [0:10] vs [0:10:2]
    Token lbracket = previous();

    // Parse first expression
    auto firstExpr = parseExpression();

    // Check if it's a range literal (contains :)
    if (match({TokenType::COLON})) {
      // Range literal: [start:stop] or [start:stop:step]
      auto stopExpr = parseExpression();
      unique_ptr<Expr> stepExpr = nullptr;

      if (match({TokenType::COLON})) {
        stepExpr = parseExpression();
      }

      consume(TokenType::RBRACKET, "Expected ']' after range literal");
      return std::make_unique<RangeLiteralExpr>(
          std::move(firstExpr), std::move(stopExpr), std::move(stepExpr),
          lbracket.location);
    } else {
      // Array literal: [1, 2, 3] - not yet implemented
      error(peek(), "Array literals not yet implemented");
      return nullptr;
    }
  }

  if (match({TokenType::SEMICOLON})) {
    return std::make_unique<EmptyExpr>(previous().location);
  }

  error(peek(), "Expected expression");
  return nullptr;
}

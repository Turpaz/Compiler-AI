#ifndef AST_H
#define AST_H

#include "../Lexer/Token.h"
#include <memory>
#include <string>
#include <vector>

using std::string;
using std::unique_ptr;
using std::vector;

class Expr;
class Stmt;

class ASTNode {
public:
  virtual ~ASTNode() = default;
  virtual void print(int indent = 0) const = 0;
};

class Expr : public ASTNode {
public:
  Location location;
  Expr(const Location &loc) : location(loc) {}
};

// Empty expression (;)
class EmptyExpr : public Expr {
public:
  EmptyExpr(const Location &loc) : Expr(loc) {}
  void print(int indent = 0) const override;
};

// Literal expressions (42, 3.14, "hello", true, etc.)
class LiteralExpr : public Expr {
public:
  Token token;

  LiteralExpr(const Token &tok) : Expr(tok.location), token(tok) {}
  void print(int indent = 0) const override;
};

// Variable references (x, myVar, etc.)
class VariableExpr : public Expr {
public:
  string name;

  VariableExpr(const string &n, const Location &loc) : Expr(loc), name(n) {}
  void print(int indent = 0) const override;
};

// Binary operations (a + b, x == y, etc.)
class BinaryExpr : public Expr {
public:
  unique_ptr<Expr> left;
  TokenType op;
  string opStr;
  unique_ptr<Expr> right;

  BinaryExpr(unique_ptr<Expr> l, TokenType o, const string &opS,
             unique_ptr<Expr> r, const Location &loc)
      : Expr(loc), left(std::move(l)), op(o), opStr(opS), right(std::move(r)) {}
  void print(int indent = 0) const override;
};

// Unary operations (-x, !flag, ++i, etc.)
class UnaryExpr : public Expr {
public:
  TokenType op;
  string opStr;
  unique_ptr<Expr> operand;
  bool isPrefix; // true for ++x, false for x++

  UnaryExpr(TokenType o, const string &opS, unique_ptr<Expr> operand,
            bool prefix, const Location &loc)
      : Expr(loc), op(o), opStr(opS), operand(std::move(operand)),
        isPrefix(prefix) {}
  void print(int indent = 0) const override;
};

// Ternary operator (condition ? trueExpr : falseExpr)
class TernaryExpr : public Expr {
public:
  unique_ptr<Expr> condition;
  unique_ptr<Expr> trueExpr;
  unique_ptr<Expr> falseExpr;

  TernaryExpr(unique_ptr<Expr> cond, unique_ptr<Expr> trueE,
              unique_ptr<Expr> falseE, const Location &loc)
      : Expr(loc), condition(std::move(cond)), trueExpr(std::move(trueE)),
        falseExpr(std::move(falseE)) {}
  void print(int indent = 0) const override;
};

// Function calls (foo(), bar(1, 2, 3))
class CallExpr : public Expr {
public:
  string functionName;
  vector<unique_ptr<Expr>> arguments;

  CallExpr(const string &name, vector<unique_ptr<Expr>> args,
           const Location &loc)
      : Expr(loc), functionName(name), arguments(std::move(args)) {}
  void print(int indent = 0) const override;
};

// Array/list indexing (arr[0], list[i])
class IndexExpr : public Expr {
public:
  unique_ptr<Expr> array;
  unique_ptr<Expr> index;

  IndexExpr(unique_ptr<Expr> arr, unique_ptr<Expr> idx, const Location &loc)
      : Expr(loc), array(std::move(arr)), index(std::move(idx)) {}
  void print(int indent = 0) const override;
};

// Range literal ([start:stop] or [start:stop:step])
class RangeLiteralExpr : public Expr {
public:
  unique_ptr<Expr> start;
  unique_ptr<Expr> stop;
  unique_ptr<Expr> step; // can be nullptr

  RangeLiteralExpr(unique_ptr<Expr> start, unique_ptr<Expr> stop,
                   unique_ptr<Expr> step, const Location &loc)
      : Expr(loc), start(std::move(start)), stop(std::move(stop)),
        step(std::move(step)) {}
  void print(int indent = 0) const override;
};

// Assignment (x = 5, arr[i] = 10)
class AssignExpr : public Expr {
public:
  string varName;
  unique_ptr<Expr> value;
  unique_ptr<Expr>
      indexExpr; // nullptr if simple variable, otherwise for arr[i] = val

  AssignExpr(const string &name, unique_ptr<Expr> val, const Location &loc,
             unique_ptr<Expr> idx = nullptr)
      : Expr(loc), varName(name), value(std::move(val)),
        indexExpr(std::move(idx)) {}
  void print(int indent = 0) const override;
};

class AssignOpExpr : public Expr {
public:
  string varName;
  TokenType op;
  string opStr;
  unique_ptr<Expr> value;
  unique_ptr<Expr>
      indexExpr; // nullptr if simple variable, otherwise for arr[i] = val

  AssignOpExpr(const string &name, Token o, unique_ptr<Expr> val,
               const Location &loc, unique_ptr<Expr> idx = nullptr)
      : Expr(loc), varName(name), op(o.type), opStr(o.value),
        value(std::move(val)), indexExpr(std::move(idx)) {}
  void print(int indent = 0) const override;
};

// ============================================================================
// STATEMENT NODES
// ============================================================================

class Stmt : public ASTNode {
public:
  Location location;
  Stmt(const Location &loc) : location(loc) {}
};

// Empty statement (just a semicolon)
class EmptyStmt : public Stmt {
public:
  EmptyStmt(const Location &loc) : Stmt(loc) {}
  void print(int indent = 0) const override;
};

// Expression as statement (function();)
class ExprStmt : public Stmt {
public:
  unique_ptr<Expr> expression;

  ExprStmt(unique_ptr<Expr> expr, const Location &loc)
      : Stmt(loc), expression(std::move(expr)) {}
  void print(int indent = 0) const override;
};

// Variable declaration (int x = 5;)
class VarDeclStmt : public Stmt {
public:
  string type; // can be just const if it should be inferred
  string name;
  unique_ptr<Expr> initializer; // can be nullptr
  bool isConst;

  VarDeclStmt(const string &t, const string &n, unique_ptr<Expr> init, bool isC,
              const Location &loc)
      : Stmt(loc), type(t), name(n), initializer(std::move(init)),
        isConst(isC) {}
  void print(int indent = 0) const override;
};

// Block of statements ({ ... })
class BlockStmt : public Stmt {
public:
  vector<unique_ptr<Stmt>> statements;

  BlockStmt(vector<unique_ptr<Stmt>> stmts, const Location &loc)
      : Stmt(loc), statements(std::move(stmts)) {}
  void print(int indent = 0) const override;
};

// If statement
class IfStmt : public Stmt {
public:
  unique_ptr<Expr> condition;
  unique_ptr<Stmt> thenBranch;
  unique_ptr<Stmt> elseBranch; // can be nullptr

  IfStmt(unique_ptr<Expr> cond, unique_ptr<Stmt> thenB, unique_ptr<Stmt> elseB,
         const Location &loc)
      : Stmt(loc), condition(std::move(cond)), thenBranch(std::move(thenB)),
        elseBranch(std::move(elseB)) {}
  void print(int indent = 0) const override;
};

// While loop
class WhileStmt : public Stmt {
public:
  unique_ptr<Expr> condition;
  unique_ptr<Stmt> body;

  WhileStmt(unique_ptr<Expr> cond, unique_ptr<Stmt> b, const Location &loc)
      : Stmt(loc), condition(std::move(cond)), body(std::move(b)) {}
  void print(int indent = 0) const override;
};

// For loop (for (int i = 0; i < 10; i++) { ... })
class ForStmt : public Stmt {
public:
  unique_ptr<Stmt> initializer; // can be nullptr
  unique_ptr<Expr> condition;   // can be nullptr
  unique_ptr<Expr> increment;   // can be nullptr
  unique_ptr<Stmt> body;

  ForStmt(unique_ptr<Stmt> init, unique_ptr<Expr> cond, unique_ptr<Expr> inc,
          unique_ptr<Stmt> b, const Location &loc)
      : Stmt(loc), initializer(std::move(init)), condition(std::move(cond)),
        increment(std::move(inc)), body(std::move(b)) {}
  void print(int indent = 0) const override;
};

// For-each loop (for (int i : arr) { ... })
class ForEachStmt : public Stmt {
public:
  unique_ptr<Stmt> initializer; // can be nullptr
  unique_ptr<Expr> collection;  // can be nullptr
  unique_ptr<Stmt> body;

  ForEachStmt(unique_ptr<Stmt> init, unique_ptr<Expr> coll, unique_ptr<Stmt> b,
              const Location &loc)
      : Stmt(loc), initializer(std::move(init)), collection(std::move(coll)),
        body(std::move(b)) {}
  void print(int indent = 0) const override;
};

// Switch statement
class SwitchStmt : public Stmt {
public:
  struct CaseClause {
    unique_ptr<Expr> value; // nullptr for default case
    unique_ptr<Stmt> statement;
  };

  unique_ptr<Expr> expression;
  vector<CaseClause> cases;

  SwitchStmt(unique_ptr<Expr> expr, vector<CaseClause> c, const Location &loc)
      : Stmt(loc), expression(std::move(expr)), cases(std::move(c)) {}
  void print(int indent = 0) const override;
};

// Return statement
class ReturnStmt : public Stmt {
public:
  unique_ptr<Expr> value; // can be nullptr for void returns

  ReturnStmt(unique_ptr<Expr> val, const Location &loc)
      : Stmt(loc), value(std::move(val)) {}
  void print(int indent = 0) const override;
};

// Break statement
class BreakStmt : public Stmt {
public:
  BreakStmt(const Location &loc) : Stmt(loc) {}
  void print(int indent = 0) const override;
};

// Continue statement
class ContinueStmt : public Stmt {
public:
  ContinueStmt(const Location &loc) : Stmt(loc) {}
  void print(int indent = 0) const override;
};

// Import statement
class ImportStmt : public Stmt {
public:
  string name;
  bool isLocal;

  ImportStmt(const string &n, bool is, const Location &loc)
      : Stmt(loc), name(n), isLocal(is) {}
  void print(int indent = 0) const override;
};

// ============================================================================
// DECLARATION NODES
// ============================================================================

// Function declaration
class FunctionDecl : public ASTNode {
public:
  struct Parameter {
    string type;
    string name;
    unique_ptr<Expr> defaultValue; // nullptr if no default
  };

  string returnType;
  string name;
  vector<Parameter> parameters;
  unique_ptr<BlockStmt> body;
  bool isArrowFunction;
  unique_ptr<Expr> arrowBody; // for arrow functions
  Location location;

  FunctionDecl(const string &retType, const string &n, vector<Parameter> params,
               unique_ptr<BlockStmt> b, const Location &loc,
               bool isArrow = false, unique_ptr<Expr> arrowB = nullptr)
      : returnType(retType), name(n), parameters(std::move(params)),
        body(std::move(b)), isArrowFunction(isArrow),
        arrowBody(std::move(arrowB)), location(loc) {}
  void print(int indent = 0) const override;
};

// ============================================================================
// PROGRAM (Top-level)
// ============================================================================

class Program : public ASTNode {
public:
  vector<unique_ptr<FunctionDecl>> functions;
  vector<unique_ptr<Stmt>> statements;

  void print(int indent = 0) const override;
};

#endif // AST_H

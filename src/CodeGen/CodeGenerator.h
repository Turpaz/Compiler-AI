#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "../Parser/AST.h"
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::stringstream;
using std::unique_ptr;
using std::vector;

class CodeGenerator {
public:
  CodeGenerator();

  // Main entry point
  string generate(const Program *program);

private:
  stringstream output;
  int regCount = 0;
  int labelCount = 0;

  // Symbol table: maps variable name to register name (e.g., "%x") or pointer
  // ("%ptr")
  struct SymbolInfo {
    string irName; // The LLVM IR name (e.g., %1 or %x)
    string type;   // The LLVM IR type (e.g., i32)
    bool isPtr;    // True if this is an alloca'd variable (needs load)
  };

  // Simple scope handling
  vector<map<string, SymbolInfo>> scopes;

  // Generation methods
  void genPreamble();
  void genGlobals();
  void genFunction(const FunctionDecl *func);
  void genStatement(const Stmt *stmt);

  // String literal handling
  map<string, string> stringLiterals; // content -> global name
  void scanStrings(const ASTNode *node);

  bool isTerminator(const Stmt *stmt);

  // Function index for default parameters
  map<string, const FunctionDecl *> functions;

  // Loop labels for break/continue
  struct LoopLabels {
    string breakLabel;
    string continueLabel;
  };
  vector<LoopLabels> loopStack;

  struct Register {
    string name;
    string type;
    Register(string name, string type) : name(name), type(type) {}
  };

  // Expression generation returns the register name containing the result
  Register genExpression(const Expr *expr);

  // ================== Defined in CodegenGenMethods.cpp ==================
  // Statement generation
  void genVarDeclStmt(const VarDeclStmt *var);
  void genReturnStmt(const ReturnStmt *ret);
  void genIfStmt(const IfStmt *ifStmt);
  void genWhileStmt(const WhileStmt *whileStmt);
  void genForStmt(const ForStmt *forStmt);
  // void genForEachStmt(const ForEachStmt *forStmt); // TODO: implement
  void genBreakStmt();
  void genContinueStmt();
  // Expression generation
  Register genLiteralExpr(const LiteralExpr *expr);
  Register genVariableExpr(const VariableExpr *expr);
  Register genBinaryExpr(const BinaryExpr *expr);

  Register genAndOr(const Expr *l, const Expr *r, const bool opIsAnd);
  Register genToBool(const Register &reg);
  Register genToInteger(const Register &reg, string i = "i32");
  Register genToFP(const Register &reg, string f = "float");

  // ================== Defined in CodegenHelperMethods.cpp ==================
  // Helper methods
  void enterScope();
  void exitScope();
  void declareVariable(const string &name, const string &irName,
                       const string &type, bool isPtr);
  SymbolInfo *lookupVariable(const string &name);

  Register newReg(string type = "");
  string newLabel(const string &prefix = "lbl");

  // Type mapping
  string mapType(const string &typeName);
  string getDefaultValue(const string &typeName);

  string escapeString(const string &input);

  template <typename T> inline const T *typeCheck(const ASTNode *node) {
    return dynamic_cast<const T *>(node);
  }

  bool isIntegerType(string type);
  bool isFloatingPointType(string type);
};

#endif // CODE_GENERATOR_H

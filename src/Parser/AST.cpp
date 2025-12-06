#include "AST.h"
#include <iostream>

using std::cout;
using std::endl;

static string indent_str(int indent) { return string(indent * 2, ' '); }

// ============================================================================
// EXPRESSION PRINT IMPLEMENTATIONS
// ============================================================================

void EmptyExpr::print(int indent) const {
  cout << indent_str(indent) << "EmptyExpr" << endl;
}

void LiteralExpr::print(int indent) const {
  cout << indent_str(indent) << "Literal: " << token.value << " ("
       << TokenTypeToString(token.type) << ")" << endl;
}

void VariableExpr::print(int indent) const {
  cout << indent_str(indent) << "Variable: " << name << endl;
}

void BinaryExpr::print(int indent) const {
  cout << indent_str(indent) << "BinaryExpr: " << opStr << endl;
  left->print(indent + 1);
  right->print(indent + 1);
}

void UnaryExpr::print(int indent) const {
  cout << indent_str(indent) << "UnaryExpr: " << opStr
       << (isPrefix ? " (prefix)" : " (postfix)") << endl;
  operand->print(indent + 1);
}

void TernaryExpr::print(int indent) const {
  cout << indent_str(indent) << "TernaryExpr: ? :" << endl;
  cout << indent_str(indent + 1) << "Condition:" << endl;
  condition->print(indent + 2);
  cout << indent_str(indent + 1) << "True:" << endl;
  trueExpr->print(indent + 2);
  cout << indent_str(indent + 1) << "False:" << endl;
  falseExpr->print(indent + 2);
}

void CallExpr::print(int indent) const {
  cout << indent_str(indent) << "Call: " << functionName << "("
       << arguments.size() << " args)" << endl;
  for (const auto &arg : arguments) {
    arg->print(indent + 1);
  }
}

void IndexExpr::print(int indent) const {
  cout << indent_str(indent) << "Index:" << endl;
  array->print(indent + 1);
  cout << indent_str(indent + 1) << "[" << endl;
  index->print(indent + 2);
  cout << indent_str(indent + 1) << "]" << endl;
}

void RangeLiteralExpr::print(int indent) const {
  cout << indent_str(indent) << "RangeLiteral: [";
  if (step) {
    cout << "start:stop:step]" << endl;
  } else {
    cout << "start:stop]" << endl;
  }
  cout << indent_str(indent + 1) << "Start:" << endl;
  start->print(indent + 2);
  cout << indent_str(indent + 1) << "Stop:" << endl;
  stop->print(indent + 2);
  if (step) {
    cout << indent_str(indent + 1) << "Step:" << endl;
    step->print(indent + 2);
  }
}

void AssignExpr::print(int indent) const {
  cout << indent_str(indent) << "Assign: " << varName;
  if (indexExpr) {
    cout << "[...]";
  }
  cout << " =" << endl;
  if (indexExpr) {
    indexExpr->print(indent + 1);
  }
  value->print(indent + 1);
}

void AssignOpExpr::print(int indent) const {
  cout << indent_str(indent) << "AssignOp: " << varName;
  if (indexExpr) {
    cout << "[...];";
  }
  cout << " " << opStr << endl;
  if (indexExpr) {
    indexExpr->print(indent + 1);
  }
  value->print(indent + 1);
}

// ============================================================================
// STATEMENT PRINT IMPLEMENTATIONS
// ============================================================================

void EmptyStmt::print(int indent) const {
  cout << indent_str(indent) << "EmptyStmt" << endl;
}

void ExprStmt::print(int indent) const {
  cout << indent_str(indent) << "ExprStmt:" << endl;
  expression->print(indent + 1);
}

void VarDeclStmt::print(int indent) const {
  cout << indent_str(indent) << "VarDecl: " << (isConst ? "const " : "") << type
       << " " << name;
  if (initializer) {
    cout << " =" << endl;
    initializer->print(indent + 1);
  } else {
    cout << endl;
  }
}

void BlockStmt::print(int indent) const {
  cout << indent_str(indent) << "Block: {" << endl;
  for (const auto &stmt : statements) {
    stmt->print(indent + 1);
  }
  cout << indent_str(indent) << "}" << endl;
}

void IfStmt::print(int indent) const {
  cout << indent_str(indent) << "If:" << endl;
  cout << indent_str(indent + 1) << "Condition:" << endl;
  condition->print(indent + 2);
  cout << indent_str(indent + 1) << "Then:" << endl;
  thenBranch->print(indent + 2);
  if (elseBranch) {
    cout << indent_str(indent + 1) << "Else:" << endl;
    elseBranch->print(indent + 2);
  }
}

void WhileStmt::print(int indent) const {
  cout << indent_str(indent) << "While:" << endl;
  cout << indent_str(indent + 1) << "Condition:" << endl;
  condition->print(indent + 2);
  cout << indent_str(indent + 1) << "Body:" << endl;
  body->print(indent + 2);
}

void ForStmt::print(int indent) const {
  cout << indent_str(indent) << "For:" << endl;
  if (initializer) {
    cout << indent_str(indent + 1) << "Init:" << endl;
    initializer->print(indent + 2);
  }
  if (condition) {
    cout << indent_str(indent + 1) << "Condition:" << endl;
    condition->print(indent + 2);
  }
  if (increment) {
    cout << indent_str(indent + 1) << "Increment:" << endl;
    increment->print(indent + 2);
  }
  cout << indent_str(indent + 1) << "Body:" << endl;
  body->print(indent + 2);
}

void ForEachStmt::print(int indent) const {
  cout << indent_str(indent) << "For Each:" << endl;
  cout << indent_str(indent + 1) << "Init:" << endl;
  initializer->print(indent + 2);
  cout << indent_str(indent + 1) << "Collection:" << endl;
  collection->print(indent + 2);
  cout << indent_str(indent + 1) << "Body:" << endl;
  body->print(indent + 2);
}

void SwitchStmt::print(int indent) const {
  cout << indent_str(indent) << "Switch:" << endl;
  cout << indent_str(indent + 1) << "Expression:" << endl;
  expression->print(indent + 2);
  cout << indent_str(indent + 1) << "Cases:" << endl;
  for (const auto &caseClause : cases) {
    if (caseClause.value) {
      cout << indent_str(indent + 2) << "Case:" << endl;
      caseClause.value->print(indent + 3);
    } else {
      cout << indent_str(indent + 2) << "Default:" << endl;
    }
    caseClause.statement->print(indent + 3);
  }
}

void ReturnStmt::print(int indent) const {
  cout << indent_str(indent) << "Return:";
  if (value) {
    cout << endl;
    value->print(indent + 1);
  } else {
    cout << " (void)" << endl;
  }
}

void BreakStmt::print(int indent) const {
  cout << indent_str(indent) << "Break" << endl;
}

void ContinueStmt::print(int indent) const {
  cout << indent_str(indent) << "Continue" << endl;
}

void ImportStmt::print(int indent) const {
  if (isLocal)
    cout << indent_str(indent) << "Import: \"" << name << "\" (local)" << endl;
  else
    cout << indent_str(indent) << "Import: " << name << endl;
}

// ============================================================================
// DECLARATION PRINT IMPLEMENTATIONS
// ============================================================================

void FunctionDecl::print(int indent) const {
  cout << indent_str(indent) << "Function: " << returnType << " " << name
       << "(";
  for (size_t i = 0; i < parameters.size(); i++) {
    cout << parameters[i].type << " " << parameters[i].name;
    if (parameters[i].defaultValue) {
      cout << " = <default>";
    }
    if (i < parameters.size() - 1)
      cout << ", ";
  }
  cout << ")";

  if (isArrowFunction) {
    cout << " => <expr>" << endl;
    if (arrowBody) {
      arrowBody->print(indent + 1);
    }
  } else {
    cout << endl;
    body->print(indent + 1);
  }
}

// ============================================================================
// PROGRAM PRINT IMPLEMENTATION
// ============================================================================

void Program::print(int indent) const {
  cout << indent_str(indent) << "Program:" << endl;
  for (const auto &func : functions) {
    func->print(indent + 1);
  }
  for (const auto &stmt : statements) {
    stmt->print(indent + 1);
  }
}

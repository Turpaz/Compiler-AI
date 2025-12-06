#include "CodeGenerator.h"

void CodeGenerator::genVarDeclStmt(const VarDeclStmt *var) {
  string type = mapType(var->type);
  // If type is "auto" (inferred), we need to infer it from initializer
  // For now, assume int if auto (TODO: better inference)
  if (var->type == "auto" || var->type == "") {
    // Simple hack: try to guess from initializer if possible, or default to
    // i32
    type = "i32";
  }

  Register ptr = newReg(type);
  output << "  " << ptr.name << " = alloca " << ptr.type << "\n";

  if (var->initializer) {
    Register val = genExpression(var->initializer.get());
    output << "  store " << ptr.type << " " << val.name << ", " << ptr.type
           << "* " << ptr.name << "\n";
  }

  declareVariable(var->name, ptr.name, ptr.type, true);
}

void CodeGenerator::genReturnStmt(const ReturnStmt *ret) {
  if (ret->value) {
    Register val = genExpression(ret->value.get());
    // We need to know the return type here, but for simplicity assuming
    // correct In a real compiler we'd look up current function return type
    output << "  ret " << val.type << " " << val.name << "\n";
  } else {
    output << "  ret void\n";
  }
}

void CodeGenerator::genIfStmt(const IfStmt *ifStmt) {
  Register cond = genExpression(ifStmt->condition.get());
  if (cond.type != "i1")
    cond = genToBool(cond); // make sure condition is bool
  string thenLbl = newLabel("if.then");
  string elseLbl = newLabel("if.else");
  string mergeLbl = newLabel("if.merge");

  output << "  br i1 " << cond.name << ", label %" << thenLbl << ", label %"
         << elseLbl << "\n";

  output << thenLbl << ":\n";
  genStatement(ifStmt->thenBranch.get());
  if (!isTerminator(ifStmt->thenBranch.get()))
    output << "  br label %" << mergeLbl << "\n";

  output << elseLbl << ":\n";
  if (ifStmt->elseBranch) {
    genStatement(ifStmt->elseBranch.get());
    if (!isTerminator(ifStmt->elseBranch.get()))
      output << "  br label %" << mergeLbl << "\n";
  } else {
    output << "  br label %" << mergeLbl << "\n";
  }

  output << mergeLbl << ":\n";
}

void CodeGenerator::genWhileStmt(const WhileStmt *whileStmt) {
  string condLbl = newLabel("while.cond");
  string bodyLbl = newLabel("while.body");
  string endLbl = newLabel("while.end");

  // Push loop labels
  loopStack.push_back({endLbl, condLbl});

  output << "  br label %" << condLbl << "\n";
  output << condLbl << ":\n";

  Register cond = genExpression(whileStmt->condition.get());
  if (cond.type != "i1")
    cond = genToBool(cond); // make sure condition is bool

  output << "  br i1 " << cond.name << ", label %" << bodyLbl << ", label %"
         << endLbl << "\n";

  output << bodyLbl << ":\n";
  genStatement(whileStmt->body.get());
  if (!isTerminator(whileStmt->body.get()))
    output << "  br label %" << condLbl << "\n";

  output << endLbl << ":\n";

  // Pop loop labels
  loopStack.pop_back();
}

void CodeGenerator::genForStmt(const ForStmt *forStmt) {
  enterScope();

  string initLbl = newLabel("for.init");
  string condLbl = newLabel("for.cond");
  string bodyLbl = newLabel("for.body");
  string incLbl = newLabel("for.inc");
  string endLbl = newLabel("for.end");

  output << "  br label %" << initLbl << "\n";
  output << initLbl << ":\n";

  if (forStmt->initializer) {
    genStatement(forStmt->initializer.get());
  }

  // Push loop labels (continue goes to increment)
  loopStack.push_back({endLbl, incLbl});

  output << "  br label %" << condLbl << "\n";
  output << condLbl << ":\n";

  if (forStmt->condition) {
    Register cond = genExpression(forStmt->condition.get());
    if (cond.type != "i1")
      cond = genToBool(cond); // make sure condition is bool
    output << "  br i1 " << cond.name << ", label %" << bodyLbl << ", label %"
           << endLbl << "\n";
  } else {
    output << "  br label %" << bodyLbl << "\n";
  }

  output << bodyLbl << ":\n";
  genStatement(forStmt->body.get());
  if (!isTerminator(forStmt->body.get()))
    output << "  br label %" << incLbl << "\n";

  output << incLbl << ":\n";
  if (forStmt->increment) {
    genExpression(forStmt->increment.get());
  }
  output << "  br label %" << condLbl << "\n";

  output << endLbl << ":\n";

  loopStack.pop_back();
  exitScope();
}

void CodeGenerator::genBreakStmt() {
  if (loopStack.empty()) {
    // error("break statement outside of loop"); // TODO: error handling
    return;
  }
  output << "  br label %" << loopStack.back().breakLabel << "\n";
}

void CodeGenerator::genContinueStmt() {
  if (loopStack.empty()) {
    // error("continue statement outside of loop"); // TODO: error handling
    return;
  }
  output << "  br label %" << loopStack.back().continueLabel << "\n";
}

// =============================================================================
// ======================= Expression generation methods =======================
// =============================================================================

CodeGenerator::Register CodeGenerator::genLiteralExpr(const LiteralExpr *lit) {
  if (lit->token.type == TokenType::LIT_INTEGER)
    return Register(lit->token.value, "i32");

  if (lit->token.type == TokenType::LIT_FLOAT)
    return Register(lit->token.value, "float");
  if (lit->token.type == TokenType::LIT_TRUE)
    return Register("1", "i1");
  if (lit->token.type == TokenType::LIT_FALSE)
    return Register("0", "i1");
  if (lit->token.type == TokenType::LIT_STRING) {
    if (stringLiterals.count(lit->token.value)) {
      string globalName = stringLiterals[lit->token.value];
      Register reg = newReg("i8*");
      // getelementptr to decay array to pointer
      // getelementptr inbounds [N x i8], [N x i8]* @str, i64 0, i64 0
      output << "  " << reg.name << " = getelementptr inbounds ["
             << (lit->token.value.length() + 1) << " x i8], ["
             << (lit->token.value.length() + 1) << " x i8]* " << globalName
             << ", i64 0, i64 0\n";
      return reg;
    }
    return Register("null", "i8*");
  }
  // error("Invalid literal type"); // TODO: error handling
  return Register("0", "i32");
}

CodeGenerator::Register
CodeGenerator::genVariableExpr(const VariableExpr *expr) {
  SymbolInfo *info = lookupVariable(expr->name);
  if (info) {
    if (info->isPtr) {
      Register reg = newReg(info->type);
      output << "  " << reg.name << " = load " << info->type << ", "
             << info->type << "* " << info->irName << "\n";
      return reg;
    } else {
      return Register(info->irName, info->type);
    }
  }
  // error("Variable not found"); // TODO: error handling
  return Register("0", "i32");
}

CodeGenerator::Register CodeGenerator::genBinaryExpr(const BinaryExpr *bin) {

  if (bin->op == TokenType::OP_AND || bin->op == TokenType::OP_OR) {
    return genAndOr(bin->left.get(), bin->right.get(),
                    bin->op == TokenType::OP_AND);
  }

  Register left = genExpression(bin->left.get());
  Register right = genExpression(bin->right.get());
  Register reg = newReg(left.type);
  // isFloatingPointType(right.type) ? right.type : left.type
  // use left operand unless right is floating point TODO: type checking
  // (convert if needed)

  // Assume integer math for now (TODO: type checking)
  string op = "";
  if (bin->op == TokenType::OP_PLUS)
    op = "add";
  else if (bin->op == TokenType::OP_MINUS)
    op = "sub";
  else if (bin->op == TokenType::OP_MULTIPLY)
    op = "mul";
  else if (bin->op == TokenType::OP_DIVIDE)
    op = "sdiv";
  else if (bin->op == TokenType::OP_LESS)
    op = "icmp slt";
  else if (bin->op == TokenType::OP_GREATER)
    op = "icmp sgt";
  else if (bin->op == TokenType::OP_EQUAL)
    op = "icmp eq";
  else if (bin->op == TokenType::OP_NOT_EQUAL)
    op = "icmp ne";
  else if (bin->op == TokenType::OP_LESS_EQUAL)
    op = "icmp sle";
  else if (bin->op == TokenType::OP_GREATER_EQUAL)
    op = "icmp sge";
  else if (bin->op == TokenType::OP_MODULO)
    op = "srem";
  else if (bin->op == TokenType::OP_BIT_AND)
    op = "and";
  else if (bin->op == TokenType::OP_BIT_OR)
    op = "or";
  else if (bin->op == TokenType::OP_BIT_XOR)
    op = "xor";
  else if (bin->op == TokenType::OP_SHIFT_LEFT)
    op = "shl";
  else if (bin->op == TokenType::OP_SHIFT_RIGHT)
    op = "ashr"; // Arithmetic shift right
  // All bitwise operators implemented

  if (op.rfind("icmp", 0) == 0) {
    reg.type = "i1";
    output << "  " << reg.name << " = " << op << " " << left.type << " "
           << left.name << ", " << right.name << "\n";
  } else {
    reg.type = left.type;
    output << "  " << reg.name << " = " << op << " " << left.type << " "
           << left.name << ", " << right.name << "\n";
  }

  return reg;
}

CodeGenerator::Register CodeGenerator::genAndOr(const Expr *l, const Expr *r,
                                                const bool opIsAnd) {
  string startLbl = newLabel(opIsAnd ? "and.start" : "or.start");
  string rhsLbl = newLabel(opIsAnd ? "and.rhs" : "or.rhs");
  string endLbl = newLabel(opIsAnd ? "and.end" : "or.end");

  output << "  br label %" << startLbl << "\n";
  output << startLbl << ":\n";
  Register lBool = genToBool(genExpression(l)); // ensure bool

  output << "  br i1 " << lBool.name << ", label %"
         << (opIsAnd ? rhsLbl : endLbl) << ", label %"
         << (opIsAnd ? endLbl : rhsLbl) << "\n";

  output << rhsLbl << ":\n";
  Register rBool = genToBool(genExpression(r)); // ensure bool
  output << "  br label %" << endLbl << "\n";

  Register reg = newReg("i1");
  output << endLbl << ":\n";
  output << "  " << reg.name << " = phi i1 [ " << (opIsAnd ? "true" : "false")
         << ", %" << startLbl << " ], [ " << rBool.name << ", %" << rhsLbl
         << "]\n";

  return reg;
}

bool CodeGenerator::isIntegerType(string type) {
  return type == "i1" || type == "i8" || type == "i16" || type == "i32" ||
         type == "i64" || type == "i128";
}

bool CodeGenerator::isFloatingPointType(string type) {
  return type == "half" || type == "float" || type == "double" ||
         type == "fp128" || type == "ppc_fp128";
}

// int to bool
CodeGenerator::Register CodeGenerator::genToBool(const Register &reg) {
  if (reg.type == "i1")
    return reg;
  else if (isFloatingPointType(reg.type)) {
    Register res = newReg("i1");
    output << "  " << res.name << " = fcmp one " << reg.type << " " << reg.name
           << ", 0.0\n";
    return res;
  } else if (isIntegerType(reg.type)) {
    Register res = newReg("i1");
    output << "  " << res.name << " = icmp ne " << reg.type << " " << reg.name
           << ", 0\n";
    return res;
  }
  assert(false && "Invalid type for genToBool");
  return Register("0", "i1");
}

// int/fp to int (unsigned)
CodeGenerator::Register CodeGenerator::genToInteger(const Register &reg,
                                                    string i) {
  Register res = newReg(i);
  if (isIntegerType(reg.type) && isIntegerType(i)) {
    if (reg.type == i)
      return reg;
    else if (std::atoi(&reg.type[1]) > std::atoi(&i[1]))
      output << "  " << res.name << " = trunc " << reg.type << " " << reg.name
             << " to " << i << "\n";
    else if (std::atoi(&reg.type[1]) < std::atoi(&i[1]))
      output << "  " << res.name << " = zext " << reg.type << " " << reg.name
             << " to " << i << "\n";
    return res;
  }
  if (isFloatingPointType(reg.type)) {
    output << "  " << res.name << " = fptosi " << reg.type << " " << reg.name
           << " to " << i << "\n";
    return res;
  }
  assert(false && "Invalid type for genToInteger");
  return Register("0", "i32");
}

// int to float32 (signed)
CodeGenerator::Register CodeGenerator::genToFP(const Register &reg, string f) {
  if (!isIntegerType(reg.type)) {
    assert(false && "Invalid type for genToFP");
    return Register("0.0", "float");
  }
  Register res = newReg(f);
  output << "  " << res.name << " = sitofp " << reg.type << " " << reg.name
         << " to " << f << "\n";
  return res;
}

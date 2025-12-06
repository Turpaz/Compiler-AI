#include "CodeGenerator.h"
#include <cstdio>
#include <iostream>

CodeGenerator::CodeGenerator() {
  // Global scope
  enterScope();
}

string CodeGenerator::generate(const Program *program) {
  // Clear output
  output.str("");
  output.clear();

  // Generate header code
  genPreamble();

  // Recursively scan for string literals through the AST
  scanStrings(program);
  genGlobals();

  // Index functions
  for (const auto &func : program->functions) {
    functions[func->name] = func.get();
  }

  // Generate functions code separately
  for (const auto &func : program->functions) {
    genFunction(func.get());
  }

  // TODO: Replace with globals approach
  // Generate a main function if statements exist at top level
  if (!program->statements.empty() && !functions.count("main")) {
    output << "define i32 @main() {\n";
    output << "entry:\n";

    enterScope();
    for (const auto &stmt : program->statements) {
      genStatement(stmt.get());
    }
    exitScope();

    output << "  ret i32 0\n";
    output << "}\n";
  }

  return output.str();
}

void CodeGenerator::genPreamble() {
  output << "; ModuleID = 'compiler_output'\n"; //  TODO: generalize
  output << "source_filename = \"compiler_output\"\n\n";

  // Runtime declarations // TODO: only declare used functions
  output << "declare void @print_int(i32)\n";
  output << "declare void @print_float(double)\n";
  output << "declare void @print_string(i8*)\n";
  output << "declare void @print_bool(i1)\n";
  output << "declare void @println_int(i32)\n";
  output << "declare void @println_float(double)\n";
  output << "declare void @println_string(i8*)\n";
  output << "declare void @println_bool(i1)\n";
  output << "declare i8* @input_string()\n";
  output << "declare i32 @input_int()\n";
  output << "declare double @input_float()\n";
  output << "declare i8* @string_concat(i8*, i8*)\n\n";
}

/**
 * @brief Scans the Abstract Syntax Tree (AST) for string literals and assigns
 * them unique labels.
 *
 * This function recursively traverses the AST. When a string literal is
 * encountered, it is added to the `stringLiterals` map if not already present,
 * and a unique LLVM-style label (e.g., `@.str.0`) is generated and associated
 * with it. This is typically used to prepare string constants for LLVM IR
 * generation.
 *
 * @param node A pointer to the current ASTNode being processed.
 */

void CodeGenerator::scanStrings(const ASTNode *node) {
  if (!node)
    return;

  if (auto *lit = typeCheck<LiteralExpr>(node)) {
    if (lit->token.type == TokenType::LIT_STRING) {
      if (stringLiterals.find(lit->token.value) == stringLiterals.end()) {
        stringLiterals[lit->token.value] =
            "@.str." + std::to_string(labelCount++);
      }
    }
  } else if (auto *prog = typeCheck<Program>(node)) {
    for (const auto &f : prog->functions)
      scanStrings(f.get());
    for (const auto &s : prog->statements)
      scanStrings(s.get());
  } else if (auto *func = typeCheck<FunctionDecl>(node)) {
    scanStrings(func->body.get());
    if (func->arrowBody)
      scanStrings(func->arrowBody.get());
    for (const auto &p : func->parameters) {
      if (p.defaultValue)
        scanStrings(p.defaultValue.get());
    }
  } else if (auto *block = typeCheck<BlockStmt>(node)) {
    for (const auto &s : block->statements)
      scanStrings(s.get());
  } else if (auto *exprStmt = typeCheck<ExprStmt>(node)) {
    scanStrings(exprStmt->expression.get());
  } else if (auto *varDecl = typeCheck<VarDeclStmt>(node)) {
    if (varDecl->initializer)
      scanStrings(varDecl->initializer.get());
  } else if (auto *ret = typeCheck<ReturnStmt>(node)) {
    if (ret->value)
      scanStrings(ret->value.get());
  } else if (auto *ifStmt = typeCheck<IfStmt>(node)) {
    scanStrings(ifStmt->condition.get());
    scanStrings(ifStmt->thenBranch.get());
    if (ifStmt->elseBranch)
      scanStrings(ifStmt->elseBranch.get());
  } else if (auto *whileStmt = typeCheck<WhileStmt>(node)) {
    scanStrings(whileStmt->condition.get());
    scanStrings(whileStmt->body.get());
  } else if (auto *call = typeCheck<CallExpr>(node)) {
    for (const auto &arg : call->arguments)
      scanStrings(arg.get());
  } else if (auto *assign = typeCheck<AssignExpr>(node)) {
    scanStrings(assign->value.get());
  } else if (auto *forStmt = typeCheck<ForStmt>(node)) {
    if (forStmt->initializer)
      scanStrings(forStmt->initializer.get());
    if (forStmt->condition)
      scanStrings(forStmt->condition.get());
    if (forStmt->increment)
      scanStrings(forStmt->increment.get());
    scanStrings(forStmt->body.get());
  }
  // TODO: Add other nodes
}

/**
 * @brief Checks if a given statement guarantees termination of the current
 * control flow.
 *
 * This function recursively inspects a statement to determine if it always
 * results in a control flow termination instruction (e.g., return, break,
 * continue). For block statements, it checks the last statement. For if
 * statements, it ensures both `then` and `else` branches are terminating. This
 * is typically used in code generation to avoid emitting redundant control flow
 * instructions (e.g., `ret void` after an explicit `return`).
 *
 * @param stmt A pointer to the `Stmt` object to be checked.
 * @return True if the statement is a terminator or guarantees termination,
 * false otherwise.
 */
bool CodeGenerator::isTerminator(const Stmt *stmt) {
  if (typeCheck<ReturnStmt>(stmt))
    return true;
  if (typeCheck<BreakStmt>(stmt))
    return true;
  if (typeCheck<ContinueStmt>(stmt))
    return true;
  if (auto *block = typeCheck<BlockStmt>(stmt)) {
    if (!block->statements.empty()) {
      return isTerminator(block->statements.back().get());
    }
  }
  if (auto *ifStmt = typeCheck<IfStmt>(stmt)) {
    if (ifStmt->elseBranch && isTerminator(ifStmt->thenBranch.get()) &&
        isTerminator(ifStmt->elseBranch.get())) {
      return true;
    }
  }
  return false;
}

/**
 * @brief Generates global string constants from the scanned string literals.
 *
 * This function iterates through the `stringLiterals` map. For each string, it
 * generates an LLVM IR constant declaration in the format:
 *
 *   @.str.0 = private unnamed_addr constant [N x i8] c"content\00"
 *
 * where `N` is the length of the string (including the null terminator).
 */
void CodeGenerator::genGlobals() {
  for (const auto &pair : stringLiterals) {
    string content = pair.first;
    string name = pair.second;
    // Calculate length including null terminator
    // LLVM string constants: [N x i8] c"content\00"
    string escaped = escapeString(content);
    output << name << " = private unnamed_addr constant ["
           << (content.length() + 1) << " x i8] c\"" << escaped << "\\00\"\n";
  }
  output << "\n";
  output
      << "@.newline = private unnamed_addr constant [2 x i8] c\"\\0A\\00\"\n";
}

/**
 * @brief Generates LLVM IR code for a function declaration.
 *
 * This function takes a pointer to a `FunctionDecl` object and generates the
 * corresponding LLVM IR code for the function. It handles the function's
 * parameters, body, and return type, and emits the necessary IR instructions
 * to define the function.
 *
 * @param func A pointer to the `FunctionDecl` object to be processed.
 */
void CodeGenerator::genFunction(const FunctionDecl *func) {
  // Reset reg count for each function
  regCount = 0;

  output << "define " << mapType(func->returnType) << " @" << func->name << "(";

  // Parameters
  for (size_t i = 0; i < func->parameters.size(); ++i) {
    if (i > 0)
      output << ", ";
    output << mapType(func->parameters[i].type) << " %"
           << func->parameters[i].name;
  }
  output << ") {\n";
  output << "entry:\n";

  enterScope();

  // Allocate space for parameters to make them mutable
  for (const auto &param : func->parameters) {
    Register ptr = newReg();
    ptr.type = mapType(param.type);

    output << "  " << ptr.name << " = alloca " << ptr.type << "\n";
    output << "  store " << ptr.type << " %" << param.name << ", " << ptr.type
           << "* " << ptr.name << "\n";
    declareVariable(param.name, ptr.name, ptr.type, true);
  }

  if (func->isArrowFunction) { // fn () int => ... ;
    // Arrow function body is a single expression
    Register result = genExpression(func->arrowBody.get());
    if (func->returnType != "void") {
      output << "  ret " << mapType(func->returnType) << " " << result.name
             << "\n";
    } else {
      output << "  ret void\n";
    }
  } else { // fn () int { ... }
    // Block body
    genStatement(func->body.get());

    // Implicit return for void functions or if missing
    if (func->returnType == "void") {
      if (!isTerminator(func->body.get()))
        output << "  ret void\n";
    } else {
      // Add a dummy return 0 for safety if control flow analysis is missing
      // In a real compiler we'd check if all paths return
      if (!isTerminator(func->body.get()))
        output << "  ret " << mapType(func->returnType) << " "
               << getDefaultValue(func->returnType) << "\n";
    }
  }

  exitScope();
  output << "}\n\n";
}

void CodeGenerator::genStatement(const Stmt *stmt) {
  if (auto *block = typeCheck<BlockStmt>(stmt)) {
    enterScope();
    for (const auto &s : block->statements) {
      genStatement(s.get());
    }
    exitScope();
  } else if (auto *exprStmt = typeCheck<const ExprStmt>(stmt)) {
    genExpression(exprStmt->expression.get());
  } else if (auto *varDecl = typeCheck<const VarDeclStmt>(stmt)) {
    genVarDeclStmt(varDecl);
  } else if (auto *ret = typeCheck<const ReturnStmt>(stmt)) {
    genReturnStmt(ret);
  } else if (auto *ifStmt = typeCheck<const IfStmt>(stmt)) {
    genIfStmt(ifStmt);
  } else if (auto *whileStmt = typeCheck<const WhileStmt>(stmt)) {
    genWhileStmt(whileStmt);
  } else if (auto *forStmt = typeCheck<const ForStmt>(stmt)) {
    genForStmt(forStmt);
  } else if (typeCheck<const BreakStmt>(stmt)) {
    genBreakStmt();
  } else if (typeCheck<const ContinueStmt>(stmt)) {
    genContinueStmt();
  }
}

CodeGenerator::Register CodeGenerator::genExpression(const Expr *expr) {
  if (auto *lit = typeCheck<const LiteralExpr>(expr)) {
    return genLiteralExpr(lit);
  } else if (auto *var = typeCheck<const VariableExpr>(expr)) {
    return genVariableExpr(var);
  } else if (auto *bin = typeCheck<const BinaryExpr>(expr)) {
    return genBinaryExpr(bin);
  } else if (auto *call = typeCheck<const CallExpr>(expr)) {
    // Handle built-ins
    if (call->functionName == "print" || call->functionName == "println") {
      for (size_t i = 0; i < call->arguments.size(); ++i) {
        Register arg = genExpression(call->arguments[i].get());

        // Determine if argument is a string or bool
        bool isString = false;
        bool isBool = false;
        const Expr *argExpr = call->arguments[i].get();

        if (auto *lit = typeCheck<const LiteralExpr>(argExpr)) {
          if (lit->token.type == TokenType::LIT_STRING)
            isString = true;
          else if (lit->token.type == TokenType::LIT_TRUE ||
                   lit->token.type == TokenType::LIT_FALSE)
            isBool = true;
        } else if (auto *var = typeCheck<const VariableExpr>(argExpr)) {
          SymbolInfo *info = lookupVariable(var->name);
          if (info) {
            if (info->type == "i8*")
              isString = true;
            else if (info->type == "i1")
              isBool = true;
          }
        } else if (auto *callArg = typeCheck<const CallExpr>(argExpr)) {
          if (functions.count(callArg->functionName)) {
            if (functions[callArg->functionName]->returnType == "string") {
              isString = true;
            } else if (functions[callArg->functionName]->returnType == "bool") {
              isBool = true;
            }
          }
        } else if (auto *bin = typeCheck<const BinaryExpr>(argExpr)) {
          if (bin->op == TokenType::OP_EQUAL ||
              bin->op == TokenType::OP_NOT_EQUAL ||
              bin->op == TokenType::OP_LESS ||
              bin->op == TokenType::OP_GREATER ||
              bin->op == TokenType::OP_LESS_EQUAL ||
              bin->op == TokenType::OP_GREATER_EQUAL) {
            isBool = true;
          }
        }

        string func;
        if (isString) {
          func = "print_string";
        } else {
          func = "print_int"; // Default to int for now
        }

        if (isString) {
          output << "  call void @" << func << "(i8* " << arg.name << ")\n";
        } else {
          string val = arg.name;
          if (isBool) {
            if (arg.name != "1" && arg.name != "0") {
              Register extReg = newReg();
              output << "  " << extReg.name << " = zext i1 " << arg.name
                     << " to i32\n";
              val = extReg.name;
            }
          }
          output << "  call void @" << func << "(i32 " << val << ")\n";
        }
      }

      if (call->functionName == "println") {
        output << "  call void @print_string(i8* getelementptr inbounds ([2 x "
                  "i8], [2 x i8]* @.newline, i64 0, i64 0))\n";
      }
      return Register("0", "i32");
    }

    // Generic call
    vector<Register> argRegs;
    size_t argCount = call->arguments.size();

    // Check for default parameters and get function info
    const FunctionDecl *funcDecl = nullptr;
    if (functions.count(call->functionName)) {
      funcDecl = functions[call->functionName];
    }

    size_t paramCount = funcDecl ? funcDecl->parameters.size() : argCount;

    for (size_t i = 0; i < paramCount; ++i) {
      if (i < argCount) {
        argRegs.push_back(genExpression(call->arguments[i].get()));
      } else if (funcDecl && funcDecl->parameters[i].defaultValue) {
        argRegs.push_back(
            genExpression(funcDecl->parameters[i].defaultValue.get()));
      } else {
        argRegs.push_back(Register("0", "i32")); // Error case
      }
    }

    // 2. Emit call instruction
    string retType = funcDecl ? mapType(funcDecl->returnType) : "i32";
    Register resultReg = Register("0", "i32");

    if (retType == "void") {
      output << "  call void @" << call->functionName << "(";
    } else {
      resultReg = newReg(retType);
      output << "  " << resultReg.name << " = call " << retType << " @"
             << call->functionName << "(";
    }

    for (size_t i = 0; i < argRegs.size(); ++i) {
      if (i > 0)
        output << ", ";

      if (funcDecl && i < funcDecl->parameters.size()) {
        output << mapType(funcDecl->parameters[i].type) << " "
               << argRegs[i].name;
      } else {
        output << argRegs[i].type << " " << argRegs[i].name;
      }
    }

    output << ")\n";
    return resultReg;
  } else if (auto *assign = typeCheck<const AssignExpr>(expr)) {
    SymbolInfo *info = lookupVariable(assign->varName);
    if (info && info->isPtr) {
      Register val = genExpression(assign->value.get());
      // TODO: check types
      output << "  store " << info->type << " " << val.name << ", "
             << info->type << "* " << info->irName << "\n";
      return val;
    } // TODO: Error handling unknown variable
  } else if (auto *ternary = typeCheck<const TernaryExpr>(expr)) {
    Register cond = genExpression(ternary->condition.get());
    string trueLbl = newLabel("true");
    string falseLbl = newLabel("false");
    string mergeLbl = newLabel("merge");

    // We need to know the type. For now assume i32.
    // In real compiler, we check types.
    string type = "i32";

    // TODO: reimplement using phi nodes
    // We need to allocate a slot for the result because LLVM SSA requires
    // phi nodes or memory. Memory is easier.
    Register resPtr = newReg();
    output << "  " << resPtr.name << " = alloca " << type << "\n";

    output << "  br i1 " << cond.name << ", label %" << trueLbl << ", label %"
           << falseLbl << "\n";

    output << trueLbl << ":\n";
    Register trueVal = genExpression(ternary->trueExpr.get());
    output << "  store " << type << " " << trueVal.name << ", " << type << "* "
           << resPtr.name << "\n";
    output << "  br label %" << mergeLbl << "\n";

    output << falseLbl << ":\n";
    Register falseVal = genExpression(ternary->falseExpr.get());
    output << "  store " << type << " " << falseVal.name << ", " << type << "* "
           << resPtr.name << "\n";
    output << "  br label %" << mergeLbl << "\n";

    output << mergeLbl << ":\n";
    Register resultReg = newReg();
    output << "  " << resultReg.name << " = load " << type << ", " << type
           << "* " << resPtr.name << "\n";
    return resultReg;
  }
  // TODO: Add missing expression types such as unary.

  return Register("0", "i32");
}

#include "CodeGenerator.h"

#include <cctype>
#include <cstdio>
#include <string>

void CodeGenerator::enterScope() {
  scopes.push_back(map<string, SymbolInfo>());
}

void CodeGenerator::exitScope() { scopes.pop_back(); }

void CodeGenerator::declareVariable(const string &name, const string &irName,
                                    const string &type, bool isPtr) {
  scopes.back()[name] = {irName, type, isPtr};
}

CodeGenerator::SymbolInfo *CodeGenerator::lookupVariable(const string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return &found->second;
    }
  }
  return nullptr;
}

CodeGenerator::Register CodeGenerator::newReg(string type) {
  return Register("%" + std::to_string(regCount++), type);
}

string CodeGenerator::newLabel(const string &prefix) {
  return prefix + std::to_string(labelCount++);
}

string CodeGenerator::mapType(const string &typeName) {
  if (typeName == "int")
    return "i32";
  if (typeName == "float")
    return "double";
  if (typeName == "bool")
    return "i1";
  if (typeName == "string")
    return "i8*";
  if (typeName == "void")
    return "void";
  return "i8*"; // Default/fallback
}

string CodeGenerator::getDefaultValue(const string &typeName) {
  if (typeName == "int")
    return "0";
  if (typeName == "float")
    return "0.0";
  if (typeName == "bool")
    return "false";
  if (typeName == "string")
    return "null";
  return "null";
}

string CodeGenerator::escapeString(const string &input) {
  string output = "";
  for (char c : input) {
    if (c == '\n') {
      output += "\\0A";
    } else if (c == '\t') {
      output += "\\09";
    } else if (c == '"') {
      output += "\\22";
    } else if (c == '\\') {
      output += "\\5C";
    } else if (isprint(c)) {
      output += c;
    } else {
      char buf[4];
      sprintf(buf, "\\%02X", (unsigned char)c);
      output += buf;
    }
  }
  return output;
}
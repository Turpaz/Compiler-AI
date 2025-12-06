#include "CodeGen/CodeGenerator.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Utility/FileReader.h"
#include <fstream>
#include <iostream>


int main(int argc, char *argv[]) {
  // Read source code from command-line arguments
  std::string code = FileReader::readFromArgs(argc, argv);

  // Tokenize
  Lexer lexer(code);
  auto tokens = lexer.tokenize();

  // Parse
  Parser parser(tokens);
  auto ast = parser.parse();

  if (!ast) {
    std::cerr << "Parsing failed." << std::endl;
    return 1;
  }

  // Generate Code
  CodeGenerator codegen;
  std::string ir = codegen.generate(ast.get());

  // Output to file
  // Check if output file is specified, else default
  std::string outFile = "output.ll";
  if (argc > 2) {
    outFile = argv[2];
  }

  std::ofstream outStream(outFile);
  outStream << ir;
  outStream.close();

  std::cout << "Successfully compiled to " << outFile << std::endl;

  return 0;
}
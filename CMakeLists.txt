cmake_minimum_required(VERSION 3.10)
project(CompilerNew VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# for clangd
#set(DCMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/Lexer/Lexer.cpp
    src/Utility/FileReader.cpp
    src/Parser/Parser.cpp
    src/Parser/AST.cpp
    src/CodeGen/CodeGenerator.cpp
    src/CodeGen/CodegenGenMethods.cpp
    src/CodeGen/CodegenHelperMethods.cpp
)

# Headers
set(HEADERS
    src/Lexer/Lexer.h
    src/Lexer/Token.h
    src/Lexer/tokenType.h
    src/Utility/FileReader.h
    src/Parser/Parser.h
    src/Parser/AST.h
    src/CodeGen/CodeGenerator.h
)

# Executable
add_executable(compiler ${SOURCES} ${HEADERS})

# Output directories
set_target_properties(compiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Compile definitions
target_compile_definitions(compiler PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# Installation
install(TARGETS compiler DESTINATION bin)

cmake_minimum_required(VERSION 2.8.7)
project(mutant)

include_directories(${PROJECT_SOURCE_DIR})

set(APP_SRC
  Loader.cpp
  Lexer.cpp
  GrammarParser.cpp
  Parser.cpp
  WrGen.cpp
  JsGen.cpp
  JsFormatter.cpp
  CppGen.cpp
  CppFormatter.cpp
  Compiler.cpp
  main.cpp
  )

add_executable(mutant ${APP_SRC})

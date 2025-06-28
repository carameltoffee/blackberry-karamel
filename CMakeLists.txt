cmake_minimum_required(VERSION 3.16)
project(YourInterpreterProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(SOURCES
    src/app/main.cpp
    src/interpreter/evaluator.cpp
    src/interpreter/function_manager.cpp
    src/interpreter/interpreter.cpp
    src/interpreter/scope_manager.cpp
    src/interpreter/value.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/interpreter/builtins.cpp
)

add_executable(interpreter ${SOURCES})

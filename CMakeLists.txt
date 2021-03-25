cmake_minimum_required(VERSION 3.17)
project(SAT)

set(CMAKE_CXX_STANDARD 14)

add_executable(SAT main.cpp function.h sat.h Sudoku.h dpll.h)
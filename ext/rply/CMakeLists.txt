cmake_minimum_required(VERSION 2.8)

PROJECT(rply)


ADD_LIBRARY(rply rply.c)
set_target_properties(rply PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

cmake_minimum_required (VERSION 3.8)

project("lisp")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
add_subdirectory(libs/googletest)
enable_testing()
add_subdirectory(tests)
cmake_minimum_required (VERSION 3.8)

project("redstart")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/googletest/CMakeLists.txt")
  add_subdirectory(libs/googletest)
  enable_testing()
  add_subdirectory(tests)
endif()

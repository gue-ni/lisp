if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

set(SRC_FILES "eval.cpp" "builtin.cpp" "expr.cpp" "parser.cpp" "tokenizer.cpp" "gc.cpp" "logger.cpp" )
set(INC_FILES "eval.h" "builtin.h" "expr.h" "parser.h" "tokenizer.h" "lisp.h" "gc.h" "logger.h" )

if(UNIX)
  set(SRC_FILES ${SRC_FILES} "shell.cpp")
  set(INC_FILES ${INC_FILES} "shell.h")
endif()

option(ENABLE_LISP_ASAN "Enable AddressSanitizer for lisp" OFF)

add_library(lisp_lib STATIC ${SRC_FILES} ${INC_FILES})

# Get the current git hash
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set( GIT_HASH_HEADER "${CMAKE_BINARY_DIR}/version.h" )

file(WRITE ${GIT_HASH_HEADER} "#pragma once\n")
file(APPEND ${GIT_HASH_HEADER} "#ifndef GIT_HASH\n")
file(APPEND ${GIT_HASH_HEADER} "#define GIT_HASH \"${GIT_HASH}\"\n")
file(APPEND ${GIT_HASH_HEADER} "#endif\n")

target_include_directories(lisp_lib PUBLIC ${CMAKE_BINARY_DIR})


if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(STATUS "Building on Linux, linking with Readline")
  target_link_libraries(lisp_lib PUBLIC readline tinfo)
endif()

target_include_directories(lisp_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lisp_lib PROPERTY CXX_STANDARD 20)
endif()

add_executable(trilo "main.cpp")
target_link_libraries(trilo lisp_lib)

if (ENABLE_LISP_ASAN AND UNIX AND NOT APPLE)
  message(STATUS "Building lisp with AddressSanitizer")

  target_compile_options(lisp_lib PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
  target_link_options(lisp_lib PRIVATE -fsanitize=address)

  target_compile_options(trilo PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
  target_link_options(trilo PRIVATE -fsanitize=address)

endif()
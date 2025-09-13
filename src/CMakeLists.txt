
set(SRC_FILES "eval.cpp" "builtin.cpp" "expr.cpp" "parser.cpp" "tokenizer.cpp" "gc.cpp")
set(INC_FILES "eval.h" "builtin.h" "expr.h" "parser.h" "tokenizer.h" "lisp.h" "gc.h")


add_library(lisp_lib STATIC ${SRC_FILES} ${INC_FILES})

target_include_directories(lisp_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lisp_lib PROPERTY CXX_STANDARD 20)
endif()

add_executable(lisp "main.cpp")
target_link_libraries(lisp lisp_lib)

if (ENABLE_ASAN AND UNIX AND NOT APPLE)
  message(STATUS "Building lisp with AddressSanitizer")

   target_compile_options(lisp_lib PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
  target_link_options(lisp_lib PRIVATE -fsanitize=address)

  target_compile_options(lisp PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
  target_link_options(lisp PRIVATE -fsanitize=address)

endif()
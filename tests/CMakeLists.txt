option(ENABLE_ASAN "Enable AddressSanitizer for tests" ON)

set(SRC_FILES "test_lisp.cpp")

if(UNIX)
  set(SRC_FILES ${SRC_FILES} "test_shell.cpp")
endif()

add_executable(tests ${SRC_FILES})

target_link_libraries(tests
  gtest
  gtest_main
  lisp_lib
)

if(ENABLE_ASAN AND UNIX AND NOT APPLE)
  message(STATUS "Building tests with AddressSanitizer")
  target_compile_options(tests PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
  target_link_options(tests PRIVATE -fsanitize=address)
endif()

include(GoogleTest)
gtest_discover_tests(tests)

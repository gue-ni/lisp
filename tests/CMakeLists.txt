add_executable(tests "test_lisp.cpp")

target_link_libraries(tests
  gtest
  gtest_main
)

target_link_libraries(tests
  lisp_lib
)


include(GoogleTest)

gtest_discover_tests(tests)

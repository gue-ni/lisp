
set(SRC_FILES "lisp.cpp")

set(INC_FILES "lisp.h")

add_library(lisp_lib STATIC ${SRC_FILES} ${INC_FILES})

target_include_directories(lisp_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lisp_lib PROPERTY CXX_STANDARD 20)
endif()

add_executable(lisp "main.cpp")
target_link_libraries(lisp lisp_lib)
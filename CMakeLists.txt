
cmake_minimum_required(VERSION 3.12)
project(file2header LANGUAGES C CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan.cmake)
conan_cmake_run(BASIC_SETUP CONANFILE conanfile.py BUILD missing)

set(sources src/main.cpp
  src/cxxopts.hpp)

add_executable(file2header ${sources})

target_include_directories(file2header PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(file2header
  PRIVATE
  $<$<CONFIG:DEBUG>:DEBUG>)
target_link_libraries(file2header
  PUBLIC
  ${CONAN_LIBS}
  $<$<PLATFORM_ID:Linux>:dl>)
install(TARGETS file2header)
    
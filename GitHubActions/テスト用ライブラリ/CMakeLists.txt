cmake_minimum_required(VERSION 3.12)
project(PointExample CXX)

option(build_app "app build." ON)
option(build_tests "tests build." OFF)

if(APPLE)
  set(CMAKE_OSX_ARCHITECTURES x86_64)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
include_directories(include)

if(build_app)
  add_executable(point
    src/Main.cpp
  )
endif()

if(build_tests)
  enable_testing()

  add_executable(point-test
    test/Main.cpp
    test/Test.cpp
  )
  target_compile_options(point-test PRIVATE --coverage)
  target_link_options(point-test PRIVATE --coverage)
  target_include_directories(point-test PUBLIC doctest)

  add_test(
    NAME point-test
    COMMAND point-test --reporters=junit --out=point-test.xml
  )
endif()

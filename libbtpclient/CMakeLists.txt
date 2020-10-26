#cmake_minimum_required(VERSION 3.1)
 
#project(libbtpclient)
#include(../cmake/ci.cmake)

add_subdirectory(btpclient)

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
endif()

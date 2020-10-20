cmake_minimum_required(VERSION 3.1)

project(phpbtp-client)

set(CMAKE_CXX_COMPILER /usr/bin/g++-9 CACHE STRING "Compiler" FORCE)

include(cmake/ci.cmake)
wci_getlib(NAME wrtstat SUPERMODULE)

add_library(phpbtp-client main.cpp btp_push.cpp)

add_subdirectory(libbtpclient)

if ( BUILD_TESTING )
  enable_testing()
  #add_subdirectory(tests)
endif()
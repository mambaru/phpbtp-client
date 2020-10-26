cmake_minimum_required(VERSION 3.1)

project(phpbtp-client)

set(CMAKE_LANG_COMPILER_VERSION 9)
#set(CMAKE_CXX_COMPILER /usr/bin/g++-9 CACHE STRING "Compiler" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include(cmake/ci.cmake)
wci_getlib(NAME wrtstat SUPERMODULE)
wci_getlib(NAME wlog)

#add_library(phpbtp-client main.cpp btp_push.cpp)
#wci_targets(phpbtp-client)

add_subdirectory(libbtpclient)

if ( BUILD_TESTING )
  enable_testing()
  #add_subdirectory(tests)
endif()

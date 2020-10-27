cmake_minimum_required(VERSION 3.1)

project(phpbtp-client)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(cmake/ci.cmake)
wci_getlib(NAME wrtstat SUPERMODULE)
wci_getlib(NAME wlog)

if(NOT EXISTS ${CMAKE_BINARY_DIR}/PHP-CPP)
  execute_process(COMMAND git clone http://github.lan/php-extensions/PHP-CPP.git WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

execute_process(COMMAND make -j6 WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/PHP-CPP)

wci_remove_options(-Wzero-as-null-pointer-constant)

add_subdirectory(libbtpclient)

add_custom_command(
  COMMAND make
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/extension/
  DEPENDS btpclient
  OUTPUT phpbtp-client.so
)

add_custom_command(
  COMMAND cp ../extension/phpbtp-client.so ./bin
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS phpbtp-client.so
  OUTPUT ${CMAKE_BINARY_DIR}/bin/phpbtp-client.so
)

add_custom_target(phpbtp-client ALL DEPENDS phpbtp-client.so ${CMAKE_BINARY_DIR}/bin/phpbtp-client.so)

if ( BUILD_TESTING )
  enable_testing()
  #add_subdirectory(tests)
endif()

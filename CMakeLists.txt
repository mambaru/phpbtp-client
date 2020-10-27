cmake_minimum_required(VERSION 3.1)

project(phpbtp-client)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(cmake/ci.cmake)
wci_getlib(NAME wrtstat SUPERMODULE)
wci_getlib(NAME wlog)

add_custom_command(
  COMMAND git clone http://github.lan/php-extensions/PHP-CPP.git 
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  OUTPUT PHP-CPP
)

add_custom_command(
  COMMAND make && DESTDIR="${CMAKE_BINARY_DIR}/PHP-CPP" make install 
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/PHP-CPP
  DEPENDS PHP-CPP
  OUTPUT libphpcpp.a.2.2.0
)

wci_remove_options(-Wzero-as-null-pointer-constant)

add_subdirectory(libbtpclient)

add_custom_command(
  COMMAND BLDDIR=${CMAKE_BINARY_DIR} make
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/extension/
  DEPENDS btpclient libphpcpp.a.2.2.0
  OUTPUT phpbtp-client.so
)

add_custom_command(
  COMMAND mkdir -p bin
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS phpbtp-client.so
  OUTPUT ${CMAKE_BINARY_DIR}/bin
)

add_custom_command(
  COMMAND cp ./extension/phpbtp-client.so ${CMAKE_BINARY_DIR}/bin/
  COMMAND cp ./extension/phpbtp-client.conf ${CMAKE_BINARY_DIR}/bin/
  COMMAND cp ./extension/phpbtp-client.ini ${CMAKE_BINARY_DIR}/bin/
  COMMAND cp ./extension/Makefile-install ${CMAKE_BINARY_DIR}/bin/Makefile
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS phpbtp-client.so ${CMAKE_BINARY_DIR}/bin
  OUTPUT ${CMAKE_BINARY_DIR}/bin/phpbtp-client.so
)

add_custom_target(phpbtp-client ALL DEPENDS phpbtp-client.so ${CMAKE_BINARY_DIR}/bin/phpbtp-client.so)

if ( BUILD_TESTING )
  enable_testing()
  #add_subdirectory(tests)
endif()

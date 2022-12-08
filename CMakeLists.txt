cmake_minimum_required(VERSION 3.3)

project(phpbtp-client)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(-Wzero-as-null-pointer-constant OFF)

if ( NOT BUILD_SHARED_LIBS )
  option(BUILD_SHARED_LIBS "Build shared libs" OFF)
endif()

include(cmake/ci.cmake)
wci_submodule(NAME wrtstat SUPERMODULE)
wci_submodule(NAME wlog)

add_custom_command(
  COMMAND git clone https://gitlab.mamba.ru/php-extensions/PHP-CPP.git
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  OUTPUT PHP-CPP
)

add_custom_command(
  COMMAND mv Makefile Makefile.tmp
  COMMAND sed -e 's/\-Wall//g' Makefile.tmp > Makefile
  COMMAND rm Makefile.tmp
  COMMAND make 
  COMMAND DESTDIR="${CMAKE_BINARY_DIR}/PHP-CPP" make install
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/PHP-CPP
  DEPENDS PHP-CPP
  OUTPUT libphpcpp.a.2.2.0
)

#wci_remove_options(-Wzero-as-null-pointer-constant)

add_subdirectory(libbtpclient)

add_custom_command(
  COMMAND BLDDIR=${CMAKE_BINARY_DIR} make clean
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
  COMMAND cp ./extension/test.php ${CMAKE_BINARY_DIR}/bin/
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

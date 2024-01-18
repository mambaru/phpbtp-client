project(btpclient VERSION "0.9.8")

include(cmake-ci)

add_subdirectory(btpclient)

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
endif()

include(install)

add_subdirectory(btpclient)

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
endif()


add_executable(
  test_btpclient
    test_btpclient.cpp
    udpclient_suite.cpp
)

target_link_libraries(test_btpclient PRIVATE btpclient fas::testing)

wci_test(TARGET test_btpclient)


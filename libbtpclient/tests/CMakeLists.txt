
add_executable(
  test_btpclient
    test_btpclient.cpp
    udpclient_suite.cpp
)

target_link_libraries(test_btpclient PUBLIC btpclient fas::testing)

wci_test(TARGET test_btpclient)

add_executable(confgen confgen.cpp)
target_link_libraries(confgen PRIVATE btpclient wjson)
wci_utils()

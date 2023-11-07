
add_executable(
  test_btpclient
    test_btpclient.cpp
    udpclient_suite.cpp
    pushout_suite.cpp
)

add_executable(
  memtest
  memtest.cpp
)

target_link_libraries(test_btpclient PUBLIC btpclient fas::testing)
target_link_libraries(memtest PUBLIC btpclient)

add_dependencies(memtest btpclient)
wci_test(TARGET test_btpclient)


add_executable(confgen confgen.cpp)
target_link_libraries(confgen PRIVATE btpclient wjson)

wci_utils(confgen memtest)

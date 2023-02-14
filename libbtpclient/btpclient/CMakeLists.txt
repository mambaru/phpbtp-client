
include(FindThreads REQUIRED)


add_library(
  btpclient
    udpclient.cpp
    btpgateway.cpp
    btpclient.cpp
    btpsharding.cpp
    btpglobal.cpp
)

update_ogenc(
  TARGETS btpclient
  WARNINGS -Wnull-dereference
  OFF
)

target_compile_definitions(btpclient PRIVATE -DBOOST_ASIO_NO_DEPRECATED)
wci_targets(btpclient)
get_boost(system)

target_include_directories(btpclient PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(btpclient PUBLIC wrtstat wlog Boost::system Threads::Threads)

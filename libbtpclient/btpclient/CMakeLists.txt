include(FindThreads)
add_library(
  btpclient 
    udpclient.cpp
    btpgateway.cpp
    btpclient.cpp
)
find_package(Boost REQUIRED COMPONENTS system)
target_include_directories(btpclient PUBLIC ${libbtpclient_SOURCE_DIR})
target_link_libraries(btpclient PUBLIC wrtstat wlog Threads::Threads  Boost::system)
wci_targets(btpclient)

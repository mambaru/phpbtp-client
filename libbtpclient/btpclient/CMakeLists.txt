
include(FindThreads)

add_library(
  btpclient 
    udpclient.cpp
    btpgateway.cpp
    btpclient.cpp
    btpsharding.cpp
    btpglobal.cpp
)

wci_targets(btpclient)

find_package(Boost REQUIRED COMPONENTS system)
target_include_directories(btpclient PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(btpclient PUBLIC wrtstat wlog Threads::Threads  Boost::system)


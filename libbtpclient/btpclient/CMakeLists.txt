
include(FindThreads)

add_library(
  btpclient 
    udpclient.cpp
    btpgateway.cpp
    btpclient.cpp
    btpsharding.cpp
    btpglobal.cpp
)

target_compile_definitions(btpclient PRIVATE -DBOOST_ASIO_NO_DEPRECATED)
wci_targets(btpclient)

find_package(Boost REQUIRED COMPONENTS system)
target_include_directories(btpclient PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(btpclient PUBLIC wrtstat wlog Threads::Threads  Boost::system)

#set_source_files_properties
# update_ogenc(TARGETS btpclient WARNINGS -Wzero-as-null-pointer-constant OFF)

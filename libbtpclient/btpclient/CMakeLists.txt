include(FindThreads)
add_library(btpclient udpclient.cpp)
target_include_directories(btpclient PUBLIC ${libbtpclient_SOURCE_DIR})
target_link_libraries(btpclient PUBLIC Threads::Threads wrtstat)

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <btpclient/net/udpclient_options.hpp>
#include <btpclient/net/tcpclient_options.hpp>

namespace wamba{ namespace btp{

struct ipclient_options
{
  udpclient_options udp;
  tcpclient_options tcp;
};
  
}}

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <btpclient/types.hpp>

namespace wamba{ namespace btp{

struct tcpclient_options
{
  bool suspend = true;
  std::string addr;
  std::string port;

  handler_fun test;
  
  static std::vector<std::string> get_schema_list() { return {"devel","production"}; }
  
  static bool create_schema(tcpclient_options& opt, const std::string& name)
  {
    if ( name=="devel")
    {
      opt.addr = "dd2";
      opt.port = "38001";
    }
    else if ( name=="production" )
    {
      opt.addr = "btp";
      opt.port = "38000";
    }
    return true;
  }
};
  
}}

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <btpclient/types.hpp>

namespace wamba{ namespace btp{

struct udpclient_options
{
  std::string addr;
  std::string port;
  
  typedef std::vector<char> data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::function<void(data_ptr)> handler_fun;
  
  handler_fun test;
  
  static std::vector<std::string> get_schema_list() { return {"devel","production"}; }
  
  static bool create_schema(udpclient_options& opt, const std::string& name)
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

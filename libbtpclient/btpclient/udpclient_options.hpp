#pragma once
#include <string>
#include <vector>

namespace wamba{ namespace btp{

struct udpclient_options
{
  std::string addr;
  std::string port;
  
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

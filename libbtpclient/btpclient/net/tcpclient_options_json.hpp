#pragma once
#include <btpclient/net/tcpclient_options.hpp>
#include <wjson/wjson.hpp>
namespace wamba{ namespace btp{

struct tcpclient_options_json
{
  JSON_NAME(suspend)
  JSON_NAME(addr)
  JSON_NAME(port)
  
  typedef wjson::object<
    tcpclient_options,
    wjson::member_list<
      wjson::member<n_suspend, tcpclient_options, bool, &tcpclient_options::suspend>,
      wjson::member<n_addr, tcpclient_options, std::string, &tcpclient_options::addr>,
      wjson::member<n_port, tcpclient_options, std::string, &tcpclient_options::port>
    >
  > type;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
  typedef type::target target;
  
};
  
}}

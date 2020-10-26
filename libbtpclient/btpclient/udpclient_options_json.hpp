#pragma once
#include <btpclient/udpclient_options.hpp>
#include <wjson/wjson.hpp>
namespace wamba{ namespace btp{

struct udpclient_options_json
{
  JSON_NAME(addr);
  JSON_NAME(port);
  
  typedef wjson::object<
    udpclient_options,
    wjson::member_list<
      wjson::member<n_addr, udpclient_options, std::string, &udpclient_options::addr>,
      wjson::member<n_port, udpclient_options, std::string, &udpclient_options::port>
    >
  > type;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
  typedef type::target target;
  
};
  
}}

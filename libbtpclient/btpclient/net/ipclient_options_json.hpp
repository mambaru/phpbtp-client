#pragma once
#include <btpclient/net/udpclient_options_json.hpp>
#include <btpclient/net/tcpclient_options_json.hpp>
#include <wjson/wjson.hpp>

namespace wamba{ namespace btp{

struct ipclient_options_json
{
  JSON_NAME(udp)
  JSON_NAME(tcp)

  typedef wjson::object<
    ipclient_options,
    wjson::member_list<
      wjson::member<n_udp, ipclient_options, udpclient_options, &ipclient_options::udp, udpclient_options_json>,
      wjson::member<n_tcp, ipclient_options, tcpclient_options, &ipclient_options::tcp, tcpclient_options_json>
      >
  > type;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
  typedef type::target target;
};
  
}}

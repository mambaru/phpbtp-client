#pragma once
#include <btpclient/btpgateway_options.hpp>
#include <btpclient/udpclient_options_json.hpp>
#include <wjson/wjson.hpp>
namespace wamba{ namespace btp{

struct btpgateway_options_json
{
  typedef wjson::object<
    btpgateway_options,
    wjson::member_list<
      wjson::base<udpclient_options_json>
    >,
    wjson::strict_mode
  > type;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
  typedef type::target target;
  
};
  
}}

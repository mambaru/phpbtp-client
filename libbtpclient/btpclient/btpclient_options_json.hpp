#pragma once
#include <btpclient/btpgateway_options_json.hpp>
#include <wrtstat/wrtstat_options_json.hpp>
#include <wrtstat/multi_packer/packer_options_json.hpp>
#include <btpclient/btpclient_options.hpp>
#include <wjson/wjson.hpp>

namespace wamba{ namespace btp{

struct btpclient_options_json
{
  JSON_NAME(time_client)
  JSON_NAME(size_client)
  JSON_NAME(aggregator)
  JSON_NAME(packer)
  
  typedef wjson::object<
    btpclient_options,
    wjson::member_list<
      wjson::member<n_aggregator, btpclient_options, wrtstat::wrtstat_options, &btpclient_options::stat, wrtstat::wrtstat_options_json>,
      wjson::member<n_packer, btpclient_options, wrtstat::packer_options, &btpclient_options::packer, wrtstat::packer_options_json>,
      wjson::member<n_time_client, btpclient_options, btpgateway_options, &btpclient_options::time_client, btpgateway_options_json>,
      wjson::member<n_size_client, btpclient_options, btpgateway_options, &btpclient_options::size_client, btpgateway_options_json>
    >,
    wjson::strict_mode
  > type;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
  typedef type::target target;
  
  static std::vector<std::string> get_scheama_list() { return {"devel", "productions"};}
  bool create_schema(btpclient_options& , const std::string& )
  {
    return true;
  }
};
  
}}

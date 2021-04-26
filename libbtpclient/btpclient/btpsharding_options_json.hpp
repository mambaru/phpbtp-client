#pragma once
#include <btpclient/btpsharding_options.hpp>
#include <btpclient/btpclient_options_json.hpp>
#include <wjson/wjson.hpp>
namespace wamba{ namespace btp{

struct btpsharding_options_json
{
  JSON_NAME(shard_weight)
  typedef wjson::object<
    btpshard_options,
    wjson::member_list<
      wjson::member<n_shard_weight, btpshard_options, size_t, &btpshard_options::shard_weight>,
      wjson::base<btpclient_options_json>
    >,
    wjson::strict_mode
  > btpshard_options_json;

  JSON_NAME(script)
  JSON_NAME(service)
  JSON_NAME(server) 
  JSON_NAME(op)
  JSON_NAME(all)
  
  typedef wjson::flags<
    int,
    wjson::member_list<
      wjson::enum_value<n_script, int, int(shard_feature::script)>,
      wjson::enum_value<n_service, int, int(shard_feature::service)>,    
      wjson::enum_value<n_server, int, int(shard_feature::server)>,    
      wjson::enum_value<n_op, int, int(shard_feature::op)>  
    >, ',',
    wjson::strict_mode
  > shard_feature_json;
  
  JSON_NAME(shards)
  JSON_NAME(shard_features)
  JSON_NAME(pushout_timer_s)
  typedef wjson::object<
    btpsharding_options,
    wjson::member_list<
      wjson::member<n_shard_features, btpsharding_options, int, &btpsharding_options::shard_features, shard_feature_json>,
      wjson::member<n_pushout_timer_s, btpsharding_options, time_t, &btpsharding_options::pushout_timer_s>,
      wjson::member<n_shards, btpsharding_options, std::vector<btpshard_options>, &btpsharding_options::shards, 
      wjson::vector_of<btpshard_options_json> >
    >,
    wjson::strict_mode
  > type;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
  typedef type::target target;
  
};
  /*

enum class shard_feature: int
{
  script = 1,
  service = 2,
  server = 4, 
  op = 8,
  all = 15
};
struct btpsharding_options
{
  std::vector<btpshard_options> shards;
  shard_feature shard_features = shard_feature::all;
};
*/
  
}}

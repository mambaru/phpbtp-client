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
  JSON_NAME(pushout_timer_ms)
  JSON_NAME(pushout_thread)
  JSON_NAME(log_path)
  JSON_NAME(suspend)
  typedef wjson::object<
    btpsharding_options,
    wjson::member_list<
      wjson::member<n_suspend, btpsharding_options, bool, &btpsharding_options::suspend>,
      wjson::member<n_shard_features, btpsharding_options, int, &btpsharding_options::shard_features, shard_feature_json>,
      wjson::member<n_pushout_timer_ms, btpsharding_options, time_t, &btpsharding_options::pushout_timer_ms, wjson::time_interval_ms<> >,
      wjson::member<n_pushout_thread, btpsharding_options, bool, &btpsharding_options::pushout_thread >,
      wjson::member<n_log_path, btpsharding_options, std::string, &btpsharding_options::log_path>,
      wjson::member<n_shards, btpsharding_options, std::vector<btpshard_options>, &btpsharding_options::shards,
      wjson::vector_of<btpshard_options_json> >
    >,
    wjson::strict_mode
  > type;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
  typedef type::target target;

};
}}

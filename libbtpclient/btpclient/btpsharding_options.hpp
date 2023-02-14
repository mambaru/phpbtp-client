#pragma once
#include <btpclient/btpclient_options.hpp>
#include <ctime>
namespace wamba{ namespace btp{

struct btpshard_options: btpclient_options
{
  size_t shard_weight=1;
};

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
  int shard_features = int(shard_feature::all);
  time_t pushout_timer_ms = 1000;
  std::string log_path;
  static bool create_schema(btpsharding_options& opt, const std::string&)
  {
    opt.shards.resize(1);
    return true;
  }
};

}}

#pragma once
#include <btpclient/btpclient_options.hpp>

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
  shard_feature shard_features = shard_feature::all;
};
  
}}

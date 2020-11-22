#include "btpsharding.hpp"
#include "btpclient.hpp"
#include <cstring>

namespace wamba{ namespace btp{

namespace{
  static const size_t MAXPROCVAL = 1000000;
}  

btpsharding::~btpsharding()
{
  this->pushout();
}

btpsharding::btpsharding(const btpsharding_options& opt)
  : _opt(opt)
{
  size_t sumval = 0;
  size_t count = 0;
  for ( const auto& shard : opt.shards )
  {
    if ( shard.shard_weight != 0 )
    {
      sumval += shard.shard_weight;
      ++count;
    }
  }
  if ( sumval == 0)
    return;
  
  float koef = float(MAXPROCVAL) / float(sumval);
  float sumweight = 0.0;
  
  for ( const auto& shard : opt.shards )
  {
    if ( shard.shard_weight != 0 )
    {
      sumweight += float(shard.shard_weight) * koef;
      size_t index = size_t(sumweight);
      auto cli = std::make_shared<btpclient>( static_cast<const btpclient_options&>(shard) );
      cli->init_id(_client_list.size(), count);
      _client_list.push_back(std::make_pair(index, cli));
    }
  }
  
}

id_t btpsharding::create_meter(
  const std::string& script,
  const std::string& service, 
  const std::string& server, 
  const std::string& op,
  size_t count,
  size_t write_size)
{
  std::lock_guard<mutex_type> lk(_mutex);
  
  if ( _client_list.empty() )
    return 0;
  
  std::string shard_name = this->shard_name_(script, service, server, op);
  size_t index = this->shard_index_(shard_name);
  auto cli = _client_list.at(index).second;
  return cli->create_meter(script, service, server, op, count, write_size);
}

bool btpsharding::add_time(const std::string& script, const std::string& service, const std::string& server, const std::string& op, 
              time_t ts, size_t count)
{
  std::lock_guard<mutex_type> lk(_mutex);
  
  if ( _client_list.empty() )
    return 0;
  
  std::string shard_name = this->shard_name_(script, service, server, op);
  size_t index = this->shard_index_(shard_name);
  auto cli = _client_list.at(index).second;
  return cli->add_time(script, service, server, op, ts, count);  
}

bool btpsharding::add_size(const std::string& script, const std::string& service, const std::string& server, const std::string& op, 
                           size_t size, size_t count)
{
  std::lock_guard<mutex_type> lk(_mutex);
  
  if ( _client_list.empty() )
    return 0;
  
  std::string shard_name = this->shard_name_(script, service, server, op);
  size_t index = this->shard_index_(shard_name);
  auto cli = _client_list.at(index).second;
  return cli->add_size(script, service, server, op, size, count);  
}

bool btpsharding::release_meter(id_t id, size_t read_size )
{
  std::lock_guard<mutex_type> lk(_mutex);

  if ( id == 0 )
    return false;
  auto cli = _client_list.at( id % _client_list.size() ).second;
  return cli->release_meter( id, read_size );
}

size_t btpsharding::pushout()
{
  std::lock_guard<mutex_type> lk(_mutex);

  size_t count = 0;
  for (auto& cli : _client_list)
    count += cli.second->pushout();
  return count;
}

std::vector<size_t> btpsharding::get_shard_vals() const
{
  std::lock_guard<mutex_type> lk(_mutex);

  std::vector<size_t> result;
  for ( const auto& item : _client_list)
    result.push_back(item.first);
  return result;
}

std::string btpsharding::shard_name_(
  const std::string& script, 
  const std::string& service, 
  const std::string& server, 
  const std::string& op
) const
{

  std::string shard_name;
  if ( int(_opt.shard_features) | int(shard_feature::script) )
    shard_name += script;
  if ( int(_opt.shard_features) | int(shard_feature::service) )
    shard_name += service;
  if ( int(_opt.shard_features) | int(shard_feature::server) )
    shard_name += server;
  if ( int(_opt.shard_features) | int(shard_feature::op) )
    shard_name += op;
  return shard_name;
}

size_t btpsharding::shard_index_(const std::string& shard_name) const
{
  size_t raw = std::hash<std::string>()(shard_name) % MAXPROCVAL;
  auto itr = std::lower_bound(
    std::begin(_client_list), 
    std::end(_client_list), 
    std::make_pair(raw, nullptr), 
    [](const index_client_t& left, const index_client_t& right)
    {
      return left.first < right.first;
    }
  );
  return static_cast<size_t>( std::distance(std::begin(_client_list), itr) );
}

size_t btpsharding::get_shard_index(const std::string& name) const
{
  std::lock_guard<mutex_type> lk(_mutex);
  return this->shard_index_(name);
}


}}

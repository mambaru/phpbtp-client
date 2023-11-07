#include "btpsharding.hpp"
#include "btpclient.hpp"
#include "logger.hpp"
#include <cstring>
#include <iostream>
#include <unistd.h>

namespace wamba{ namespace btp{

namespace{
  static const size_t MAXPROCVAL = 100/*0000*/ /*DEBUG!!!!*/;
}

btpsharding::~btpsharding()
{
  if ( !_points_map.empty() )
    this->stop();
}

void btpsharding::stop()
{
  if ( _pushout_timer_flag )
  {
    _pushout_timer_flag=false;
    _pushout_cv.notify_all();
    _pushout_thread->join();
  }
  size_t res = this->force_pushout();
  wlog::only_for_log(res);
  _points_map.clear();

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

  _pushout_timer_flag = false;
  if ( _opt.pushout_timer_ms > 0 && _opt.pushout_thread )
  {
    _pushout_timer_flag = true;
    time_t pushout_span = _opt.pushout_timer_ms;
    _pushout_thread=std::make_shared<std::thread>([this, pushout_span](){
        while ( this->_pushout_timer_flag )
        {
          std::unique_lock<std::mutex> lk(_pushout_mutex);
          _pushout_cv.wait_for(lk, std::chrono::milliseconds(pushout_span), [this]() -> bool { return !this->_pushout_timer_flag;} );
          if (this->_pushout_timer_flag)
          {
            size_t pushout_count = this->pushout();
            wlog::only_for_log(pushout_count);
          }
        }
    });
  }

  _pushout_time = clock_type::now() + std::chrono::milliseconds(_opt.pushout_timer_ms);
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
  if ( _opt.suspend ) return false;

  this->pushout_by_timer_();
  if ( auto cli = this->get_client_(script, service, server, op) )
    return cli->create_meter(script, service, server, op, count, write_size);
  return wrtstat::bad_id;
}

id_t btpsharding::create_meter(
  size_t count,
  size_t write_size)
{
  std::lock_guard<mutex_type> lk(_mutex);
  if ( _opt.suspend ) return false;

  this->pushout_by_timer_();
  id_t id = ++_time_point_counter;
  point_info pi1;
  pi1.count = count;
  pi1.write_size = write_size;
  pi1.point = clock_type::now();
  _points_map.insert(std::make_pair(id, pi1));
  return id;
}

bool btpsharding::add_time(const std::string& script, const std::string& service, const std::string& server, const std::string& op,
              time_t ts, size_t count)
{
  std::lock_guard<mutex_type> lk(_mutex);
  if ( _opt.suspend ) return false;
  this->pushout_by_timer_();
  if ( auto cli = this->get_client_(script, service, server, op) )
    return cli->add_time(script, service, server, op, ts, count);
  return false;
}

bool btpsharding::add_size(const std::string& script, const std::string& service, const std::string& server, const std::string& op,
                           size_t size, size_t count)
{
  std::lock_guard<mutex_type> lk(_mutex);
  if ( _opt.suspend ) return false;
  this->pushout_by_timer_();
  if ( auto cli = this->get_client_(script, service, server, op) )
    return cli->add_size(script, service, server, op, size, count);
  return false;
}

bool btpsharding::release_meter(id_t id, size_t read_size )
{
  std::lock_guard<mutex_type> lk(_mutex);
  if ( _opt.suspend ) return false;

  this->pushout_by_timer_();
  if ( id == 0 )
    return false;
  auto cli = _client_list.at( id % _client_list.size() ).second;
  return cli->release_meter( id, read_size );
}

bool btpsharding::release_meter(
  id_t id,
  const std::string& script,
  const std::string& service,
  const std::string& server,
  const std::string& op,
  size_t read_size )
{

  auto finish = clock_type::now();
  std::lock_guard<mutex_type> lk(_mutex);
  if ( _opt.suspend ) return false;

  this->pushout_by_timer_();
  auto itr = _points_map.find(id);
  if ( itr == _points_map.end() )
    return false;

  wrtstat::time_type span = std::chrono::duration_cast<std::chrono::microseconds>( finish - itr->second.point ).count();

  if ( auto cli = this->get_client_(script, service, server, op) )
  {
    cli->add_complex(script, service, server, op, span, itr->second.count, itr->second.write_size, read_size);
    _points_map.erase(itr);
    return true;
  }
  _points_map.erase(itr);
  return false;
}

size_t btpsharding::pushout()
{
  std::lock_guard<mutex_type> lk(_mutex);
  if ( _opt.suspend ) return 0;
  return this->pushout_();
}

size_t btpsharding::pushout_by_timer()
{
  std::lock_guard<mutex_type> lk(_mutex);
  if ( _opt.suspend ) return 0;
  return this->pushout_by_timer_();
}

size_t btpsharding::force_pushout()
{
  std::lock_guard<mutex_type> lk(_mutex);
  if ( _opt.suspend ) return 0;
  return this->force_pushout_();
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

btpsharding::client_ptr btpsharding::get_client_(
  const std::string& script,
  const std::string& service,
  const std::string& server,
  const std::string& op) const
{
  if ( _client_list.empty() )
    return nullptr;

  std::string shard_name = this->shard_name_(script, service, server, op);
  size_t index = this->shard_index_(shard_name);
  if ( index < _client_list.size() )
    return _client_list.at(index).second;
  return nullptr;
}

size_t btpsharding::get_shard_index(const std::string& name) const
{
  std::lock_guard<mutex_type> lk(_mutex);
  return this->shard_index_(name);
}

size_t btpsharding::pushout_()
{
  size_t count = 0;
  for (auto& cli : _client_list)
  {
    count += cli.second->pushout();
  }
  _pushout_time = clock_type::now() + std::chrono::milliseconds(_opt.pushout_timer_ms);
  return count;
}

size_t btpsharding::force_pushout_()
{
  size_t count = 0;
  for (auto& cli : _client_list)
  {
    count += cli.second->force_pushout();
  }
  _pushout_time = clock_type::now() + std::chrono::milliseconds(_opt.pushout_timer_ms);
  return count;
}

size_t btpsharding::pushout_by_timer_()
{
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(_pushout_time -  clock_type::now());
  if ( ms.count() < 0 )
  {
    return this->pushout_();
  }
  return 0;
}

}}

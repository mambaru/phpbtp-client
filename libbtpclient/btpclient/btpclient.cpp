#include "btpclient.hpp"
#include <cstring>

namespace wamba{ namespace btp{

namespace
{
  static const char* size_suffix = ".{size}";
}

btpclient::btpclient(const btpclient_options& opt)
  : _opt(opt)
/*  , _packer(nullptr)
  , _gateway(nullptr)*/
{
  using namespace std::placeholders;
  _opt.stat.handler = std::bind(&btpclient::stat_handler_, this, _1, _2);
  _time_gateway = std::make_shared<btpgateway>(_opt.packer.json_compact, _opt.time_client);
  _size_gateway = std::make_shared<btpgateway>(_opt.packer.json_compact, _opt.size_client);
  _time_packer = std::make_shared<packer_type>(opt.packer, std::bind(&btpclient::push_handler_, this, true, _1) );
  _size_packer = std::make_shared<packer_type>(opt.packer, std::bind(&btpclient::push_handler_, this, false, _1) );
}

btpclient::id_t btpclient::create_meter(
  const std::string& script,
  const std::string& service, 
  const std::string& server, 
  const std::string& op,
  size_t count,
  size_t write_size)
{
  std::string statkey = script + "." + service + "." + server;
  auto itr = _wrtstat_map.find(statkey);
  if ( itr == _wrtstat_map.end() )
  {
    wrtstat::wrtstat_options opt = _opt.stat;
    opt.prefixes.push_back("service~~" + service + "~~" + server + "~~");
    opt.prefixes.push_back("service~~" + service + "~~");
    if ( !script.empty() )
      opt.prefixes.push_back("script~~" + script + "~~" + service + "~~" );
  
    auto pwrtstat = std::make_shared<wrtstat::wrtstat>(opt);
    itr = _wrtstat_map.insert(std::make_pair(statkey, pwrtstat) ).first;
  }
  
  id_t cur_id  = ++_id_count;
  
  auto meter = itr->second->create_composite_multi_meter<std::chrono::microseconds>( 
    op, op + ":write" + size_suffix, op + ":read" + size_suffix, true);
  
  auto p = meter.create_shared(count, 0, write_size);
  _points_map.insert(std::make_pair(cur_id, p) );
  return cur_id;
}

bool btpclient::release_meter(id_t id, size_t read_size )
{
  auto itr = _points_map.find(id);
  if (itr == _points_map.end() )
    return false;
  itr->second->set_read_size(read_size);
  _points_map.erase(itr);
  return true;
}

void btpclient::stat_handler_(const std::string& name, wrtstat::aggregated_data::ptr ag)
{
  static const size_t suffix_len = std::strlen(size_suffix);
  bool is_time = true;
  if ( name.size() > suffix_len )
  {
    is_time = name.substr( name.size() - suffix_len) == size_suffix;
  }
  
  if ( is_time )
    _time_packer->push(name, std::move(ag));
  else
    _size_packer->push( name, std::move(ag));
}

size_t btpclient::pushout()
{
  return _time_packer->pushout() + _size_packer->pushout();
}

void btpclient::push_handler_(bool is_time, wrtstat::request::multi_push::ptr req)
{
  if ( is_time )
    _time_gateway->multi_push(std::move(req), nullptr );
  else
    _size_gateway->multi_push(std::move(req), nullptr );
}

}}

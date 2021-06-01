#include "btpclient.hpp"
#include "logger.hpp"
#include <cstring>
#include <iostream>

namespace wamba{ namespace btp{

namespace
{
  static const char* size_suffix = ".{size}";
}

  bool stat_points_maps::release_meter(id_t id, size_t read_size )
  {
    auto itr = composite.find(id);
    if (itr == composite.end() )
    {
      BTP_LOG_DEBUG("stat_points_maps: false")
      return false;
    }
    BTP_LOG_DEBUG("stat_points_maps: true")
    itr->second->set_read_size( static_cast<wrtstat::value_type>(read_size));
    composite.erase(itr);
    return true;
  }


btpclient::btpclient(const btpclient_options& opt)
  : _opt(opt)
{
  using namespace std::placeholders;
  _opt.stat.handler = std::bind(&btpclient::stat_handler_, this, _1, _2);
  _time_gateway = std::make_shared<btpgateway>(_opt.packer.json_compact, _opt.time_client);
  _size_gateway = std::make_shared<btpgateway>(_opt.packer.json_compact, _opt.size_client);
  _time_packer = std::make_shared<packer_type>(opt.packer, std::bind(&btpclient::push_handler_, this, true, _1) );
  _size_packer = std::make_shared<packer_type>(opt.packer, std::bind(&btpclient::push_handler_, this, false, _1) );
}

void btpclient::init_id(id_t start, id_t step)
{
  _id_count = start;
  _id_step = step;
}

id_t btpclient::create_meter(
  const std::string& script,
  const std::string& service, 
  const std::string& server, 
  const std::string& op,
  size_t count,
  size_t write_size)
{
  auto pwrtstat = get_or_cre_(script, service, server);
  
  _id_count += _id_step;
  id_t cur_id = _id_count;
  
  auto meter = pwrtstat->create_composite_multi_meter<std::chrono::microseconds>( 
    op, op + ":write" + size_suffix, op + ":read" + size_suffix, true);
  
  auto p = meter.create_shared(count, static_cast<wrtstat::value_type>(0), static_cast<wrtstat::value_type>(write_size) );
  _points.composite.insert(std::make_pair(cur_id, p) );
  return cur_id;
}

bool btpclient::release_meter(id_t id, size_t read_size )
{
  BTP_LOG_DEBUG("btpclient::release_meter id=" << id << " read_size=" << read_size)
  return _points.release_meter(id, read_size);
}


bool btpclient::add_time(const std::string& script, const std::string& service, const std::string& server, const std::string& op, 
                         time_t ts, size_t count)
{
  auto pwrtstat = get_or_cre_(script, service, server);
  auto meter = pwrtstat->create_value_multi_meter(op);
  meter.create(ts, count);
  return true;
}

bool btpclient::add_size(const std::string& script, const std::string& service, const std::string& server, const std::string& op, 
                         size_t size, size_t count)
{
  auto pwrtstat = get_or_cre_(script, service, server);
  auto meter = pwrtstat->create_value_multi_meter(op + size_suffix);
  meter.create(static_cast<wrtstat::value_type>(size), count );
  return true;
}

bool btpclient::add_complex(const std::string& script, const std::string& service, const std::string& server, const std::string& op, 
                            time_t span, size_t count, size_t read_size, size_t write_size)
{
  auto pwrtstat = get_or_cre_(script, service, server);
  
  auto meter = pwrtstat->create_composite_multi_meter<std::chrono::microseconds>( 
  op, op + ":write" + size_suffix, op + ":read" + size_suffix, true);
  
  meter.create(count, static_cast<wrtstat::value_type>(read_size), static_cast<wrtstat::value_type>(write_size), span );
  return true;
}

size_t btpclient::pushout()
{
  size_t time_count =_time_packer->pushout();
  size_t size_count = _size_packer->pushout();
  BTP_LOG_DEBUG("btpclient::pushout: time_packer:" << time_count)
  BTP_LOG_DEBUG("btpclient::pushout: size_packer:" << size_count)

  return time_count + size_count;
}

btpclient::wrtstat_ptr btpclient::get_or_cre_(const std::string& script, const std::string& service, const std::string& server)
{
  std::string statkey = script + "." + service + "." + server;
  auto itr = _wrtstat_map.find(statkey);
  if ( itr == _wrtstat_map.end() )
  {
    wrtstat::wrtstat_options opt = _opt.stat;
    if ( !service.empty() )
    {
      opt.prefixes.push_back("service~~" + service + "~~" + server + "~~");
      opt.prefixes.push_back("service~~" + service + "~~");
    }
    if ( !script.empty() )
      opt.prefixes.push_back("script~~" + script + "~~" + service + "~~" );
    
    auto pwrtstat = std::make_shared<wrtstat::wrtstat>(opt);
    itr = _wrtstat_map.insert(std::make_pair(statkey, pwrtstat) ).first;
  }
  return itr->second;
}


void btpclient::stat_handler_(const std::string& name, wrtstat::aggregated_data::ptr ag)
{
  BTP_LOG_DEBUG(">>> btpclient::stat_handler_ " << name)
  static const size_t suffix_len = std::strlen(size_suffix);
  bool is_time = true;
  if ( name.size() > suffix_len )
  {
    is_time = name.substr( name.size() - suffix_len) != size_suffix;
  }
  
  if ( is_time )
    _time_packer->push(name, std::move(ag));
  else
  {
    // Нули не пропускаем
    if ( ag->count!=0 || ag->max!=0 )
    {
      std::string sname = name;
      sname.resize(name.size() - suffix_len);
      _size_packer->push( sname, std::move(ag));
    }
  }
}


void btpclient::push_handler_(bool is_time, wrtstat::request::multi_push::ptr req)
{
  if ( is_time )
    _time_gateway->multi_push(std::move(req), nullptr );
  else
    _size_gateway->multi_push(std::move(req), nullptr );
}

}}

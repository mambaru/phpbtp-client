#include "btpclient.hpp"
#include "logger.hpp"
#include <cstring>
#include <iostream>

namespace wamba{ namespace btp{

namespace
{
  static const char* size_suffix = ".{size}";
}

stat_points_maps::stat_points_maps(wrtstat_ptr pwrt, bool only_pack, size_t limit)
  : _limit(limit)
  , _wrtstat(pwrt)
  , _only_pack(only_pack)
{
}

stat_points_maps::meter_fun_t stat_points_maps::make_meter_fun_(const std::string& name, multi_push_ptr req)
{
  return [name, req](
    wrtstat::time_type /*now*/,
    wrtstat::value_type span,
    wrtstat::size_type count,
    wrtstat::value_type /*readed*/,
    wrtstat::value_type /*writed*/)
  {
    wrtstat::request::push p;
    p.name = name;
    p.count = count;
    p.data.push_back(span);

    req->data.push_back(std::move(p));
  };
}

stat_points_maps::point_t stat_points_maps::make_meter_point_(const std::string& name, multi_push_ptr req, size_t count, size_t write_size)
{
  return point_t(
    this->make_meter_fun_(name, req),
    _wrtstat->resolution(), //!!!
    count,
    0,
    static_cast<time_type>(write_size)
  );
}

size_t stat_points_maps::erase_overflow_()
{
  if ( _limit == 0 )
    return 0;

  size_t count = 0;
  while ( _id_list.size() > _limit )
  {
    this->release_meter(_id_list.front(), 0);
    ++count;
  }
  return count;
}

id_t stat_points_maps::create_meter(
  const prefix_list_t& prefixes,
  const std::string& op,
  size_t count, size_t write_size)
{
  if ( _limit == 0 )
    return 0;

  this->erase_overflow_();

  _id_count += _id_step;
  id_t cur_id = _id_count;

  _id_list.push_back(cur_id);
  if ( !_only_pack )
  {
    auto meter = _wrtstat->create_composite_multi_meter<std::chrono::microseconds>(
      prefixes,
      op, op + ":write" + size_suffix, op + ":read" + size_suffix, true);


    auto p = meter.create(count, static_cast<wrtstat::value_type>(0), static_cast<wrtstat::value_type>(write_size) );
    _composite.insert( std::make_pair(cur_id, std::move(p) ) );

  }
  else
  {
    auto req = std::make_shared<wrtstat::request::multi_push>();
    composite_point_t p;
    for (const auto& pref : prefixes)
    {
      p.push_back(this->make_meter_point_( pref + op, req, count, write_size));
    }
    _composite.insert( std::make_pair(cur_id, std::move(p) ) );
    _push_map.insert(std::make_pair(cur_id, req));
  }
  return cur_id;
}


stat_points_maps::multi_push_ptr stat_points_maps::release_meter(id_t id, size_t read_size/*, bool force_pushout*/ )
{
  if ( _limit == 0 )
    return nullptr;

  auto id_itr = std::lower_bound(_id_list.begin(), _id_list.end(), id, std::less<id_t>() );
  if ( id_itr != _id_list.end() && *id_itr == id)
    _id_list.erase(id_itr);

  auto itr = _composite.find(id);
  if (itr == _composite.end() )
  {
    return nullptr;
  }
  itr->second.set_read_size( static_cast<wrtstat::value_type>(read_size));

  _composite.erase(itr);

  auto push_itr = _push_map.find(id);
  if (push_itr == _push_map.end() )
    return nullptr;

  auto req = push_itr->second;
  _push_map.erase(push_itr);
  return req;
}

void stat_points_maps::init_id(id_t start, id_t step)
{
  _id_count = start;
  _id_step = step;
}

size_t stat_points_maps::size() const
{
  return _composite.size();
}

size_t btpclient::size()
{
  return _points->size();
}



btpclient::btpclient(const btpclient_options& opt)
  : _opt(opt)
{
  if ( _opt.open_metrics_limit == 0)
    return;

  using namespace std::placeholders;
  _opt.stat.handler = std::bind(&btpclient::stat_handler_, this, _1, _2);
  _wrtstat = std::make_shared<wrtstat::wrtstat>(_opt.stat);
  _time_gateway = std::make_shared<btpgateway>(_opt.packer.json_compact, _opt.time_client);
  _size_gateway = std::make_shared<btpgateway>(_opt.packer.json_compact, _opt.size_client);
  _time_packer = std::make_shared<packer_type>(opt.packer, std::bind(&btpclient::push_handler_, this, true, _1) );
  _size_packer = std::make_shared<packer_type>(opt.packer, std::bind(&btpclient::push_handler_, this, false, _1) );
  _points = std::make_shared<stat_points_maps>( _wrtstat, _opt.only_pack, _opt.open_metrics_limit );
}

void btpclient::init_id(id_t start, id_t step)
{
  if ( _opt.open_metrics_limit == 0)
    return;

  _points->init_id(start, step);
}

id_t btpclient::create_meter(
  const std::string& script,
  const std::string& service,
  const std::string& server,
  const std::string& op,
  size_t count,
  size_t write_size)
{
  if ( _opt.open_metrics_limit == 0)
    return 1;

  auto prefixes = get_or_cre_(script, service, server);
  return _points->create_meter(prefixes, op, count, write_size);
}

bool btpclient::release_meter(id_t id, size_t read_size )
{
  if ( _opt.open_metrics_limit == 0)
    return true;

  if ( auto req = _points->release_meter(id, read_size) )
  {
    for ( const auto& d : req->data )
    {
      this->stat_handler_(d.name, std::make_unique<wrtstat::aggregated_data>(d) );
    }
  }
  return true;
}


bool btpclient::add_time(const std::string& script, const std::string& service, const std::string& server, const std::string& op,
                         time_t ts, size_t count)
{
  if ( _opt.open_metrics_limit == 0)
    return true;

  auto prefixes = get_or_cre_(script, service, server);
  if ( !_opt.only_pack )
  {
    auto meter = _wrtstat->create_value_multi_meter(prefixes, op);
    meter.create(ts, count);
  }
  else
  {
    for (const auto& pref : prefixes)
    {
      auto ag = std::make_unique<wrtstat::aggregated_data>();
      ag->data.push_back(ts);
      ag->count = count;
      this->stat_handler_( pref + op, std::move(ag) );
    }
  }
  return true;
}

bool btpclient::add_size(const std::string& script, const std::string& service, const std::string& server, const std::string& op,
                         size_t size, size_t count)
{
  if ( _opt.open_metrics_limit == 0)
    return true;

  auto prefixes = get_or_cre_(script, service, server);
  if ( !_opt.only_pack )
  {
    auto meter = _wrtstat->create_value_multi_meter(prefixes, op + size_suffix);
    meter.create(static_cast<wrtstat::value_type>(size), count );
  }
  else
  {
    for (const auto& pref : prefixes)
    {
      auto ag = std::make_unique<wrtstat::aggregated_data>();
      ag->data.push_back( static_cast<wrtstat::time_type>(size) );
      ag->count = count;
      this->stat_handler_( pref + op + size_suffix, std::move(ag) );
    }
  }
  return true;
}

bool btpclient::add_complex(const std::string& script, const std::string& service, const std::string& server, const std::string& op,
                            time_t span, size_t count, size_t read_size, size_t write_size)
{
  if ( _opt.open_metrics_limit == 0)
    return true;

  auto prefixes = get_or_cre_(script, service, server);

  if ( !_opt.only_pack )
  {
    auto meter = _wrtstat->create_composite_multi_meter<std::chrono::microseconds>(
      prefixes, op, op + ":write" + size_suffix, op + ":read" + size_suffix, true);

    meter.create(count, static_cast<wrtstat::value_type>(read_size), static_cast<wrtstat::value_type>(write_size), span );
  }
  else
  {
    for (const auto& pref : prefixes)
    {
      auto ag = std::make_unique<wrtstat::aggregated_data>();
      ag->data.push_back(span);
      ag->count = count;
      this->stat_handler_( pref + op, std::move(ag) );

      ag = std::make_unique<wrtstat::aggregated_data>();
      ag->data.push_back( static_cast<wrtstat::time_type>(write_size) );
      ag->count = write_size;
      this->stat_handler_( pref + op + ":write" + size_suffix, std::move(ag) );

      ag = std::make_unique<wrtstat::aggregated_data>();
      ag->data.push_back( static_cast<wrtstat::time_type>(read_size) );
      ag->count = read_size;
      this->stat_handler_( pref + op + ":read" + size_suffix, std::move(ag) );
    }
  }
  return true;
}

size_t btpclient::pushout()
{
  if ( _opt.open_metrics_limit == 0)
    return 0;

  _wrtstat->pushout();
  size_t time_count =_time_packer->pushout();
  size_t size_count = _size_packer->pushout();
  return time_count + size_count;
}

size_t btpclient::force_pushout()
{
  if ( _opt.open_metrics_limit == 0)
    return 0;

  _wrtstat->force_pushout();
  size_t time_count =_time_packer->pushout();
  size_t size_count = _size_packer->pushout();
  return time_count + size_count;
}

btpclient::prefix_list_t btpclient::get_or_cre_(const std::string& script, const std::string& service, const std::string& server)
{
  prefix_list_t prefixes;

  if ( !service.empty() )
  {
    prefixes.push_back("service~~" + service + "~~" + server + "~~");
    prefixes.push_back("service~~" + service + "~~");
  }
  if ( !script.empty() )
  {
    //prefixes.push_back("script~~" + script + "~~" + service + "~~" + server + "~~");
    prefixes.push_back("script~~" + script + "~~" + service + "~~" );
  }
  return prefixes;
}


void btpclient::stat_handler_(const std::string& name, wrtstat::aggregated_data::ptr ag)
{
  static const size_t suffix_len = std::strlen(size_suffix);
  bool is_time = true;
  if ( name.size() > suffix_len )
  {
    is_time = name.substr( name.size() - suffix_len) != std::string(size_suffix);
  }

  if ( is_time )
  {
    _time_packer->push(name, std::move(ag));
  }
  else
  {
    // Нули не пропускаем
    if ( ag->count!=0 || ag->max!=0 )
    {
      std::string sname{name};
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

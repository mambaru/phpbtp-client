#pragma once

#include <wrtstat/wrtstat.hpp>
#include <wrtstat/multi_packer/basic_packer.hpp>
#include <wrtstat/meters/size_meter.hpp>
#include <wrtstat/meters/value_meter.hpp>
#include <btpclient/btpclient_options.hpp>
#include <btpclient/btpgateway.hpp>
#include <btpclient/types.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/container/flat_map.hpp>
#include <deque>

namespace wamba{ namespace btp{

struct stat_points_maps
{
  typedef std::shared_ptr<wrtstat::wrtstat> wrtstat_ptr;

  typedef std::chrono::microseconds duration_type;

  typedef wrtstat::time_type time_type;
  typedef wrtstat::composite_point<duration_type> point_t;
  typedef wrtstat::multi_point< point_t > composite_point_t;
  typedef std::shared_ptr<composite_point_t> composite_point_ptr;
  typedef std::unordered_map<id_t, composite_point_t> composite_points_map_t;
  typedef std::shared_ptr<wrtstat::request::multi_push> multi_push_ptr;
  typedef std::unordered_map<id_t, multi_push_ptr> push_map_t;
  typedef wrtstat::wrtstat::prefix_list_t prefix_list_t;
  typedef std::deque<id_t> id_list_t;


  stat_points_maps(wrtstat_ptr pwrt, bool only_pack, size_t limit);

  void init_id(id_t start, id_t step);

  size_t size() const;

  id_t create_meter(
    const prefix_list_t& prefixes,
    const std::string& op,
    size_t count, size_t write_size);

  multi_push_ptr release_meter(id_t id, size_t read_size);



private:
  typedef wrtstat::composite_point<duration_type>::meter_fun_t meter_fun_t;
  meter_fun_t make_meter_fun_(const std::string& name, multi_push_ptr req);
  wrtstat::composite_point<duration_type> make_meter_point_(const std::string& name, multi_push_ptr req, size_t count, size_t write_size);

  size_t erase_overflow_();
private:
  id_t _id_count = 0;
  id_t _id_step = 1;
  size_t _limit = 0;
  composite_points_map_t _composite;
  push_map_t _push_map;
  id_list_t _id_list;
  wrtstat_ptr _wrtstat;
  bool _only_pack = false;
};

class btpclient
{
public:
  typedef std::shared_ptr<wrtstat::wrtstat> wrtstat_ptr;
  typedef std::map<std::string, wrtstat_ptr> wrtstat_map_t;
  typedef std::shared_ptr<btpgateway> btpgateway_ptr;
  typedef wrtstat::basic_packer packer_type;
  typedef wrtstat::wrtstat::prefix_list_t prefix_list_t;
  typedef std::shared_ptr<packer_type> packer_ptr;

  explicit btpclient(const btpclient_options& opt);

  void init_id(id_t start, id_t step);

  id_t create_meter(
    const std::string& script,
    const std::string& service,
    const std::string& server,
    const std::string& op,
    size_t count = 1,
    size_t write_size = 0);

  bool release_meter(id_t id, size_t read_size = 0 );

  size_t pushout();

  size_t force_pushout();

  bool add_time(const std::string& script, const std::string& service, const std::string& server, const std::string& op,
                time_t ts, size_t count);

  bool add_size(const std::string& script, const std::string& service, const std::string& server, const std::string& op,
                size_t size, size_t count);

  bool add_complex(const std::string& script, const std::string& service, const std::string& server, const std::string& op,
                   time_t span, size_t count, size_t read_size, size_t write_size);

  size_t size() ;
private:
  void stat_handler_(const std::string& name, wrtstat::aggregated_data::ptr ag);
  void push_handler_(bool is_time, wrtstat::request::multi_push::ptr req);

  prefix_list_t get_or_cre_(const std::string& script, const std::string& service, const std::string& server);
private:
  btpclient_options _opt;
  packer_ptr _time_packer;
  packer_ptr _size_packer;
  btpgateway_ptr _time_gateway;
  btpgateway_ptr _size_gateway;
  // wrtstat_map_t _wrtstat_map;
  wrtstat_ptr _wrtstat;
  std::shared_ptr<stat_points_maps> _points;
};

}}

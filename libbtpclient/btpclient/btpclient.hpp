#pragma once

#include <wrtstat/wrtstat.hpp>
#include <wrtstat/multi_packer/basic_packer.hpp>
#include <wrtstat/meters/size_meter.hpp>
#include <wrtstat/meters/value_meter.hpp>
#include <btpclient/btpclient_options.hpp>
#include <btpclient/btpgateway.hpp>
#include <btpclient/types.hpp>

namespace wamba{ namespace btp{
  
struct stat_points_maps
{
  typedef std::chrono::microseconds duration_type;
  typedef wrtstat::multi_point< wrtstat::time_point<duration_type> > time_point_t; 
  typedef wrtstat::multi_point< wrtstat::size_point > size_point_t; 
  typedef wrtstat::multi_point< wrtstat::value_point > value_point_t; 
  typedef wrtstat::multi_point< wrtstat::composite_point<duration_type> > composite_point_t; 
  typedef std::shared_ptr<time_point_t> time_point_ptr;
  typedef std::shared_ptr<size_point_t> size_point_ptr;
  typedef std::shared_ptr<value_point_t> value_point_ptr;
  typedef std::shared_ptr<composite_point_t> composite_point_ptr;
  typedef std::map<id_t, time_point_ptr> time_points_map_t;
  typedef std::map<id_t, size_point_ptr> size_points_map_t;
  typedef std::map<id_t, value_point_ptr> value_points_map_t;
  typedef std::map<id_t, composite_point_ptr> composite_points_map_t;
  
  time_points_map_t time;
  size_points_map_t size;
  value_points_map_t value;
  composite_points_map_t composite;
  
  bool release_meter(id_t id, size_t read_size );
};

class btpclient
{
public:
  typedef std::shared_ptr<wrtstat::wrtstat> wrtstat_ptr;
  typedef std::map<std::string, wrtstat_ptr> wrtstat_map_t;
  typedef std::shared_ptr<btpgateway> btpgateway_ptr;
  typedef wrtstat::basic_packer packer_type;
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

  bool add_time(const std::string& script, const std::string& service, const std::string& server, const std::string& op, 
                time_t ts, size_t count);  
  
  bool add_size(const std::string& script, const std::string& service, const std::string& server, const std::string& op, 
                size_t size, size_t count);  

  bool add_complex(const std::string& script, const std::string& service, const std::string& server, const std::string& op, 
                   time_t span, size_t count, size_t read_size, size_t write_size);  
private:
  void stat_handler_(const std::string& name, wrtstat::aggregated_data::ptr ag);
  void push_handler_(bool is_time, wrtstat::request::multi_push::ptr req);
  
  wrtstat_ptr get_or_cre_(const std::string& script, const std::string& service, const std::string& server);
private:
  btpclient_options _opt;
  packer_ptr _time_packer;
  packer_ptr _size_packer;
  btpgateway_ptr _time_gateway;
  btpgateway_ptr _size_gateway;
  id_t _id_count = 0;
  id_t _id_step = 1;
  wrtstat_map_t _wrtstat_map;
  stat_points_maps _points;
};
  
}}

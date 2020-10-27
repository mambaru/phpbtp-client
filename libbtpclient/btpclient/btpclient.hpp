#pragma once

#include <wrtstat/wrtstat.hpp>
#include <wrtstat/multi_packer/basic_packer.hpp>
#include <btpclient/btpclient_options.hpp>
#include <btpclient/btpgateway.hpp>
#include <btpclient/types.hpp>

namespace wamba{ namespace btp{
  
class btpclient
{
public:
  typedef std::shared_ptr<wrtstat::wrtstat> wrtstat_ptr;
  typedef std::map<std::string, wrtstat_ptr> wrtstat_map_t;
  typedef std::shared_ptr<btpgateway> btpgateway_ptr;
  typedef wrtstat::basic_packer packer_type;
  typedef std::shared_ptr<packer_type> packer_ptr;
  typedef std::chrono::microseconds duration_type;
  typedef wrtstat::multi_point< wrtstat::composite_point<duration_type> > point_type; 
  typedef std::shared_ptr<point_type> point_ptr;
  typedef std::map<id_t, point_ptr> points_map_t;
  
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
  
  
private:
  void stat_handler_(const std::string& name, wrtstat::aggregated_data::ptr ag);
  void push_handler_(bool is_time, wrtstat::request::multi_push::ptr req);
  
private:
  btpclient_options _opt;
  packer_ptr _time_packer;
  packer_ptr _size_packer;
  btpgateway_ptr _time_gateway;
  btpgateway_ptr _size_gateway;
  id_t _id_count = 0;
  id_t _id_step = 1;
  wrtstat_map_t _wrtstat_map;
  points_map_t _points_map;
};
  
}}

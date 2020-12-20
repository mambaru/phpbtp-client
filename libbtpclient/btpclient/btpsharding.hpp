#pragma once

#include <btpclient/btpsharding_options.hpp>
#include <btpclient/types.hpp>
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <map>

namespace wamba{ namespace btp{
  
class btpclient;

class btpsharding
{
  typedef std::chrono::steady_clock clock_type;
  typedef clock_type::time_point time_point;
  
  struct point_info{
    time_point point = time_point();
    size_t count = 0 ;
    size_t write_size = 0;
  };
  
public:
  typedef std::shared_ptr<btpclient> client_ptr;
  typedef std::pair<size_t, client_ptr> index_client_t;
  typedef std::vector<index_client_t> client_list_t;
  typedef std::mutex mutex_type;
  typedef std::map<id_t, point_info> points_map;
  
  explicit btpsharding(const btpsharding_options& opt);
  
  virtual ~btpsharding();
  
  id_t create_meter(
    const std::string& script,
    const std::string& service, 
    const std::string& server, 
    const std::string& op,
    size_t count = 1,
    size_t write_size = 0);
  
  id_t create_meter(
    size_t count = 1,
    size_t write_size = 0);

  bool release_meter(id_t id, size_t read_size = 0 );

  bool release_meter(
    id_t id, 
    const std::string& script,
    const std::string& service, 
    const std::string& server, 
    const std::string& op,
    size_t read_size = 0 );

  size_t pushout();
  
  std::vector<size_t> get_shard_vals() const;
  
  size_t get_shard_index(const std::string& name) const;

  bool add_time(const std::string& script, const std::string& service, const std::string& server, const std::string& op, 
                time_t ts, size_t count);
  bool add_size(const std::string& script, const std::string& service, const std::string& server, const std::string& op, 
                size_t size, size_t count);
private:
  std::string shard_name_(const std::string& script, const std::string& service, const std::string& server, const std::string& op) const;
  size_t shard_index_(const std::string& shard_name) const;
  client_ptr get_client_(const std::string& script, const std::string& service, const std::string& server, const std::string& op) const;
private:
  btpsharding_options _opt;
  client_list_t _client_list;
  mutable mutex_type _mutex;
  points_map _points_map;
  id_t _time_point_counter = 0;
};
  
}}

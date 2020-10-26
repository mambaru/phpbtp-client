#pragma once

#include <btpclient/btpsharding_options.hpp>
#include <btpclient/types.hpp>
#include <string>
#include <vector>
#include <mutex>

namespace wamba{ namespace btp{
  
class btpclient;

class btpsharding
{
public:
  typedef std::shared_ptr<btpclient> client_ptr;
  typedef std::pair<size_t, client_ptr> index_client_t;
  typedef std::vector<index_client_t> client_list_t;
  typedef std::mutex mutex_type;
  
  btpsharding(const btpsharding_options& opt);
  
  virtual ~btpsharding();
  
  id_t create_meter(
    const std::string& script,
    const std::string& service, 
    const std::string& server, 
    const std::string& op,
    size_t count = 1,
    size_t write_size = 0);
  
  bool release_meter(id_t id, size_t read_size = 0 );

  size_t pushout();
  
  std::vector<size_t> get_shard_vals() const;
  
  size_t get_shard_index(const std::string& name) const;
private:
  std::string shard_name_(const std::string& script, const std::string& service, const std::string& server, const std::string& op) const;
  size_t shard_index_(const std::string& shard_name) const;
private:
  btpsharding_options _opt;
  client_list_t _client_list;
  mutable mutex_type _mutex;
  
};
  
}}
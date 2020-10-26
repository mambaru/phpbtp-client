#include "btpglobal.hpp"
#include "btpsharding.hpp"
#include <atomic>
#include <mutex>

namespace {
class btp_global
{
  typedef std::shared_ptr<wamba::btp::btpsharding> btpsharding_ptr;
public:
  static btp_global* instance()
  {
    btp_global* sin= _instance.load(std::memory_order_acquire);
    if ( !sin )
    {
      std::lock_guard<std::mutex> lock(_s_mutex);
      sin= _instance.load(std::memory_order_relaxed);
      if( !sin ){
        sin= new btp_global();
        _instance.store(sin,std::memory_order_release);
      }
    }   
    // volatile int dummy{};
    return sin;
  }
  
  void initialize(const wamba::btp::btpsharding_options& opt)
  {
    std::lock_guard<std::mutex> lk(_mutex);
    _btpsharding = std::make_shared<wamba::btp::btpsharding>(opt);
  }
  
  btpsharding_ptr get()
  {
    std::lock_guard<std::mutex> lk(_mutex);
    return _btpsharding;
  }
  
private:
  btp_global()= default;
  ~btp_global()= default;
  btp_global(const btp_global&)= delete;
  btp_global& operator=(const btp_global&)= delete;
  
  std::mutex _mutex;
  btpsharding_ptr _btpsharding;
  static std::atomic<btp_global*> _instance;
  static std::mutex _s_mutex;
};

std::atomic<btp_global*> btp_global::_instance;
std::mutex btp_global::_s_mutex;
}


namespace wamba{ namespace btp{
  
void configure(const std::string& /*path*/)
{
  wamba::btp::btpshard_options opt;
  opt.time_client.addr = "dd2";
  opt.time_client.port = "38001";
  opt.size_client.addr = "dd2";
  opt.size_client.port = "38001";
  opt.stat.aggregation_step_ts = 1000000;
  opt.stat.resolution = wrtstat::resolutions::microseconds;
  opt.packer.json_limit=1024;
  
  wamba::btp::btpsharding_options opts;
  opts.shards.push_back(opt);
  opts.shards.back().shard_weight = 10;
  opts.shards.push_back(opt);
  opts.shards.back().shard_weight = 15;
  opts.shards.push_back(opt);
  opts.shards.back().shard_weight = 33;
  opts.shards.push_back(opt);
  opts.shards.back().shard_weight = 1;
  btp_global::instance()->initialize(opts);
}


btp_id_t create_meter(
  const std::string& script,
  const std::string& service, 
  const std::string& server, 
  const std::string& op,
  size_t count,
  size_t write_size)
{
  if ( auto cli = btp_global::instance()->get() )
  {
    return cli->create_meter(script, service, server, op, count, write_size);
  }
  return 0;
}

bool release_meter(btp_id_t id, size_t read_size)
{
  if ( auto cli = btp_global::instance()->get() )
  {
    return cli->release_meter(id, read_size);
  }
  return false;
}

size_t pushout()
{
  if ( auto cli = btp_global::instance()->get() )
  {
    return cli->pushout();
  }
  return 0;
}
}}

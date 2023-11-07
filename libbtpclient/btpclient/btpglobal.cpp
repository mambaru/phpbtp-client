#include "btpglobal.hpp"
#include "btpsharding.hpp"
#include "btpsharding_options_json.hpp"
#include "logger.hpp"
#include <wlog/wlog.hpp>
#include <atomic>
#include <mutex>
#include <fstream>
#include <stdexcept>

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

  void release()
  {
    std::lock_guard<std::mutex> lk(_mutex);
    _btpsharding.reset();
  }

  btpsharding_ptr get()
  {
    std::lock_guard<std::mutex> lk(_mutex);
    return _btpsharding;
  }

private:
  btp_global()= default;
  ~btp_global()
  {
    if (_btpsharding!=nullptr)
      _btpsharding->force_pushout();
  };
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

bool configure(const std::string& path)
{
  if ( btp_global::instance()->get() != nullptr )
  {
    BTP_LOG_DEBUG("wamba::btp::configure ignored");
    return false;
  }

  std::ifstream ifs(path);
  typedef std::istreambuf_iterator<char> iterator;
  wamba::btp::btpsharding_options opt;
  std::string json((iterator(ifs)), iterator());
  if ( json.empty() )
    throw std::domain_error("Bad configuration file " + path);
  wjson::json_error er;
  btpsharding_options_json::serializer()(opt, json.begin(), json.end(), &er);
  if ( er )
    throw std::domain_error(wjson::strerror::message_trace(er, json.begin(), json.end()));


  if ( !opt.log_path.empty() )
  {
    wlog::logger_options lopt;
    lopt.stdout.name="cout";
    lopt.path=opt.log_path;
    lopt.startup_rotate = false;
    wlog::init(lopt);
  }

  btp_global::instance()->initialize(opt);
  return true;
}

void shutdown()
{

  if ( auto cli = btp_global::instance()->get() )
    cli->stop();
  btp_global::instance()->release();
}

void idle_middle()
{
  if ( auto cli = btp_global::instance()->get() )
  {
    size_t count = cli->pushout_by_timer();
    wlog::only_for_log(count);
    BTP_LOG_DEBUG("idle_middle() -2-" << count);
  }
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

  BTP_LOG_ERROR("Extension phpbtp-client is not configured");
  throw std::domain_error("Extension phpbtp-client is not configured");
}

btp_id_t create_meter(
  size_t count,
  size_t write_size)
{
  if ( auto cli = btp_global::instance()->get() )
  {
    return cli->create_meter(count, write_size);
  }

  BTP_LOG_ERROR("Extension phpbtp-client is not configured");
  throw std::domain_error("Extension phpbtp-client is not configured");
}


bool release_meter(
  btp_id_t id,
  size_t read_size)
{
  if ( auto cli = btp_global::instance()->get() )
  {
    return cli->release_meter(id, read_size);
  }
  throw std::domain_error("Extension phpbtp-client is not configured");
}

bool release_meter(
  btp_id_t id,
  const std::string& script,
  const std::string& service,
  const std::string& server,
  const std::string& op,
  size_t read_size)
{
  if ( auto cli = btp_global::instance()->get() )
  {
    return cli->release_meter(id, script, service, server, op, read_size);
  }
  BTP_LOG_ERROR("Extension phpbtp-client is not configured");
  throw std::domain_error("Extension phpbtp-client is not configured");
}


size_t pushout()
{
  if ( auto cli = btp_global::instance()->get() )
  {
    return cli->pushout();
  }
  throw std::domain_error("Extension phpbtp-client is not configured");
}

bool add_time(const std::string& script, const std::string& service, const std::string& server, const std::string& op,
              time_t ts, size_t count)
{
  if ( auto cli = btp_global::instance()->get() )
  {
    return cli->add_time(script, service, server, op, ts, count);
  }

  throw std::domain_error("Extension phpbtp-client is not configured");
}

bool add_size(const std::string& script, const std::string& service, const std::string& server, const std::string& op,
              size_t size, size_t count)
{
  if ( auto cli = btp_global::instance()->get() )
  {
    return cli->add_size(script, service, server, op, size, count);
  }

  throw std::domain_error("Extension phpbtp-client is not configured");
}

}}

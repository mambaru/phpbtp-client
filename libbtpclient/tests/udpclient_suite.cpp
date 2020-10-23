#include <fas/testing.hpp>
#include <btpclient/udpclient.hpp>
#include <btpclient/btpgateway.hpp>
#include <btpclient/btpclient.hpp>
#include <boost/asio.hpp>
#include <wjson/_json.hpp>
#include <fas/system/memory.hpp>

namespace {

const std::string addr = "dd2";
const size_t COUNT = 100;
UNIT(udpclient1, "")
{
  using namespace fas::testing;
  using namespace wamba::btp;
  using namespace wjson::literals;

  udpclient_options opt;
  opt.addr = addr;
  opt.port = "38001";
  udpclient cli;
  t << is_true<expect>( cli.connect(opt) ) << FAS_FL;
  
  std::string json="{'method':'push','id':1,'params':{'name':'service~~name1~~name2~~name3','data':[1,2,3,4,5]}}\r\n"_json;
  t << message("send: ") << json;
  t << flush;
  
  for(int i=0; i < COUNT; ++i)
  {
    cli.send( std::make_unique< udpclient::data_type>(json.begin(), json.end()), nullptr);
    t << message("poll... ") << i;
    t << flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
  t << message("run... ");
  t << nothing;
}

UNIT(btpgateway1, "")
{
  using namespace fas::testing;
  using namespace wamba::btp;
  using namespace wrtstat;
  using namespace wjson::literals;

  btpgateway_options opt;
  opt.addr = addr;
  opt.port = "38001";
  btpgateway cli(false, opt);
  for(int i=0; i < COUNT; ++i)
  {
    auto req=std::make_unique<request::push>();
    req->name = "service~~name1~~name5~~name6";
    t << message("push... ") << i;
    for (int j=0; j < 10; j++)
      req->data.push_back(std::rand() % 10);
    cli.push(std::move(req), nullptr);
  }
}

UNIT(btpclient1, "")
{
  using namespace fas::testing;
  using namespace wamba::btp;
  using namespace wrtstat;
  using namespace wjson::literals;

  btpclient_options opt;
  opt.time_client.addr = addr;
  opt.time_client.port = "38001";
  opt.size_client.addr = addr;
  opt.size_client.port = "38001";
  opt.stat.aggregation_step_ts = 1000000;
  opt.stat.resolution = resolutions::microseconds;
  opt.packer.json_limit=1024;
  
  std::vector<std::string> scripts = {"", "script1", "script2", "script3"};
  std::vector<std::string> services = {"service1", "service2", "service3"};
  std::vector<std::string> servers = {"server1", "server2", "server3"};
  std::vector<std::string> ops = {"op1", "op2", "op3"};
  btpclient cli(opt);
  for (int i=0 ; i < COUNT ; ++i)
  {
    auto id = cli.create_meter(
      scripts[std::rand()%4], 
      services[std::rand()%3],
      servers[std::rand()%3],
      ops[std::rand()%3], 
      1, std::rand()%100);
    //auto id2 = cli.create_meter("script1", "service1", "server2", "op1", 1, 11);
    //auto id3 = cli.create_meter("script1", "service1", "server1", "op1", 1, 11);
    t << message("push... ") << i << " " << id;
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    t << is_true<assert>( cli.release_meter(id, std::rand()%100) ) << FAS_FL;
  }
  size_t push_count = cli.pushout();
  t << message("pushout... ") << push_count;
  ;
}

}

BEGIN_SUITE(udpclient, "")
  //ADD_UNIT(udpclient1)
  //ADD_UNIT(btpgateway1)
  ADD_UNIT(btpclient1)
END_SUITE(udpclient)

#include <fas/testing.hpp>
#include <btpclient/udpclient.hpp>
#include <btpclient/btpgateway.hpp>
#include <btpclient/btpclient.hpp>
#include <btpclient/btpsharding.hpp>
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
  
  for(size_t i=0; i < COUNT; ++i)
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
  for(size_t i=0; i < COUNT; ++i)
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
  for (size_t i=0 ; i < COUNT ; ++i)
  {
    auto id = cli.create_meter(
      scripts[std::rand()%4], 
      services[std::rand()%3],
      servers[std::rand()%3],
      ops[std::rand()%3], 
      1, std::rand()%100
    );
    t << message("push... ") << i << " " << id;
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    t << is_true<assert>( cli.release_meter(id, std::rand()%100) ) << FAS_FL;
  }
  size_t push_count = cli.pushout();
  t << message("pushout... ") << push_count;
  ;
}

UNIT(btpsharding1, "")
{
  using namespace fas::testing;
  using namespace wamba::btp;
  using namespace wrtstat;
  using namespace wjson::literals;
  
  btpshard_options opt;
  opt.time_client.addr = addr;
  opt.time_client.port = "38001";
  opt.size_client.addr = addr;
  opt.size_client.port = "38001";
  opt.stat.aggregation_step_ts = 1000000;
  opt.stat.resolution = resolutions::microseconds;
  opt.packer.json_limit=1024;
  opt.time_client.test = [&t](udpclient_options::data_ptr d)
  {
    t << message("TIME TEST:") << std::string( d->begin(), d->end() ) << FAS_FL;
  };

  btpsharding_options opts;
  opts.shards.push_back(opt);
  opts.shards.back().shard_weight = 10;
  opts.shards.push_back(opt);
  opts.shards.back().shard_weight = 15;
  opts.shards.push_back(opt);
  opts.shards.back().shard_weight = 33;
  opts.shards.push_back(opt);
  opts.shards.back().shard_weight = 1;
  
  btpsharding cli(opts);
  auto shards = cli.get_shard_vals();
  t << equal<assert, size_t>( 4, shards.size() ) << FAS_FL;
  for (size_t i = 0; i < shards.size(); ++i)
  {
    t << message("shard ") << opts.shards[i].shard_weight << " : " << shards[i];
  }
  
  std::map<size_t, size_t> calls;
  for (size_t i = 0; i < 1000 ; ++i)
    calls[cli.get_shard_index(std::to_string(i))]+=1;
  
  t << equal<assert, size_t>( 4, calls.size() ) << FAS_FL;
  size_t summcall = 0;
  for (auto p : calls)
  {
    summcall += p.second;
    t << message("calls ") << p.first << " : " << p.second;
  }
  t << equal<assert, size_t>( 1000, summcall ) << FAS_FL;
  t << equal<assert, size_t>( 156, calls.at(0) ) << FAS_FL;
  t << equal<assert, size_t>( 269, calls.at(1) ) << FAS_FL;
  t << equal<assert, size_t>( 556, calls.at(2) ) << FAS_FL;
  t << equal<assert, size_t>( 19, calls.at(3) ) << FAS_FL;

  std::vector<std::string> scripts = {"", "script1.1", "script2.1", "script3.1"};
  std::vector<std::string> services = {"service1.1", "service2.1", "service3.1"};
  std::vector<std::string> servers = {"server1.1", "server2.1", "server3.1"};
  std::vector<std::string> ops = {"op1.1", "op2.1", "op3.1"};
 
  for (size_t i=0 ; i < COUNT ; ++i)
  {
    auto id = cli.create_meter(
      scripts[std::rand()%4], 
                               services[std::rand()%3],
                               servers[std::rand()%3],
                               ops[std::rand()%3], 
                               1, std::rand()%100
    );
    t << message("push... ") << i << " " << id;
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    t << is_true<assert>( cli.release_meter(id, std::rand()%100) ) << FAS_FL;
  }
  size_t push_count = cli.pushout();
  t << message("pushout... ") << push_count;
  ;
  
}

UNIT(btpsharding2, "")
{
  using namespace fas::testing;
  using namespace wamba::btp;
  using namespace wrtstat;
  using namespace wjson::literals;
  
  btpshard_options opt;
  opt.time_client.addr = addr;
  opt.time_client.port = "38001";
  opt.size_client.addr = addr;
  opt.size_client.port = "38001";
  opt.stat.aggregation_step_ts = 1000000;
  opt.stat.resolution = resolutions::microseconds;
  opt.packer.json_limit=1024;
  opt.time_client.test = [&t](udpclient_options::data_ptr d)
  {
    t << message("TIME TEST:") << std::string( d->begin(), d->end() ) << FAS_FL;
  };
  
  btpsharding_options opts;
  opts.shards.push_back(opt);
  opts.shards.back().shard_weight = 10;
  opts.shards.push_back(opt);
  opts.shards.back().shard_weight = 15;
  opts.shards.push_back(opt);
  opts.shards.back().shard_weight = 33;
  opts.shards.push_back(opt);
  opts.shards.back().shard_weight = 1;
  
  btpsharding cli(opts);
  auto shards = cli.get_shard_vals();
  t << equal<assert, size_t>( 4, shards.size() ) << FAS_FL;
  for (size_t i = 0; i < shards.size(); ++i)
  {
    t << message("shard ") << opts.shards[i].shard_weight << " : " << shards[i];
  }
  
  std::map<size_t, size_t> calls;
  for (size_t i = 0; i < 1000 ; ++i)
    calls[cli.get_shard_index(std::to_string(i))]+=1;
  
  t << equal<assert, size_t>( 4, calls.size() ) << FAS_FL;
  size_t summcall = 0;
  for (auto p : calls)
  {
    summcall += p.second;
    t << message("calls ") << p.first << " : " << p.second;
  }
  t << equal<assert, size_t>( 1000, summcall ) << FAS_FL;
  t << equal<assert, size_t>( 156, calls.at(0) ) << FAS_FL;
  t << equal<assert, size_t>( 269, calls.at(1) ) << FAS_FL;
  t << equal<assert, size_t>( 556, calls.at(2) ) << FAS_FL;
  t << equal<assert, size_t>( 19, calls.at(3) ) << FAS_FL;
  
  std::vector<std::string> scripts = {"", "script1.1", "script2.1", "script3.1"};
  std::vector<std::string> services = {"service1.1", "service2.1", "service3.1"};
  std::vector<std::string> servers = {"server1.1", "server2.1", "server3.1"};
  std::vector<std::string> ops = {"op1.1", "op2.1", "op3.1"};
  
  for (size_t i=0 ; i < COUNT ; ++i)
  {
    auto id = cli.create_meter(
      1, std::rand()%100
    );
    t << message("push... ") << i << " " << id;
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    t << is_true<assert>( cli.release_meter(
      id,       
      scripts[std::rand()%4], 
      services[std::rand()%3],
      servers[std::rand()%3],
      ops[std::rand()%3], 
      std::rand()%100) 
    ) << FAS_FL;
  }
  size_t push_count = cli.pushout();
  t << message("pushout... ") << push_count;
  ;
  
}

} // namespace

BEGIN_SUITE(udpclient, "")
  //ADD_UNIT(udpclient1)
  //ADD_UNIT(btpgateway1)
  //ADD_UNIT(btpclient1)
  //ADD_UNIT(btpsharding1)
  ADD_UNIT(btpsharding2)
END_SUITE(udpclient)

#include <fas/testing.hpp>
#include <btpclient/udpclient.hpp>
#include <boost/asio.hpp>
#include <wjson/_json.hpp>
#include <fas/system/memory.hpp>

namespace {

UNIT(udpclient1, "")
{
  using namespace fas::testing;
  using namespace wamba::btp;
  using namespace wjson::literals;

  udpclient_options opt;
  opt.addr = "0.0.0.0";//"dd2";
  opt.port = "38001";
  boost::asio::io_context context;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> wrk(context.get_executor());
  udpclient cli(context);
  t << is_true<expect>( cli.connect(opt) ) << FAS_FL;

  std::string json="{'method':'push','id':1,'params':{'name':'service~~name1~~name2~~name2','data':[1,2,3,4,5]}}\r\n"_json;
  t << message("send: ") << json;
  t << flush;

  cli.send( std::make_unique< udpclient::data_type>(json.begin(), json.end()), [&]( udpclient::data_ptr d)
  {
    t << message("result:") << std::string(d->begin(), d->end() );
    t << flush;
  }
  );

  t << message("run... ");
  t << flush;
  std::thread thr([&context]{
    context.run();
  });
  thr.join();
  /*for(;;)
  {
    t << message("run... ");
    t << flush;
    context.run_one();
  }*/
  t << nothing;
}

}

BEGIN_SUITE(udpclient, "")
  ADD_UNIT(udpclient1)
END_SUITE(udpclient)

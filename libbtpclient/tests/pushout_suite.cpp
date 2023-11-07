#include <fas/testing.hpp>
#include <btpclient/udpclient.hpp>
#include <btpclient/btpgateway.hpp>
#include <btpclient/btpclient.hpp>
#include <btpclient/btpsharding.hpp>
#include <boost/asio.hpp>
#include <wjson/_json.hpp>
#include <fas/system/memory.hpp>

namespace{

UNIT(pushout1, "")
{
  using namespace fas::testing;
  using namespace wamba::btp;
  using namespace wrtstat;
  using namespace wjson::literals;

  btpclient_options opt;
  opt.only_pack = true;
  opt.stat.resolution = resolutions::milliseconds;
  opt.stat.aggregation_step_ts = 500;
  opt.time_client.addr = "0.0.0.0";
  opt.size_client.addr = "0.0.0.0";
  opt.time_client.test = [](wamba::btp::udpclient_options::data_ptr d)
  {
    std::cout << "--------------[" << std::endl;
    std::cout << std::string( d->begin(), d->end() ) << std::endl;
    std::cout << "]--------------" << std::endl;
  };
  opt.size_client.test = opt.time_client.test;

  btpclient btp(opt);
  auto id = btp.create_meter("s1", "s2", "s3", "op", 1, 0);
  sleep(1);
  btp.pushout();
  btp.release_meter(id, 0);
  sleep(1);
  btp.pushout();
  t << nothing;
}

}// namespace

BEGIN_SUITE(pushout, "")
  ADD_UNIT(pushout1)
END_SUITE(pushout)


#include <fas/testing.hpp>
#include <btpclient/net/udpclient.hpp>
#include <btpclient/btpgateway.hpp>
#include <btpclient/btpclient.hpp>
#include <btpclient/btpsharding.hpp>
#include <boost/asio.hpp>
#include <wjson/_json.hpp>
#include <fas/system/memory.hpp>
#include <wrtstat/api/multi_push_json.hpp>

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
  opt.time_client.udp.addr = "0.0.0.0";
  opt.size_client.udp.addr = "0.0.0.0";
  opt.time_client.udp.test = [](wamba::btp::data_ptr d)
  {
    std::cout << "--------------[" << std::endl;
    std::cout << std::string( d->begin(), d->end() ) << std::endl;
    std::cout << "]--------------" << std::endl;
  };
  opt.size_client.udp.test = opt.time_client.udp.test;

  btpclient btp(opt);
  auto id = btp.create_meter("s1", "s2", "s3", "op", 1, 0);
  sleep(1);
  btp.pushout();
  btp.release_meter(id, 0);
  sleep(1);
  btp.pushout();
  t << nothing;
}


    struct method{ std::string params;};
    JSON_NAME(params)
    struct method_json: wjson::object<
      method,
      wjson::member_list<
        wjson::member<n_params, method, std::string, &method::params, wjson::raw_value<> >
      >
    > {};

UNIT(multimeter_bug1, "Попытка найти баг с потерей")
{
  using namespace fas::testing;
  using namespace wamba::btp;
  using namespace wrtstat;
  using namespace wjson::literals;

  btpclient_options opt;
  opt.only_pack = true;
  opt.packer.name_compact = true;
  opt.stat.resolution = resolutions::milliseconds;
  opt.stat.aggregation_step_ts = 500;
  opt.size_client.tcp.suspend = true;
  opt.time_client.udp.addr = "0.0.0.0";
  opt.time_client.udp.port = "38000";
  opt.size_client.udp.addr = "0.0.0.0";
  opt.size_client.udp.port = "38000";
  size_t out_count1 = 0;
  size_t out_count2 = 0;
  opt.time_client.udp.test = [&t, &out_count1, &out_count2](wamba::btp::data_ptr d)
  {

    wjson::json_error e;
    method m;
    method_json::serializer()(m, d->begin(), d->end(), &e);

    if (e)
    {
      t << fatal(wjson::strerror::message(e)) << FAS_FL;
      t << stop;
      return;
    }

    std::string serr;
    wrtstat::request::multi_push mp;
    wrtstat::request::multi_push_json::serializer()(mp, m.params.begin(), m.params.end(), &e);
    if ( !wrtstat::basic_packer::recompact(&mp, &serr) )
    {
      t << fatal(serr) << FAS_FL;
      t << stop;
      return;
    }
    for (const auto &p : mp.data)
    {
      const std::string prefix1 = "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss2";
      const std::string prefix2 = "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss4";

      std::cout << "<<<<" << p.name << ">>>>"<< std::endl;
      //if ( p.name.size() >= prefix.size() && p.name.compare(0, prefix.size(), prefix) == 0)
      if ( p.name.find(prefix1) != std::string::npos)
        out_count1 += p.count;
      else if ( p.name.find(prefix2) != std::string::npos)
        out_count2 += p.count;
      // else
      //  out_count += p.count;
    }
    std::cout << std::string( d->begin(), d->end() ) << std::endl;
    std::cout << std::endl;
  };
  opt.size_client.udp.test = opt.time_client.udp.test;

  btpclient btp(opt);

  std::cout << std::endl;
  for (int i =0; i < 1000; ++i)
  {
    auto id = btp.create_meter(
      "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss1",
      "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss2",
      "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss3"+std::to_string(i),
      "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss_op", 1, 0);
    auto id2 =    btp.create_meter(
      "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss1",
      "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss4",
      "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss3"+std::to_string(i),
      "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss_op", 1, 0);
    std::cout << id << " ";
    std::cout.flush();
    btp.release_meter(id, 0);
    btp.release_meter(id2, 0);
  }
  std::cout << std::endl;
  sleep(1);
  btp.pushout();
  t << equal<expect>(out_count1, 3000) << FAS_FL;
  t << equal<expect>(out_count2, 3000) << FAS_FL;
}

UNIT(packer_bug1, "Попытка найти баг с потерей")
{
  using namespace fas::testing;
  using namespace wamba::btp;
  using namespace wrtstat;
  using namespace wjson::literals;

  wrtstat::packer_options opt;
  opt.name_compact = true;
  wrtstat::basic_packer bp(opt, nullptr);
  std::string service = "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss1";
  std::string server = "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss2";
  std::string script = "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss3";
  std::string op = "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss_op";

  for (int i =0; i < 1000; ++i)
  {
    aggregated_data::ptr ag = std::make_unique<aggregated_data>();
    ag->count = 1;
    bp.push(service+server+op+std::to_string(i), std::move(ag) );
    ag = std::make_unique<aggregated_data>();
    ag->count = 1;
    bp.push(service+op+std::to_string(i), std::move(ag) );
    ag = std::make_unique<aggregated_data>();
    ag->count = 1;
    bp.push(service+script+op+std::to_string(i), std::move(ag) );
  }

  size_t legend_count = 0;
  while ( auto mp = bp.multi_pop() )
    legend_count += mp->legend.size();
  //wrtstat::basic_packer::compact(mp.get());
  t << equal<expect>(legend_count, 3000) << std::endl;

}

}// namespace

BEGIN_SUITE(pushout, "")
  ADD_UNIT(pushout1)
  ADD_UNIT(multimeter_bug1)
  ADD_UNIT(packer_bug1)
END_SUITE(pushout)


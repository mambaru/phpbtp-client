#include <fas/testing.hpp>
#include <btpclient/udpclient.hpp>
#include <btpclient/btpgateway.hpp>
#include <btpclient/btpclient.hpp>
#include <btpclient/btpsharding.hpp>
#include <boost/asio.hpp>
#include <wjson/_json.hpp>
#include <fas/system/memory.hpp>
#include <deque>

#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

//////////////////////////////////////////////////////////////////////////////
//
// process_mem_usage(double &, double &) - takes two doubles by reference,
// attempts to read the system-dependent data for a process' virtual memory
// size and resident set size, and return the results in KB.
//
// On failure, returns 0.0, 0.0
void process_mem_usage(double& vm_usage, double& resident_set);

void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = double(vsize) / 1024.0;
   resident_set = double(rss * page_size_kb);
   vm_usage /= 1024;
   resident_set /= 1024;
}


int main(int, char**)
{
  using namespace fas::testing;
  using namespace wamba::btp;
  using namespace wrtstat;
  using namespace wjson::literals;
  double vm=0.0, rss=0.0;

  process_mem_usage(vm, rss);
  std::cout << "VM: " << vm << "; RSS: " << rss << std::endl;

  btpsharding_options opt;
  opt.shards.resize(1);
  opt.shards[0].stat.reducer_limit = 1;
  opt.shards[0].stat.reducer_levels = 1;
  opt.shards[0].stat.outgoing_reduced_size=1;

  opt.shards[0].only_pack = true;
  opt.shards[0].open_metrics_limit = 5;
  opt.shards[0].packer.name_compact = true;
  opt.shards[0].packer.json_compact = true;
  opt.shards[0].stat.resolution = resolutions::milliseconds;
  opt.shards[0].stat.aggregation_step_ts = 500;
  opt.shards[0].time_client.addr = "0.0.0.0";
  opt.shards[0].size_client.addr = "0.0.0.0";
  opt.shards[0].time_client.test = [](wamba::btp::udpclient_options::data_ptr d)
  {
    std::cout << std::string( d->begin(), d->end() ) << std::endl;
  };
  opt.shards[0].size_client.test = opt.shards[0].time_client.test;

  for (int k=0; k < 3; ++k)
  {
    std::cout << "===================" << std::endl;
    process_mem_usage(vm, rss);
    std::cout << "VM: " << vm << "; RSS: " << rss << std::endl;
  btpclient btp(opt.shards[0]);
  std::deque<wamba::btp::id_t> ids;
  for (int j=0; j < 4; ++j)
  {
    for (int i =0 ; i < 50000; ++i)
    {
      auto id = btp.create_meter("s"+std::to_string(i)+std::to_string(j)+std::to_string(k),
                                 "s2"+std::to_string(i), "s3"+std::to_string(i), "op"+std::to_string(i), 1, 0);
      ids.push_back(id);
    }
    btp.pushout();

    process_mem_usage(vm, rss);
    std::cout << "VM: " << vm << "; RSS: " << rss << std::endl;
    std::cout << "size=" << btp.size() << std::endl;
    std::cout << "press for continue" << std::endl;
    std::cin.get();
  }


  std::cout << "press for release_meter" << std::endl;
  std::cin.get();
  for (auto id: ids)
    if ( !btp.release_meter(id, 0) )
      abort();
  btp.force_pushout();
  std::cout << "size=" << btp.size() << std::endl;
  }
  process_mem_usage(vm, rss);
  std::cout << "VM: " << vm << "; RSS: " << rss << std::endl;
  std::cout << "press for exit" << std::endl;
  std::cin.get();
  return 0;
}

int main1(int, char**);

int main1(int, char**)
{
  using namespace fas::testing;
  using namespace wamba::btp;
  using namespace wjson::literals;
  typedef std::shared_ptr<wrtstat::wrtstat> wrtstat_ptr;
  typedef std::list<wrtstat_ptr> wrtstat_list_t;

  double vm=0.0, rss=0.0;
  process_mem_usage(vm, rss);
  std::cout << "VM: " << vm << "; RSS: " << rss << std::endl;
  std::cout << "press for exit" << std::endl;
  std::cin.get();

  {
    wrtstat_list_t wlst;
    wrtstat::wrtstat_options opt;
    for (int i = 0; i < 1; i++)
      wlst.push_back( std::make_shared<wrtstat::wrtstat>(opt));
    process_mem_usage(vm, rss);
    std::cout << "VM: " << vm << "; RSS: " << rss << std::endl;
    std::cout << "press for exit" << std::endl;
    std::cin.get();

    for ( auto &l : wlst)
      l=nullptr;
    process_mem_usage(vm, rss);
    std::cout << "VM: " << vm << "; RSS: " << rss << std::endl;
    std::cout << "press for exit" << std::endl;
    std::cin.get();
  }
  process_mem_usage(vm, rss);
  std::cout << "VM: " << vm << "; RSS: " << rss << std::endl;
  std::cout << "press for exit" << std::endl;
  std::cin.get();
  return 0;
}

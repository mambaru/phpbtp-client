#pragma once
#include <btpclient/btpgateway_options.hpp>
#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/multi_packer/packer_options.hpp>

namespace wamba{ namespace btp{

struct btpclient_options
{
  btpgateway_options time_client;
  btpgateway_options size_client;
  
  // Отключает аггрегацию для экономии памяти
  bool only_pack = true;
  size_t open_metrics_limit = 1000000;
  wrtstat::wrtstat_options stat;
  wrtstat::packer_options packer;
};
  
}}

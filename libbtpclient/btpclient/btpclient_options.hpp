#pragma once
#include <btpclient/btpgateway_options.hpp>
#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/multi_packer/packer_options.hpp>

namespace wamba{ namespace btp{

struct btpclient_options
{
  btpgateway_options time_client;
  btpgateway_options size_client;
  
  wrtstat::wrtstat_options stat;
  wrtstat::packer_options packer;
};
  
}}

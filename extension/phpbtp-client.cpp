#include <btpclient/btpglobal.hpp>
#include "phpbtp-client.hpp"
#include <string>
#include <iostream>

namespace{
  
std::string get_string(Php::Parameters& params, size_t pos, const std::string& def = std::string())
{
  if ( pos < params.size() )
    return params[pos];
  return def;
}

template<typename T>
T get_number(Php::Parameters& params, size_t pos, const T& def = T())
{
  if ( pos < params.size() )
    return static_cast<T>(std::stol(params[pos]));
  return def;
}

  
} 

Php::Value btp_configure(Php::Parameters& params)
try
{
  std::string path = get_string(params, 0);
  wamba::btp::configure( path );
  return true;
}
catch(const std::exception& e)
{
  Php::error << e.what() << std::flush;
  return false;
}

Php::Value btp_create_meter(Php::Parameters& params)
try
{
  std::string script = get_string(params,0);
  std::string service = get_string(params,1);
  std::string server = get_string(params,2);
  std::string op = get_string(params,3);
  size_t count = get_number<size_t>(params, 4, 1);
  size_t write_size = get_number<size_t>(params, 5, 0);
  
  int64_t res = wamba::btp::create_meter(script, service, server, op, count, write_size);
  return res;
}
catch(const std::exception& e)
{
  Php::error << e.what() << std::flush;
  return false;
}

Php::Value btp_release_meter(Php::Parameters& params)
try
{
  size_t id = get_number<size_t>(params, 0);
  size_t read_size = get_number<size_t>(params, 1, 0);
  return wamba::btp::release_meter(id, read_size);
}
catch(const std::exception& e)
{
  Php::error << e.what() << std::flush;
  return false;
}

Php::Value btp_pushout(Php::Parameters&)
try
{
  int64_t res = wamba::btp::pushout();
  return res;
}
catch(const std::exception& e)
{
  Php::error << e.what() << std::flush;
  return false;
}

Php::Value btp_add_time(Php::Parameters& params)
try
{
  std::string script = get_string(params,0);
  std::string service = get_string(params,1);
  std::string server = get_string(params,2);
  std::string op = get_string(params,3);
  time_t ts = get_number<time_t>(params, 4, 1);
  size_t count = get_number<size_t>(params, 5, 0);
  
  bool res = wamba::btp::add_time(script, service, server, op, ts, count);
  return res;
  
}
catch(const std::exception& e)
{
  Php::error << e.what() << std::flush;
  return false;
}

Php::Value btp_add_size(Php::Parameters& params)
try
{
  std::string script = get_string(params,0);
  std::string service = get_string(params,1);
  std::string server = get_string(params,2);
  std::string op = get_string(params,3);
  time_t size = get_number<time_t>(params, 4, 1);
  size_t count = get_number<size_t>(params, 5, 0);
  
  bool res = wamba::btp::add_size(script, service, server, op, size, count);
  return res;
}
catch(const std::exception& e)
{
  Php::error << e.what() << std::flush;
  return false;
}


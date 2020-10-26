#include <btpclient/btpglobal.hpp>
#include "btp_push.hpp"
#include <string>

Php::Value btp_configure(Php::Parameters& params)
{
  std::string path = params[0];
  wamba::btp::configure( path );
  return true;
}

Php::Value btp_create_meter(Php::Parameters& params)
{
  std::string script = params[0];
  std::string service = params[1];
  std::string server = params[2];
  std::string op = params[3];
  size_t count = std::stoul(params[4]);
  size_t write_size = std::stoul(params[5]);
  
  int64_t res = wamba::btp::create_meter(script, service, server, op, count, write_size);
  return res;
}

Php::Value btp_release_meter(Php::Parameters& params)
{
  size_t id = std::stoul(params[0]);
  size_t read_size = std::stoul(params[1]);
  return wamba::btp::release_meter(id, read_size);
}

Php::Value btp_pushout(Php::Parameters&)
{
  int64_t res = wamba::btp::pushout();
  return res;
}

/*
Php::Value btp_script_name_set(Php::Parameters&)
{
  return true;
}

Php::Value btp_config_server_set(Php::Parameters&)
{
  return true;
}

Php::Value btp_dump(Php::Parameters&)
{
  return true;
}

Php::Value btp_dump_timer(Php::Parameters&)
{
  return true;
}

Php::Value btp_project_name_set(Php::Parameters&)
{
  return true;
}

Php::Value btp_timer_start(Php::Parameters&)
{
  return true;
}

Php::Value btp_timer_stop(Php::Parameters&)
{
  return true;
}

Php::Value btp_flush(Php::Parameters&)
{
  return true;
}

Php::Value btp_timer_count(Php::Parameters&)
{
  return true;
}

Php::Value btp_timer_count_script(Php::Parameters&)
{
  return true;
}

Php::Value btp_timer_set_operation(Php::Parameters&)
{
  return true;
}
*/

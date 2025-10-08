#include "ipclient.hpp"
#include "../logger.hpp"
#include <fas/system/memory.hpp>
#include <iostream>

namespace wamba{ namespace btp{

ipclient::ipclient()
{
  _udp = std::make_shared<udpclient>(_context);
  _tcp = std::make_shared<tcpclient>(_context);
}

bool ipclient::connect(const ipclient_options& opt)
try
{
  _udp->connect(opt.udp);
  _tcp->connect(opt.tcp);

  _repli_flag = !_udp->is_suspend() && !_tcp->is_suspend();
  return true;
}
catch(const boost::system::error_code& ec)
{
  BTP_LOG_ERROR("ipclient::connect: " << ec.message())
  return false;
}

bool ipclient::send(data_ptr d, handler_fun handler)
try
{
  if ( _repli_flag && handler!=nullptr )
  {
    _udp->send( d, [](data_ptr) noexcept{});
    _tcp->send( d, handler);
  }
  else
  {
    _udp->send( d, handler);
    _tcp->send( d, handler);

  }
  return true;
}
catch(const boost::system::error_code& ec)
{
  BTP_LOG_ERROR("ipclient::send: " << ec.message())
  return false;
}

}}

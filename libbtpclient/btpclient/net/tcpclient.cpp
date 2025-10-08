#include "tcpclient.hpp"
#include "../logger.hpp"
#include <fas/system/memory.hpp>
#include <iostream>

namespace wamba{ namespace btp{

using boost::asio::ip::tcp;

tcpclient::tcpclient(context_type& ct)
  : _context(ct)
  , _socket(ct)
{
}

bool tcpclient::connect(const tcpclient_options& opt)
try
{
  _suspend = opt.suspend;

  _test = opt.test;
  tcp::resolver resolver(_context);

  if ( !opt.addr.empty() && !opt.port.empty() )
  {
    _receiver_endpoint = *resolver.resolve(tcp::v4(), opt.addr, opt.port).begin();
  }
  else
  {
    _suspend = true;
  }

  if ( !_suspend )
  {
    _socket.open(tcp::v4());
    _socket.connect(_receiver_endpoint);
  }
  return true;
}
catch(const boost::system::error_code& ec)
{
  BTP_LOG_ERROR("tcpclient::connect: " << ec.message())
  _suspend = true;
  return false;
}
catch(const std::exception& ec)
{
  BTP_LOG_ERROR("tcpclient::connect: " << ec.what())
  _suspend = true;
  return false;
}
catch(...)
{
  BTP_LOG_ERROR("tcpclient::connect: ... (unknown)" )
  _suspend = true;
  return false;
}

bool tcpclient::send(const data_ptr& d, const handler_fun& handler)
try
{
  if ( _suspend )
    return true;

  BTP_LOG_DEBUG("tcpclient::send")

  if (_test!=nullptr)
  {
    BTP_LOG_DEBUG("tcpclient::send TEST!")
    _test( std::make_unique<data_type>(d->begin(), d->end() ) );
    return true;
  }

  BTP_LOG_DEBUG("tcpclient::send to ")
  // context_type _context;

  _socket.send(boost::asio::buffer(d->data(),d->size()));
  if ( handler != nullptr )
  {
    auto res = std::make_unique<data_type>();
    res->resize(65535);
    size_t bytes_transferred =
      _socket.receive(boost::asio::buffer(res->data(),res->size()));
    res->resize(bytes_transferred);
    handler( std::move(res) );
  }
  else
  {

  }
  return true;
}
catch(const boost::system::error_code& ec)
{
  BTP_LOG_ERROR("tcpclient::send: " << ec.message())
  return false;
}
catch(const std::exception& ec)
{
  BTP_LOG_ERROR("tcpclient::send: " << ec.what())
  return false;
}
catch(...)
{
  BTP_LOG_ERROR("tcpclient::send: ... (unknown)" )
  return false;
}

bool tcpclient::is_suspend()
{
  return _suspend;
}


}}

#include "udpclient.hpp"
#include "../logger.hpp"
#include <fas/system/memory.hpp>
#include <iostream>

namespace wamba{ namespace btp{

using boost::asio::ip::udp;

udpclient::udpclient(context_type& ct)
  : _context(ct)
{
}

bool udpclient::connect(const udpclient_options& opt)
try
{
  _suspend = opt.suspend;

  _test = opt.test;
  udp::resolver resolver(_context);

  if ( !opt.addr.empty() && !opt.port.empty() )
  {
    _receiver_endpoint = *resolver.resolve(udp::v4(), opt.addr, opt.port).begin();
  }
  else
  {
    _suspend = true;
  }

  return true;
}
catch(const boost::system::error_code& ec)
{
  BTP_LOG_ERROR("udpclient::connect: " << ec.message())
  return false;
}

bool udpclient::send(const data_ptr& d, const handler_fun& handler)
try
{
  if ( _suspend )
    return true;

  BTP_LOG_DEBUG("udpclient::send")

  if (_test!=nullptr)
  {
    BTP_LOG_DEBUG("udpclient::send TEST!")
    _test( std::make_unique<data_type>(d->begin(), d->end() ) );
    return true;
  }

  BTP_LOG_DEBUG("udpclient::send to ")

  socket_type _socket(_context); //<-
  _socket.open(udp::v4()); //<-
  _socket.send_to(boost::asio::buffer(d->data(),d->size()), _receiver_endpoint);
  if ( handler != nullptr )
  {
    auto res = std::make_unique<data_type>();
    res->resize(65535);
    udp::endpoint sender_endpoint;
    size_t bytes_transferred =
      _socket.receive_from(boost::asio::buffer(res->data(),res->size()), sender_endpoint);
    res->resize(bytes_transferred);
    handler( std::move(res) );
  }
  else
  {

  }
  _socket.close(); //<-
  return true;
}
catch(const boost::system::error_code& ec)
{
  BTP_LOG_ERROR("udpclient::send: " << ec.message())
  return false;
}

bool udpclient::is_suspend()
{
  return _suspend;
}


}}

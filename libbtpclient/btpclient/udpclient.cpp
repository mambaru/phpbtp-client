#include "udpclient.hpp"
#include "logger.hpp"
#include <fas/system/memory.hpp>

namespace wamba{ namespace btp{

using boost::asio::ip::udp;

udpclient::udpclient()
{
}

bool udpclient::connect(const udpclient_options& opt)
try
{
  _test = opt.test;
  context_type _context;
  udp::resolver resolver(_context);
  _receiver_endpoint = *resolver.resolve(udp::v4(), opt.addr, opt.port).begin();
  return true;
}
catch(const boost::system::error_code& ec)
{
  BTP_LOG_ERROR("udpclient::connect: " << ec.message())
  return false;
}

bool udpclient::send(data_ptr d, handler_fun handler)
try
{
  context_type _context;
  socket_type _socket(_context);
  _socket.open(udp::v4());
  boost::system::error_code ec;
  _socket.send_to(boost::asio::buffer(d->data(),d->size()), _receiver_endpoint);
  if (_test!=nullptr)
    _test( std::move(d) );
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
  return true;
}
catch(const boost::system::error_code& ec)
{
  BTP_LOG_ERROR("udpclient::send: " << ec.message())
  return false;
}

}}

#include "udpclient.hpp"
#include <fas/system/memory.hpp>

namespace wamba{ namespace btp{
  
using boost::asio::ip::udp;
  
udpclient::udpclient(context_type& context)
  : _context(context)
  , _socket(context)
{
}

bool udpclient::connect(const udpclient_options& opt)
{
  udp::resolver resolver(_context);
  boost::system::error_code ec;
  _receiver_endpoint = *resolver.resolve(udp::v4(), opt.addr, opt.port, ec).begin();
  if ( ec )
    return false;
  
  _socket.open(udp::v4(), ec);

  if ( ec )
    return false;
  
  return true;
}

bool udpclient::send(data_ptr d, handler_fun handler)
{
  boost::system::error_code ec;
  _socket.async_send_to(boost::asio::buffer(d->data(),d->size()), _receiver_endpoint, 
  [handler, this](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
      if ( ec )
      {
        if ( handler!=nullptr)
          handler(nullptr);
        return;
      }
      
      _context.post([this, handler](){
        auto res = std::make_shared<data_type>();
        res->resize(65535);
        auto sender_endpoint = std::make_shared<udp::endpoint>();
        _socket.async_receive_from(
          boost::asio::buffer(res->data(),res->size()), *sender_endpoint, 
          [res, handler, sender_endpoint, this](const boost::system::error_code& ec, std::size_t bytes_transferred)
          {
            abort();
            if ( ec )
            {
              if ( handler!=nullptr)
                handler(nullptr);
              return;
            }
            if ( handler!=nullptr)
            {
              handler(std::make_unique<data_type>(std::move(*res)));
            }
          }
        );
      });
    });
  
  return true;
}

  
}}

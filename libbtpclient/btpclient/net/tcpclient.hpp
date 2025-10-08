#pragma once
#include <boost/asio.hpp>
#include <btpclient/net/tcpclient_options.hpp>
#include <btpclient/types.hpp>
namespace wamba{ namespace btp{
  
class tcpclient
{
public:
  typedef boost::asio::io_context context_type; 
  typedef boost::asio::ip::tcp::socket socket_type;
  typedef boost::asio::ip::tcp::endpoint endpoint_type;

  explicit tcpclient(context_type& ct);
  bool connect(const tcpclient_options& opt);
  bool send(const data_ptr& d, const handler_fun& handler);
  bool is_suspend();
private:
  context_type& _context;
  socket_type _socket;
  endpoint_type _receiver_endpoint;
  handler_fun _test;
  bool _suspend = false;
};
  
}}

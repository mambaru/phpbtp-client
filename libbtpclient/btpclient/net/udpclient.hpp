#pragma once
#include <boost/asio.hpp>
#include <btpclient/net/udpclient_options.hpp>
#include <btpclient/types.hpp>
namespace wamba{ namespace btp{
  
class udpclient
{
public:
  typedef boost::asio::io_context context_type; 
  typedef boost::asio::ip::udp::socket socket_type;
  typedef boost::asio::ip::udp::endpoint endpoint_type;

  explicit udpclient(context_type& ct);
  bool connect(const udpclient_options& opt);
  bool send(const data_ptr& d, const handler_fun& handler);
  bool is_suspend();
private:
  context_type& _context;
  endpoint_type _receiver_endpoint;
  handler_fun _test;
  bool _suspend = false;
};
  
}}

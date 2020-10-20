#pragma once
#include <boost/asio.hpp>

namespace wamba{ namespace btp{

struct udpclient_options
{
  std::string addr;
  std::string port;
};
  
class udpclient
{
public:
  typedef boost::asio::io_context context_type; 
  typedef boost::asio::ip::udp::socket socket_type;
  typedef boost::asio::ip::udp::endpoint endpoint_type;
  typedef std::vector<char> data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::function<void(data_ptr)> handler_fun;
  
  explicit udpclient(context_type& _context);
  bool connect(const udpclient_options& opt);
  bool send(data_ptr d, handler_fun handler);
private:
  context_type& _context;
  socket_type _socket;
  endpoint_type _receiver_endpoint;
};
  
}}

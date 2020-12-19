#pragma once
#include <boost/asio.hpp>
#include <btpclient/udpclient_options.hpp>
namespace wamba{ namespace btp{
  
class udpclient
{
public:
  typedef boost::asio::io_context context_type; 
  typedef boost::asio::ip::udp::socket socket_type;
  typedef boost::asio::ip::udp::endpoint endpoint_type;
  typedef udpclient_options::data_type data_type;
  typedef udpclient_options::data_ptr data_ptr;
  typedef udpclient_options::handler_fun handler_fun;
  
  explicit udpclient();
  bool connect(const udpclient_options& opt);
  bool send(data_ptr d, handler_fun handler);
private:
  endpoint_type _receiver_endpoint;
  handler_fun _test;
};
  
}}

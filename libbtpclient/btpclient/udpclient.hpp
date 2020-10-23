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
  typedef std::vector<char> data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::function<void(data_ptr)> handler_fun;
  
  explicit udpclient();
  bool connect(const udpclient_options& opt);
  bool send(data_ptr d, handler_fun handler);
private:
  endpoint_type _receiver_endpoint;
};
  
}}

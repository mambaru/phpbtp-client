#pragma once
#include <boost/asio.hpp>
#include <btpclient/net/ipclient_options.hpp>
#include <btpclient/net/udpclient.hpp>
#include <btpclient/net/tcpclient.hpp>
#include <btpclient/types.hpp>

namespace wamba{ namespace btp{
  
class ipclient
{
public:
  typedef boost::asio::io_context context_type; 
  typedef std::shared_ptr<udpclient> udp_ptr;
  typedef std::shared_ptr<tcpclient> tcp_ptr;
  explicit ipclient();
  bool connect(const ipclient_options& opt);
  bool send(data_ptr d, handler_fun handler);
private:
  context_type _context;
  udp_ptr _udp;
  tcp_ptr _tcp;
  bool _repli_flag = false;
};
  
}}

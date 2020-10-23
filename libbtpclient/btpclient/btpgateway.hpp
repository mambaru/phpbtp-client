#pragma once

#include <wrtstat/api/push.hpp>
#include <wrtstat/api/multi_push.hpp>
#include <btpclient/btpgateway_options.hpp>

namespace wamba{ namespace btp{
  
class udpclient;

class btpgateway
{
public:
  btpgateway(bool json_compact, const btpgateway_options& opt);
  void push(wrtstat::request::push::ptr req, wrtstat::response::push::handler handler = nullptr);
  void multi_push(wrtstat::request::multi_push::ptr req, wrtstat::response::multi_push::handler handler = nullptr);
private:
  bool _json_compact = false;
  btpgateway_options _opt;
  std::shared_ptr<udpclient> _client;
};
  
}}

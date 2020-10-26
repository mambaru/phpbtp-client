#include "btpgateway.hpp"
#include "udpclient.hpp"
#include "logger.hpp"
#include <wrtstat/api/push_json.hpp>
#include <wrtstat/api/push_json_compact.hpp>
#include <wrtstat/api/multi_push_json.hpp>
#include <wrtstat/api/multi_push_json_compact.hpp>
#include <wjson/_json.hpp>
#include <sstream>
#include <fas/system/memory.hpp>

namespace wamba{ namespace btp{

namespace
{
  struct push_result
  {
    typedef std::shared_ptr<wrtstat::response::push> response_ptr;
    response_ptr result;
    std::string error;
  };

  template<typename J>
  struct push_result_json_t
  {
    JSON_NAME(result)
    JSON_NAME(error)
    
    typedef wjson::pointer<push_result::response_ptr, J> ptr_json;
    typedef wjson::object<
      push_result,
      wjson::member_list<
        wjson::member<n_result, push_result, push_result::response_ptr, &push_result::result, ptr_json>,
        wjson::member<n_error, push_result, std::string, &push_result::error, wjson::raw_value<> >
      >
    > type;
    
  };

  struct push_result_json: push_result_json_t<wrtstat::response::push_json> {};
  struct push_result_json_compact: push_result_json_t<wrtstat::response::push_json_compact> {};
}

namespace
{
  struct multi_push_result
  {
    typedef std::shared_ptr<wrtstat::response::multi_push> response_ptr;
    response_ptr result;
    std::string error;
  };
  
  template<typename J>
  struct multi_push_result_json_t
  {
    JSON_NAME(result)
    JSON_NAME(error)
    
    typedef wjson::pointer<multi_push_result::response_ptr, J> ptr_json;
    typedef wjson::object<
      multi_push_result,
      wjson::member_list<
        wjson::member<n_result, multi_push_result, multi_push_result::response_ptr, &multi_push_result::result, ptr_json>,
        wjson::member<n_error, multi_push_result, std::string, &multi_push_result::error, wjson::raw_value<> >
      >
    > type;
  };
  
  struct multi_push_result_json: multi_push_result_json_t<wrtstat::response::multi_push_json> {};
  struct multi_push_result_json_compact: multi_push_result_json_t<wrtstat::response::multi_push_json_compact> {};
}

  
btpgateway::btpgateway(bool json_compact, const btpgateway_options& opt)
 : _json_compact(json_compact)
 , _opt(opt)
 , _client(std::make_shared<udpclient>())
{
  _client->connect(_opt);
}

void btpgateway::push(wrtstat::request::push::ptr req, wrtstat::response::push::handler handler)
{
  using namespace wjson::literals;
  static const std::string pref_ntf = "{'method':'push','params':"_json;
  static const std::string pref_req = "{'id':1,'method':'push','params':"_json;
  auto json_req = handler == nullptr
    ? std::make_unique<udpclient::data_type>( std::begin(pref_ntf), std::end(pref_ntf) )
    : std::make_unique<udpclient::data_type>( std::begin(pref_req), std::end(pref_req) );
  
  if ( _json_compact )
    wrtstat::request::push_json_compact::serializer()(*req, std::back_inserter(*json_req) );
  else
    wrtstat::request::push_json::serializer()(*req, std::back_inserter(*json_req) );
  
  json_req->push_back('}');
  json_req->push_back('\r');
  json_req->push_back('\n');
  if (handler == nullptr)
  {
    _client->send(std::move(json_req), nullptr);
  }
  else
  {
    bool compact = _json_compact;
    _client->send(std::move(json_req), [compact, handler](udpclient::data_ptr json_res){
      if ( json_res == nullptr )
      {
        handler(nullptr);
      }
      else
      {
        wjson::json_error er;
        push_result rpcres;
        if ( compact)
          push_result_json_compact::type::serializer()(rpcres, json_res->begin(), json_res->end(), &er);
        else
          push_result_json::type::serializer()(rpcres, json_res->begin(), json_res->end(), &er);
        if ( er )
        {
          BTP_LOG_ERROR("btpgateway::push result handler JSON error: " << wjson::strerror::message(er) );
          handler(nullptr);
        }
        else if ( rpcres.result!=nullptr )
        {
          auto res = std::make_unique<wrtstat::response::push>( std::move(*rpcres.result) );
          handler( std::move(res) );
        }
        else
        {
          BTP_LOG_ERROR("btpgateway::push result error: " << rpcres.error );
          handler(nullptr);
        }
      }
    });   
  }
}
  
  
void btpgateway::multi_push(wrtstat::request::multi_push::ptr req, wrtstat::response::multi_push::handler handler)
{
  using namespace wjson::literals;
  static const std::string pref_ntf = "{'method':'multi_push','params':"_json;
  static const std::string pref_req = "{'id':1,'method':'multi_push','params':"_json;
  auto json_req = handler == nullptr
    ? std::make_unique<udpclient::data_type>( std::begin(pref_ntf), std::end(pref_ntf) )
    : std::make_unique<udpclient::data_type>( std::begin(pref_req), std::end(pref_req) );
  
  if ( _json_compact )
    wrtstat::request::multi_push_json_compact::serializer()(*req, std::back_inserter(*json_req) );
  else
    wrtstat::request::multi_push_json::serializer()(*req, std::back_inserter(*json_req) );
  
  json_req->push_back('}');
  json_req->push_back('\r');
  json_req->push_back('\n');
  
  if (handler == nullptr)
  {
    _client->send(std::move(json_req), nullptr);
  }
  else
  {
    bool compact = _json_compact;
    _client->send(std::move(json_req), [compact, handler](udpclient::data_ptr json_res){
      if ( json_res == nullptr )
      {
        handler(nullptr);
      }
      else
      {
        wjson::json_error er;
        multi_push_result rpcres;
        if ( compact)
          multi_push_result_json_compact::type::serializer()(rpcres, json_res->begin(), json_res->end(), &er);
        else
          multi_push_result_json::type::serializer()(rpcres, json_res->begin(), json_res->end(), &er);
        if ( er )
        {
          BTP_LOG_ERROR("btpgateway::multi_push result handler JSON error: " << wjson::strerror::message(er) );
          handler(nullptr);
        }
        else if ( rpcres.result!=nullptr )
        {
          auto res = std::make_unique<wrtstat::response::multi_push>( std::move(*rpcres.result) );
          handler( std::move(res) );
        }
        else
        {
          BTP_LOG_ERROR("btpgateway::push result error: " << rpcres.error );
          handler(nullptr);
        }
      }
    });   
  }
}

  
}}

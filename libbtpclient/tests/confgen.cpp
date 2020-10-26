#include <btpclient/btpsharding_options_json.hpp>
#include <wjson/schema.hpp>
#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
  typedef wamba::btp::btpsharding_options_json config_json;
  typedef config_json::target config_type;
  
  if ( argc==2 && ( std::string(argv[1])=="--help" || std::string(argv[1])=="-h" ) )
  {
    auto list = wjson::schema::get_schema_list<config_type>();
    std::cout << "Доступные схемы:" << std::endl;
    for ( std::string name : list)
      std::cout << "\t" << name << std::endl;
    return 0;
  }
  
  std::vector<std::string> names;
  for (int i = 1; i < argc; ++i)
    names.push_back(argv[i]);
  
  config_type config;
  wjson::schema::create_schema(config, names);
  config_json::serializer()(config, std::ostreambuf_iterator<char>(std::cout) );
  std::cout << std::endl;
  return 0;
}

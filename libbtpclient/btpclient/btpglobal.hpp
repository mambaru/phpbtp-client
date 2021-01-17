#pragma once

#include <string>

typedef size_t btp_id_t;

namespace wamba{ namespace btp{
  
void configure(const std::string& path);
  
btp_id_t create_meter(
    const std::string& script,
    const std::string& service, 
    const std::string& server, 
    const std::string& op,
    size_t count = 1,
    size_t write_size = 0);
  
btp_id_t create_meter(
    size_t count = 1,
    size_t write_size = 0);

bool release_meter(btp_id_t id, size_t read_size = 0 );

bool release_meter(
  btp_id_t id, 
  const std::string& script,
  const std::string& service, 
  const std::string& server, 
  const std::string& op,
  size_t read_size = 0 
);

size_t pushout();


bool add_time(const std::string& script, const std::string& service, const std::string& server, const std::string& op, 
              time_t ts, size_t count);  

bool add_size(const std::string& script, const std::string& service, const std::string& server, const std::string& op, 
              size_t size, size_t count);  
}}

#pragma once
#include <wrtstat/aggregator/api/types.hpp>

namespace wamba{ namespace btp{

typedef wrtstat::id_t id_t;

//constexpr id_t bad_id = wrtstat::bad_id;

typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;
typedef std::function<void(data_ptr)> handler_fun;

}}

#pragma once

#include <phpcpp/visibility.h>
#include <phpcpp.h>

Php::Value btp_config_server_pool(Php::Parameters&);
Php::Value btp_script_name_set(Php::Parameters&);

Php::Value btp_config_server_set(Php::Parameters&);
Php::Value btp_dump(Php::Parameters&);
Php::Value btp_dump_timer(Php::Parameters&);
Php::Value btp_project_name_set(Php::Parameters&);
Php::Value btp_timer_start(Php::Parameters&);
Php::Value btp_timer_stop(Php::Parameters&);
Php::Value btp_flush(Php::Parameters&);
Php::Value btp_timer_count(Php::Parameters&);
Php::Value btp_timer_count_script(Php::Parameters&);
Php::Value btp_timer_set_operation(Php::Parameters&);

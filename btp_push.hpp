#pragma once 

#include <phpcpp.h>

Php::Value btp_config_server_pool(Php::Parameters &params);
Php::Value btp_script_name_set(Php::Parameters &params);

Php::Value btp_config_server_set(Php::Parameters &params);
Php::Value btp_dump(Php::Parameters &params);
Php::Value btp_dump_timer(Php::Parameters &params);
Php::Value btp_project_name_set(Php::Parameters &params);
Php::Value btp_timer_start(Php::Parameters &params);
Php::Value btp_timer_stop(Php::Parameters &params);
Php::Value btp_flush(Php::Parameters &params);
Php::Value btp_timer_count(Php::Parameters &params);
Php::Value btp_timer_count_script(Php::Parameters &params);
Php::Value btp_timer_set_operation(Php::Parameters &params);

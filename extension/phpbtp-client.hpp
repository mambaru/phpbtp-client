#pragma once

#include <stdexcept>
#include <phpcpp/visibility.h>
#include <phpcpp.h>

Php::Value btp_configure(Php::Parameters&);
Php::Value btp_create_meter(Php::Parameters&);
Php::Value btp_release_meter(Php::Parameters&);
Php::Value btp_create_meter2(Php::Parameters&);
Php::Value btp_release_meter2(Php::Parameters&);
Php::Value btp_pushout(Php::Parameters&);

Php::Value  btp_add_time(Php::Parameters&);
Php::Value  btp_add_size(Php::Parameters&);

#pragma once

#include <phpcpp/visibility.h>
#include <phpcpp.h>

Php::Value btp_configure(Php::Parameters&);
Php::Value btp_create_meter(Php::Parameters&);
Php::Value btp_release_meter(Php::Parameters&);
Php::Value btp_pushout(Php::Parameters&);


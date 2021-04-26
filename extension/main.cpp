#include <stdexcept>
#include <phpcpp.h>
#include <iostream>
#include <fstream>
#include "phpbtp-client.hpp"
/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {
    
    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("btp-client", "1.2");

        extension.add<btp_configure>("btp_configure", {});
        extension.add<btp_create_meter>("btp_create_meter", {});
        extension.add<btp_release_meter>("btp_release_meter", {});
        extension.add<btp_create_meter2>("btp_create_meter2", {});
        extension.add<btp_release_meter2>("btp_release_meter2", {});
        extension.add<btp_pushout>("btp_pushout", {});
        extension.add<btp_add_time>("btp_add_time", {});
        extension.add<btp_add_size>("btp_add_size", {});
        // return the extension
        return extension;
    }
}

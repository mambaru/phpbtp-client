#include <phpcpp.h>
#include "btp_push.hpp"
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
        static Php::Extension extension("btp-pusher", "1.0");
        
        // @todo    add your own functions, classes, namespaces to the extension
        extension.add<btp_config_server_pool>("btp_config_server_pool", {});
        extension.add<btp_script_name_set>("btp_script_name_set", {});        
   
        extension.add<btp_config_server_set>("btp_config_server_set", {});
        extension.add<btp_dump>("btp_dump", {});
        extension.add<btp_dump_timer>("btp_dump_timer", {});
        extension.add<btp_project_name_set>("btp_project_name_set", {});
        extension.add<btp_timer_start>("btp_timer_start", {});
        extension.add<btp_timer_stop>("btp_timer_stop", {});
        extension.add<btp_flush>("btp_flush", {});
        extension.add<btp_timer_count>("btp_timer_count", {});
        extension.add<btp_timer_count_script>("btp_timer_count_script", {});
        extension.add<btp_timer_set_operation>("btp_timer_set_operation", {});
        
        
        
        // return the extension
        return extension;
    }
}

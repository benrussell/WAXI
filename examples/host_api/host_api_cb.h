


#include <cstdint>

extern "C" {

    #define CB_IMPORT(ret_type, name, ...) \
        __attribute__((import_module("cb"))) \
        __attribute__((import_name(#name))) \
        ret_type cb_##name(__VA_ARGS__);

    //CMD_IMPORT(int64_t, find, const char*)    
    CB_IMPORT(int64_t, reg, int32_t)
    CB_IMPORT(void, unreg, int64_t)
    
}

#undef CB_IMPORT
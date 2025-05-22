


#include <cstdint>

extern "C" {

    #define LOG_IMPORT(ret_type, name, ...) \
        __attribute__((import_module("log"))) \
        __attribute__((import_name(#name))) \
        ret_type log_##name(__VA_ARGS__);

    LOG_IMPORT(int64_t, raw, const char*)    
    
}

#undef LOG_IMPORT
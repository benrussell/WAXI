


#include <cstdint>

extern "C" {

    #define CMD_IMPORT(ret_type, name, ...) \
        __attribute__((import_module("cmd"))) \
        __attribute__((import_name(#name))) \
        ret_type cmd_##name(__VA_ARGS__);

    CMD_IMPORT(int64_t, find, const char*)    
    CMD_IMPORT(void, begin, int32_t)
    CMD_IMPORT(void, end, int32_t)
    CMD_IMPORT(void, once, int32_t)
    
}

#undef CMD_IMPORT
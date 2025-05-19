


#include <cstdint>

extern "C" {

    #define DREF_IMPORT(ret_type, name, ...) \
        __attribute__((import_module("dref"))) \
        __attribute__((import_name(#name))) \
        ret_type dref_##name(__VA_ARGS__);

    DREF_IMPORT(int32_t, newBytes, char*, int32_t)    
    DREF_IMPORT(int32_t, newDouble, char*)
    DREF_IMPORT(int32_t, newFloat, char*)
    DREF_IMPORT(int32_t, newFloatArr, char*, int32_t)
    DREF_IMPORT(int32_t, newInt, char*)
    DREF_IMPORT(int32_t, newIntArr, char*, int32_t)
        
    DREF_IMPORT(int64_t, find, const char*)
    
    DREF_IMPORT(float, getFloat, int32_t)
    DREF_IMPORT(void, setFloat, int32_t, float)

    DREF_IMPORT(int32_t, getInt, int32_t)
    DREF_IMPORT(void, setInt, int32_t, int32_t)

}

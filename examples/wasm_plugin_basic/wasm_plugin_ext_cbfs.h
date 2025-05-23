#ifndef WASM_PLUGIN_EXT_CBFS_H
#define WASM_PLUGIN_EXT_CBFS_H

#include <cstring>
#include <iostream>
#include <cstdio>

#include "host_api.h"




struct TestStructure{
    std::string snafu;
    std::string fubar;
};





void test_cbf(){
    printf("wasm/ test_cbf() exec!\n");
}

void test_cbf2(){
    printf("wasm/ test_cbf2() exec!&******************\n");
}



void test_cbfs(){

    TestStructure s; //FIXME: rename this struct def
    s.snafu = "situ normal";
    s.fubar = "fouled up";
    printf("wasm/ stack ptr for some_str instance: %p\n", &s);

    TestStructure* ptr_s = new TestStructure();
    printf("wasm/ heap ptr for some_str instance: %p\n", ptr_s);
    delete( ptr_s );
    
    
    printf("wasm/ calling cb_reg cbf2..\n");
    cb_reg( (int32_t) &test_cbf2 );

}

#endif


// Bare bones test harness

#include <cstring>
#include <iostream>
#include <cstdio>


#include "wasm_plugin_basic.h"


#include "host_api.h"


// optional extras to test parts of the WASM host api
#include "wasm_plugin_ext_cbfs.h"
#include "wasm_plugin_ext_cmd.h"
#include "wasm_plugin_ext_dref.h"
#include "wasm_plugin_ext_vfs.h"





// this is used to track how many times plugin_message has been called. debug hack.
static uint64_t msg_counter = 0;



int main(void){
    //printf("wasm/ main() is required by WASI _start()\n");
    return 0;
}



int plugin_start(char* outName, char* outSig, char* outDesc) {
    #if 0
    // printf("wasm/ plugin_start: [%p], [%p], [%p]\n", outName, outSig, outDesc );
    // printf("wasm/ plugin_start: [%s], [%s], [%s]\n", outName, outSig, outDesc );
    //fflush( stdout );
    #endif

    #if 0
    printf("wasm/ plugin_start(); calling std::cout..\n");
    // this crashes if WASI has not been init correctly.
    // host must call _start() which calls __wasi_ctors() 
    // CPP io stream test
    std::cout << "wasm/   std::cout works.\n";
    std::flush( std::cout );
    #endif

    snprintf(outName, 256, "Basic WASM Plugin Example");
    snprintf(outSig,  256, "wasm_xpl/examples/wasm_plugin_basic");
    snprintf(outDesc, 256, "Bare min SDK functionality test.");

    
    log_raw("wasm/ plugin_start()\n");


    // ret 1 to start, 0 to refuse.
    return 1;
}


// We use this to proxy into the WASM flat memory block.
void plugin_flcb_proxy( int32_t fptr ){
    printf("wasm/ plugin_flcb_proxy() fptr: %i\n", fptr);
    ((void(*)())fptr)();
}


void plugin_stop(){
    printf("wasm/ plugin_stop()\n");
    
}


int plugin_enable(){
    printf("wasm/ plugin_enable() msg_counter: %llu\n", msg_counter);

    test_drefs();

    test_cmds();

    test_cbfs();
    
    test_vfs();

    return msg_counter;
}


void plugin_disable(){
    printf("wasm/ plugin_disable()\n");
}


void plugin_message( int64_t inFromWho, int64_t inMessage, int32_t inParam ){
    
    printf("wasm/ plugin_message: from: %lld, msg: %lld, param: %i\n", inFromWho, inMessage, inParam);
    
    ++msg_counter;
}


//
// Created by br on 1/8/24.
//


// Bare bones test harness for calling wanted sigs in WASM VM

#include <cstring>
#include <iostream>
#include <cstdio>
#include <dirent.h>


#include "wasm_plugin_basic.h"

#include "host_api.h"







void test_cbf(){
    printf("wasm/ test_cbf() exec!\n");
}

void test_cbf2(){
    printf("wasm/ test_cbf2() exec!&******************\n");
}


void test_cmds(){
    printf("wasm/ test_cmds()\n");
    auto cmdh_b = cmd_find("cmd/b");
    auto cmdh_c = cmd_find("cmd/c");
    cmd_begin(cmdh_c);
    cmd_end(cmdh_c);
    cmd_once(cmdh_b);
}

void test_drefs(){
    printf("wasm/ test_drefs()\n");

    {
        int32_t drh = dref_find("dref/a");
        float val_before = dref_getFloat( drh );
        dref_setFloat( drh, 1.23 );
        float val_after = dref_getFloat( drh );
        printf("wasm/   dref_getFloat(a) before: %f\n", val_before);
        printf("wasm/   dref_getFloat(a) after: %f\n", val_after);
    }

    {
        int32_t drh = dref_find("dref/b");
        float val_before = dref_getFloat( drh );
        dref_setFloat( drh, 9.23 );
        float val_after = dref_getFloat( drh );
        printf("wasm/   dref_getFloat(b) before: %f\n", val_before);
        printf("wasm/   dref_getFloat(b) after: %f\n", val_after);
    }

    int32_t drh = dref_find("dref/_invalid");

}






int main(void){
    printf("wasm/ main() is required by WASI _start()\n");
    return 0;
}



int plugin_start(char* outName, char* outSig, char* outDesc) {
    // printf("wasm/ plugin_start: [%p], [%p], [%p]\n", outName, outSig, outDesc );
    // printf("wasm/ plugin_start: [%s], [%s], [%s]\n", outName, outSig, outDesc );
    //fflush( stdout );

    printf("wasm/ plugin_start()... about to call std::cout <<\n");
    // FIXME: this crashes if WASI has not been init correctly.
    // host must call _start() which calls __wasi_ctors() 
    // CPP io stream test
    std::cout << "wasm/ CPP plugin_start\n";
    std::flush( std::cout );

    snprintf(outName, 256, "Basic WASM Plugin Example");
    snprintf(outSig,  256, "wasm_xpl/examples/wasm_plugin_basic");
    snprintf(outDesc, 256, "Bare min SDK functionality test.");

    printf("wasm/ Hello World! plugin_start() is working.\n");
    
    log_raw("wasm/ --- this is a call to XPLMDebugString from wasm byte code ---\n");


    some_str s;
    s.snafu = "situ normal";
    s.fubar = "fouled up";
    printf("wasm/ stack ptr for some_str instance: %p\n", &s);

    some_str* ptr_s = new some_str();
    printf("wasm/ heap ptr for some_str instance: %p\n", ptr_s);
    delete( ptr_s );
    
    
    printf("wasm/ calling cb_reg cbf2..\n");
    cb_reg( (int32_t) &test_cbf2 );
    

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

    //test_drefs();

    //test_cmds();

    
    
    // List all files in the current folder using filesystem APIs
    printf("wasm/ Listing files in the current folder:\n");

    DIR* dir = opendir(".");
    if (dir == nullptr) {
        perror("opendir");
        return msg_counter;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        printf("wasm/  %s\n", entry->d_name);
    }

    closedir(dir);
    


    return msg_counter;
}


void plugin_disable(){
    printf("wasm/ plugin_disable()\n");
}




void plugin_message( int64_t inFromWho, int64_t inMessage, int32_t inParam ){
    
    printf("wasm/ plugin_message: from: %lld, msg: %lld, param: %i\n", inFromWho, inMessage, inParam);
    
    ++msg_counter;
}




//
// Created by br on 1/8/24.
//


// Bare bones test harness for calling wanted sigs in WASM VM

#include <cstring>
#include <iostream>
#include <cstdio>
#include <dirent.h>


#include "host_api.h"
//#include "host_api_dref.h"


extern "C" {
    int plugin_start(char* outName, char* outSig, char* outDesc);
    void plugin_stop();

    int plugin_enable();
    void plugin_disable();

    void plugin_message( int64_t inFromWho, int64_t inMessage, int32_t inParam );


} //extern




// this is used to track how many times plugin_message has been called. debug hack.
static uint64_t msg_counter = 0;




void test_drefs(){
    printf("wasm/ Running basic dataref tests..\n");

    {
        int32_t drh = dref_find("dref/a");
        float val_before = dref_getFloat( drh );
        dref_setFloat( drh, 1.23 );
        float val_after = dref_getFloat( drh );
        printf("  wasm/ dref_getFloat(a) before: %f\n", val_before);
        printf("  wasm/ dref_getFloat(a) after: %f\n", val_after);
    }

    {
        int32_t drh = dref_find("dref/b");
        float val_before = dref_getFloat( drh );
        dref_setFloat( drh, 9.23 );
        float val_after = dref_getFloat( drh );
        printf("  wasm/ dref_getFloat(b) before: %f\n", val_before);
        printf("  wasm/ dref_getFloat(b) after: %f\n", val_after);
    }

    int32_t drh = dref_find("dref/_invalid");

}




int plugin_start(char* outName, char* outSig, char* outDesc) {

    // printf("wasm/ plugin_start: [%p], [%p], [%p]\n", outName, outSig, outDesc );
    // printf("wasm/ plugin_start: [%s], [%s], [%s]\n", outName, outSig, outDesc );
    //fflush( stdout );

    // FIXME: this crashes. :-(
    // CPP io stream test
    // std::cout << "wasm/ CPP plugin_start\n";
    // std::flush( std::cout );

    snprintf(outName, 256, "Basic WASM Plugin Example");
    snprintf(outSig,  256, "wasm_xpl/examples/wasm_plugin_basic");
    snprintf(outDesc, 256, "Bare min SDK functionality test.");

    printf("wasm/ Hello World! plugin_start() is working.\n");
    

    // ret 1 to start, 0 to refuse.
    return 1;
}



void plugin_stop(){
    printf("wasm/ plugin_stop()\n");
    
}


int plugin_enable(){
    printf("wasm/ plugin_enable() msg_counter: %llu\n", msg_counter);

    test_drefs();

    printf("wasm/ cmd test\n");
    auto cmdh_b = cmd_find("cmd/b");
    auto cmdh_c = cmd_find("cmd/c");
    cmd_begin(cmdh_c);
    cmd_end(cmdh_c);
    cmd_once(cmdh_b);
    

    // List all files in the current folder using filesystem APIs
    printf("wasm/ Listing files in the current folder:\n");

    DIR* dir = opendir(".");
    if (dir == nullptr) {
        perror("opendir");
        return msg_counter;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        printf("  %s\n", entry->d_name);
    }

    closedir(dir);
    




    return msg_counter;
}


void plugin_disable(){
    printf("wasm/ plugin_disable()\n");
}




void plugin_message( int64_t inFromWho, int64_t inMessage, int32_t inParam ){
    
    printf("plugin_message: from: %lld, msg: %lld, param: %i\n", inFromWho, inMessage, inParam);
    
    ++msg_counter;
}




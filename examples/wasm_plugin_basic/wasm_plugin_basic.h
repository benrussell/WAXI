//
// Created by br on 1/8/24.
//


// Bare bones test harness for calling wanted sigs in WASM VM

// #include <cstring>
// #include <iostream>
// #include <cstdio>
// #include <dirent.h>

#include "host_api.h"


extern "C" {
    //int main(void); //does it need an export?

    int plugin_start(char* outName, char* outSig, char* outDesc);
    void plugin_stop();

    int plugin_enable();
    void plugin_disable();

    void plugin_message( int64_t inFromWho, int64_t inMessage, int32_t inParam );


    //it may be possible to replace this with fn resolution at the host side.
    void plugin_flcb_proxy( int32_t fptr );


} //extern


int main(void);




// this is used to track how many times plugin_message has been called. debug hack.
static uint64_t msg_counter = 0;


void test_cbf();
void test_cbf2();

void test_cmds();

void test_drefs();


struct some_str{
    std::string snafu;
    std::string fubar;
};








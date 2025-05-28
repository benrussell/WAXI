#include <iostream>

#include "WasmVM.h"


int main(int argc, const char* argv[]) {
    

    std::cout << "WASM boot: wasm_plugin_basic\n";
    {

        char outName[256];
        char outSig[256];
        char outDesc[256];

        std::string path = "../examples/wasm_plugin_basic/build/wasm_plugin_basic.wasm";
        WasmVM vm_husk{path};
        if (vm_husk.call_plugin_start( outName, outSig, outDesc ) ){

            vm_husk.call_plugin_enable();
            std::cout << "> wasm_host/ sending messages..\n";
                for( int x=0; x<10; ++x){
                    vm_husk.call_plugin_message( 0, 1, x );
                }
            vm_husk.call_plugin_disable();
            
            vm_husk.call_plugin_stop();

        }
    }

    return 0;

}
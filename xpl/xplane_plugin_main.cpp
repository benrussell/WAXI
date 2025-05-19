
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"

#include "WasmVM.h"


WasmVM* global_WasmVM;


// Plugin start function
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc) {

    //FIXME: enable native paths
    //FIXME: enable other good stuff?

    //FIXME: determine path to loaded plugin so that we can load from its resource bundle

    std::string filename = "lame.wasm";
    global_WasmVM = new WasmVM( filename );

    
    //FIXME: these should be able to be populated by the WASM module.
    strncpy(outName, "Wasm Plugin", 255);
    strncpy(outSig, "com.example.wasmplugin", 255);
    strncpy(outDesc, "A plugin that runs WebAssembly modules.", 255);

    // The outName, outSig, and outDesc buffers are guaranteed to be at least 256 bytes each.
    // Reference: X-Plane SDK documentation.

    //int wasm_ret = global_WasmVM->call_plugin_start( outName, outSig, outDesc );
    int wasm_ret = global_WasmVM->call_plugin_start();


    return wasm_ret;
}


// Plugin stop function
PLUGIN_API void XPluginStop(void) {
    // Cleanup code here

    global_WasmVM->call_plugin_stop();

    delete global_WasmVM;
}


// Plugin enable function
PLUGIN_API int XPluginEnable(void) {
    // Code to enable the plugin
    return global_WasmVM->call_plugin_enable();;
}


// Plugin disable function
PLUGIN_API void XPluginDisable(void) {
    // Code to disable the plugin
    global_WasmVM->call_plugin_disable();
}


// Plugin receive message function
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, int inMessage, void *inParam) {
    // Handle messages here

    global_WasmVM->call_plugin_message(inFromWho, inMessage, reinterpret_cast<intptr_t>(inParam));

}



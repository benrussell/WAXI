
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"

#include "WasmVM.h"


WasmVM* global_WasmVM;


// Plugin start function
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc) {

    // XPLMEnableFeature("XPLM_WANTS_REFLECTIONS", 1);
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
    XPLMEnableFeature("XPLM_USE_NATIVE_WIDGET_WINDOWS", 1);
    //XPLMEnableFeature("XPLM_WANTS_DATAREF_NOTIFICATIONS", 1);
    
    
    char path[1024];
    XPLMGetPluginInfo(XPLMGetMyID(), nullptr, path, nullptr, nullptr);

    // Extract the folder path from the full path
    std::string fullPath(path);
    std::string caFolderName = fullPath.substr(0, fullPath.find_last_of("/\\"));

    // Modify the folder path to point to the resources directory
    std::string resourcesPath = caFolderName + "/resources/";

    
    //FIXME: determine path to loaded plugin so that we can load from its resource bundle

    std::string filename = "lame.wasm";
    global_WasmVM = new WasmVM( filename );

    // These values should be over-written by the call to plugin_start()
    strncpy(outName, "Wasm Plugin", 255);
    strncpy(outSig, "com.example.wasmplugin", 255);
    strncpy(outDesc, "A plugin that runs WebAssembly modules.", 255);

    // The outName, outSig, and outDesc buffers are guaranteed to be at least 256 bytes each.
    // Reference: X-Plane SDK documentation.

    int wasm_ret = global_WasmVM->call_plugin_start( outName, outSig, outDesc );
    

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



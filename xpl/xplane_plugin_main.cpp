
#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"

#include "WasmVM.h"


WasmVM* global_WasmVM;






// Flight loop callback function
float CustomFlightLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon) {
    // Custom logic for the flight loop callback
    //std::cout << "CustomFlightLoopCallback\n";
    std::cout << "CustomFlightLoopCallback called. Elapsed time: " << inElapsedSinceLastCall << " seconds.\n";

    // Example: Call a function in the WasmVM
    if (global_WasmVM) {
        //global_WasmVM->call_flight_loop(inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter);
    }

    // Return the interval in seconds for the next callback
    return 0.1f; // Call again in 1 second
}


// Register the flight loop callback
void RegisterFlightLoopCallback() {
    std::cout << "wasm_xpl/ registering flcb\n";
    //XPLMRegisterFlightLoopCallback(CustomFlightLoopCallback, 1.0f, nullptr); // Initial interval of 1 second


    // Output the pointer address of CustomFlightLoopCallback as a decimal value
    printf("wasm_xpl/ flcb ptr: %p\n", &CustomFlightLoopCallback);

    // Create a flight loop structure
    XPLMCreateFlightLoop_t flightLoopParams;
    flightLoopParams.structSize = sizeof(XPLMCreateFlightLoop_t);
    flightLoopParams.refcon = nullptr;
    flightLoopParams.callbackFunc = CustomFlightLoopCallback;
    flightLoopParams.phase = xplm_FlightLoop_Phase_BeforeFlightModel;
    //flightLoopParams.phase = xplm_FlightLoop_Phase_AfterFlightModel;

    printf("wasm_xpl handoff: fl_param.callbackFunc: %p\n", flightLoopParams.callbackFunc);

    printf(" handoff stackptr for flcb_params: %p\n", &flightLoopParams);

    // Create the flight loop
    XPLMFlightLoopID flightLoopID = XPLMCreateFlightLoop(&flightLoopParams);


    // Schedule the flight loop to run
    XPLMScheduleFlightLoop(
        flightLoopID, 
        0.01f, 
        1
    );


}

// Unregister the flight loop callback
void UnregisterFlightLoopCallback() {
    //XPLMUnregisterFlightLoopCallback(CustomFlightLoopCallback, nullptr);
}








// Plugin start function
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc) {

    // XPLMEnableFeature("XPLM_WANTS_REFLECTIONS", 1);
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
    XPLMEnableFeature("XPLM_USE_NATIVE_WIDGET_WINDOWS", 1);
    //XPLMEnableFeature("XPLM_WANTS_DATAREF_NOTIFICATIONS", 1);
    
    // Ensure the callback is registered during plugin initialization
    RegisterFlightLoopCallback();


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



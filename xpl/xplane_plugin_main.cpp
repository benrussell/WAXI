
#include <XPLMPlugin.h>
#include <XPLMProcessing.h>
#include <XPLMUtilities.h>
#include <XPLMPlanes.h>

#include "WasmVM.h"
#include "WasmVM_Config.h"



WasmVM* global_WasmVM;


#include <nlohmann/json.hpp>


#if 0
// Flight loop callback function
float CustomFlightLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon) {
    // Custom logic for the flight loop callback
    //std::cout << "CustomFlightLoopCallback\n";
    //std::cout << "CustomFlightLoopCallback called. Elapsed time: " << inElapsedSinceLastCall << " seconds.\n";

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

    printf("wasm_xpl/ handoff: fl_param.callbackFunc: %p\n", flightLoopParams.callbackFunc);

    printf("wasm_xpl/ handoff stackptr for flcb_params: %p\n", &flightLoopParams);

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
#endif


void resolve_paths( WasmVM_Config &config ){

    // Need to determine:
    // - {plugin_root} - done    
    {
        char path[1024]{};
        XPLMGetPluginInfo(XPLMGetMyID(), nullptr, path, nullptr, nullptr);
        //std::cout << "wasm_xpl/ plugin_path: ["<< path <<"]\n";

        // Extract the folder path from the full path
        std::string fullPath(path);
        std::string pluginFolderName = fullPath.substr(0, fullPath.find_last_of("/\\")); //strip the plugin filename
        pluginFolderName = pluginFolderName.substr(0, pluginFolderName.find_last_of("/\\")); //strip the os_64 leaf

        config.plugin_folder = pluginFolderName;
        std::cout << "wasm_xpl/ plugin_folder: [" << config.plugin_folder << "]\n";
    }

    // - {xp_root}
    {
        char xpFolder[1024]{};
        XPLMGetSystemPath(xpFolder);
        std::string xpRootPath(xpFolder);
        
        config.xp_folder = xpRootPath;
        std::cout << "wasm_xpl/ xp_folder: [" << config.xp_folder << "]\n";
    }
    
    // - {acf_root}
    {
        char acfFilename[1024]{};
        char acfFolder[1024]{};
        XPLMGetNthAircraftModel(0, acfFilename, acfFolder); // Get the path of the user's aircraft
        //std::cout << "wasm_xpl/ Aircraft full path: [" << acfFilename << "]\n";
        //std::cout << "wasm_xpl/ Aircraft API folder path: [" << acfFolder << "]\n";
        
        config.acf_folder = acfFolder;
        std::cout << "wasm_xpl/ acf_folder: [" << config.acf_folder << "]\n";
    }

}






// Plugin start function
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc) {

    // XPLMEnableFeature("XPLM_WANTS_REFLECTIONS", 1);
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
    XPLMEnableFeature("XPLM_USE_NATIVE_WIDGET_WINDOWS", 1);
    //XPLMEnableFeature("XPLM_WANTS_DATAREF_NOTIFICATIONS", 1);


    // Ensure the callback is registered during plugin initialization
    // RegisterFlightLoopCallback();


    WasmVM_Config config;
    resolve_paths( config );



    std::string wasm_filename;

    std::string config_json_filename = config.plugin_folder + "/config.json";
    std::cout << "wasm_xpl/ dyn config json fn: " + config_json_filename + "\n";

    // Load configuration from config.json
    std::ifstream configFile(config_json_filename);
    if (configFile.is_open()) {
        try {
            nlohmann::json jsonConfig;
            configFile >> jsonConfig;


            // // Example: Parse some configuration values
            // if (jsonConfig.contains("plugin_folder")) {
            //     config.plugin_folder = jsonConfig["plugin_folder"].get<std::string>();
            // }
            // if (jsonConfig.contains("xp_folder")) {
            //     config.xp_folder = jsonConfig["xp_folder"].get<std::string>();
            // }
            // if (jsonConfig.contains("acf_folder")) {
            //     config.acf_folder = jsonConfig["acf_folder"].get<std::string>();
            // }

            
            if (jsonConfig.contains("wasm_filename")) {
                wasm_filename = jsonConfig["wasm_filename"].get<std::string>();
                
            }

            


            std::cout << "wasm_xpl/ Loaded configuration from config.json\n";
        } catch (const std::exception &e) {
            std::cerr << "wasm_xpl/ Error parsing config.json: " << e.what() << "\n";
        }
    } else {
        std::cerr << "wasm_xpl/ Could not open config.json\n";
    }




    //FIXME: wasm filename? config.json entry?
    //std::string filename = acfFolderPath + "/wasm_plugin_basic.wasm";
    // std::string wasm_filename = "/home/br/Dev/wasm/wasm_plugin/wasm_plugin_basic/build/wasm_plugin_basic.wasm";
    std::cout << "wasm_xpl/ wasm filename:[" << wasm_filename << "]\n";
    global_WasmVM = new WasmVM( wasm_filename, config );


    // These values should be over-written by the call to plugin_start()
    strncpy(outName, "WASM Plugin Shim", 255);
    strncpy(outSig, "x-plugins.com/wasm_xpl", 255);
    strncpy(outDesc, "WASM for X-Plane", 255);

    // The outName, outSig, and outDesc buffers are guaranteed to be at least 256 bytes each.
    // Reference: X-Plane SDK documentation.

    //XPLMDebugString("wasm_xpl/ Calling into wasm start...\n");
    int wasm_ret = global_WasmVM->call_plugin_start( outName, outSig, outDesc );
    
    
    XPLMDebugString("wasm_xpl/ Plugin started...\n");

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



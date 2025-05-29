

#include <fstream>
#include <iostream>
#include <sstream>
#include <wasmtime.hh>
#include <cstring>
#include <iomanip>

// #include "WasmVM.h"

#include "LinkerHelpers.h"



#define USE_XP_SDK 1
#if USE_XP_SDK
#include <XPLMUtilities.h>
#endif



using namespace wasmtime;

namespace xp_api
{
    class log
    {
    public:
        static int64_t raw(Caller caller, char *c_str)
        {
            //std::cout << ">> api/ log_raw(" << c_str << ")" << std::endl;

            #if USE_XP_SDK
                XPLMDebugString( c_str );
            #endif
            
            return 0; //FIXME: being lazy with binding helper
        }
        
    };
}
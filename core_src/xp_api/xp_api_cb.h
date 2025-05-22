

#include <fstream>
#include <iostream>
#include <sstream>
#include <wasmtime.hh>
#include <cstring>
#include <iomanip>

// #include "WasmVM.h"

#include "LinkerHelpers.h"

using namespace wasmtime;

namespace xp_api
{
    class cb
    {
    public:
        static int32_t glob_cbf_ptr;
        
        static int64_t reg(Caller caller, int32_t cbf_ptr)
        {
            std::cout << ">> api/ cb_reg(" << cbf_ptr << ")" << std::endl;
            //std::cout << ">> api/ cb_reg(..) needs to ret cb_handle" << std::endl;
            glob_cbf_ptr = cbf_ptr;

            return 0;
        }

        static void unreg(Caller caller, int64_t cb_handle)
        {
            std::cout << ">> api/ cb_unreg(" << cb_handle << ")" << std::endl;
        }

        static void set_schedule(Caller caller, int64_t cb_handle, float interval)
        {
            std::cout << ">> api/ cb_set_schedule(" << cb_handle << "," << interval << ")" << std::endl;
        }
        
    };
}

int32_t xp_api::cb::glob_cbf_ptr = 0;
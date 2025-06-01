#ifndef LINKER_HELPERS_H
#define LINKER_HELPERS_H


#include <fstream>
#include <iostream>
#include <sstream>
#include <wasmtime.hh>
#include <cstring>
#include <iomanip>


#include "nanovg.h"


using namespace wasmtime;





class LinkHelp{
public:



     // Lambda to wrap a C++ function and expose it to WASM
    static auto wrap(
		wasmtime::Linker &linker, 
		auto cpp_fn, 
		const std::string &mod, 
		const std::string &name) {
        return linker.func_wrap(mod, name, cpp_fn);
    };


  // Helper to wrap and expose functions with (Caller, char*) signature
    static auto wrap_and_expose_caller_charptr(
            wasmtime::Linker &linker, 
            const std::string &mod,
            const std::string &name,
            int64_t(*fn)(Caller, char*) //signature that we need to wrap
        ) {
        return linker.func_wrap(
            mod, name,
            // We're using a lambda closure here to augment the functionality.
            //ptrs into wasm memory space are only 32bit
            [fn](Caller caller, int32_t ptr) -> int64_t {
                // Get memory from the caller
                auto memory_export = caller.get_export("memory");
                if (!memory_export || !std::holds_alternative<wasmtime::Memory>(*memory_export)) {
                    std::cerr << "Memory not found in caller" << std::endl;
                    return -1;
                }
                auto memory = &std::get<wasmtime::Memory>(*memory_export);


                // Read the string from WASM memory
                auto mem_data = memory->data(caller);
                
                //calc the size of the c_str we're being passed.
                size_t len = 0;
                while ((ptr + len) < mem_data.size() && mem_data[ptr + len] != '\0') {
                    ++len;
                }

                std::vector<char> buf(len + 1, 0);
                std::memcpy(buf.data(), mem_data.data() + ptr, len);

                // Call the actual function
                return fn(caller, buf.data());
            }
        );
    }








// Helper to wrap and expose functions with (Caller, char*) signature
    static auto wrap_nvg_proxy_renderFill(
            wasmtime::Linker &linker, 
            const std::string &mod,
            const std::string &name,
            void(*fn_plain)(
                uint64_t uptr,
                uint32_t paint_wptr,
                NVGcompositeOperationState comp,
                uint32_t scissor_wptr,
                float fringe,
                const uint32_t bounds_wptr,
                const uint32_t paths_wptr,
                int npaths            
            ) //signature that we need to wrap
        ) {
        return linker.func_wrap(
            mod, name,
            // We're using a lambda closure here to augment the functionality.
            //ptrs into wasm memory space are only 32bit
            [fn_plain](
                Caller caller, 
                uint64_t uptr,
                uint32_t paint_wptr,
                //NVGcompositeOperationState comp, - destrctured into four ints by WASM
                    int32_t comp_a,
                    int32_t comp_b,
                    int32_t comp_c,
                    int32_t comp_d,
                uint32_t scissor_wptr,
                float fringe,
                const uint32_t bounds_wptr,
                const uint32_t paths_wptr,
                int npaths            

            ) -> void {


#if 0
                // Get memory from the caller
                auto memory_export = caller.get_export("memory");
                if (!memory_export || !std::holds_alternative<wasmtime::Memory>(*memory_export)) {
                    std::cerr << "Memory not found in caller" << std::endl;
                    return;
                }
                auto memory = &std::get<wasmtime::Memory>(*memory_export);


                // Read the string from WASM memory
                auto mem_data = memory->data(caller);
                
                //calc the size of the c_str we're being passed.
                size_t len = 0;
                while ((ptr + len) < mem_data.size() && mem_data[ptr + len] != '\0') {
                    ++len;
                }

                std::vector<char> buf(len + 1, 0);
                std::memcpy(buf.data(), mem_data.data() + ptr, len);
#endif


                // re-construct the struct before we call our underlying code
                NVGcompositeOperationState comp { comp_a, comp_b, comp_c, comp_d };

                // Call the actual function
                fn_plain(
                    uptr,
                    paint_wptr, 
                    comp,
                    scissor_wptr,
                    fringe,
                    bounds_wptr,
                    paths_wptr,
                    npaths                
                );
            }
        );
    }





};


#endif
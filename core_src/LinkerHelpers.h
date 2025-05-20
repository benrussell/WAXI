#ifndef LINKER_HELPERS_H
#define LINKER_HELPERS_H


#include <fstream>
#include <iostream>
#include <sstream>
#include <wasmtime.hh>
#include <cstring>
#include <iomanip>


using namespace wasmtime;





class LinkerHelpers{
public:



     // Lambda to wrap a C++ function and expose it to WASM
    static auto wrap_and_expose(
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



};


#endif
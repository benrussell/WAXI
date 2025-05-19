#include <fstream>
#include <iostream>
#include <sstream>
#include <wasmtime.hh>
#include <cstring>
#include <iomanip>



using namespace wasmtime;







class XP_API{
    
private:
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



public:

    static void init(wasmtime::Linker &linker, wasmtime::Store *store){
        
        // provide some host fns
		
        // Lambda to wrap a C++ function and expose it to WASM
        auto wrap_and_expose = [&](auto cpp_fn, const std::string &mod, const std::string &name) {
            return linker.func_wrap(mod, name, cpp_fn);
        };



        // Expose host_test
        auto res1 = wrap_and_expose(XP_API::host_test, "host", "host_test");
        // Expose host_foo
        auto res2 = wrap_and_expose(XP_API::host_foo, "host", "host_foo");


        auto res3 = wrap_and_expose_caller_charptr(linker, "dref", "find", XP_API::dref_find);
        auto res4 = wrap_and_expose(XP_API::dref_getFloat, "dref", "getFloat");
        auto res5 = wrap_and_expose(XP_API::dref_setFloat, "dref", "setFloat");


    }




    // Define a host function
    static void host_test(Caller caller, int32_t arg, int32_t arg2, int32_t arg3) {
        std::cout << ">> api/ host_test() arg1: " << arg 
              << ", arg2: " << arg2 
              << ", arg3: " << arg3 << std::endl;
    }

    static int host_foo(Caller caller, int32_t arg) {
        std::cout << ">> api/ host_foo(" << arg << ")" << std::endl;
        return arg - 1;
    }


      

    static int64_t dref_find(Caller caller, char* dref_name ) {
        std::cout << ">> api/ dref_find(" << dref_name << ")" << std::endl;

        // Example array of three char* values
        static const char* dref_names[] = { "dref/a", "dref/b", "dref/c" };

        for (int i = 0; i < 3; ++i) {
            if (std::strcmp(dref_names[i], dref_name) == 0) {
            return i;
            }
        }

        std::cerr << " * Invalid dref name: " << dref_name << std::endl;
        return -1;
    }

    static float m_dr_values[3];

    static float dref_getFloat(Caller caller, int32_t drh ) {
        //std::cout << ">> api/ dref_getFloat(" << drh << ")" << std::endl;
        if( drh >= 0 ){
            return m_dr_values[drh];
        }
        return 0.f;
    }

    static void dref_setFloat(Caller caller, int32_t drh, float value ) {
        //std::cout << ">> api/ dref_setFloat(" << drh << ", " << value << ")" << std::endl;
        if( drh >= 0 ){
            m_dr_values[drh] = value;
        }
        else {
            std::cerr << "Invalid drh" << std::endl;
        }
    }


};


float XP_API::m_dr_values[3];
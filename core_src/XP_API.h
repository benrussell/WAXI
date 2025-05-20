#ifndef XP_API_H
#define XP_API_H


#include <fstream>
#include <iostream>
#include <sstream>
#include <wasmtime.hh>
#include <cstring>
#include <iomanip>

//#include "WasmVM.h"

#include "LinkerHelpers.h"


using namespace wasmtime;






class XP_API{
    
private:



public:


    static void init(wasmtime::Linker &linker, wasmtime::Store *store){
        
        // provide some host fns
	
        // // Expose host_test
        // auto res1 = wrap_and_expose(XP_API::host_test, "host", "host_test");
        // // Expose host_foo
        // auto res2 = wrap_and_expose(XP_API::host_foo, "host", "host_foo");

        std::cout << ">> api/ export dref module" << std::endl;
        auto res3 = LinkerHelpers::wrap_and_expose_caller_charptr(linker, "dref", "find", XP_API::dref_find);
        auto res4 = LinkerHelpers::wrap_and_expose(linker, XP_API::dref_getFloat, "dref", "getFloat");
        auto res5 = LinkerHelpers::wrap_and_expose(linker, XP_API::dref_setFloat, "dref", "setFloat");

        std::cout << ">> api/ export cmd module" << std::endl;
        auto res6 = LinkerHelpers::wrap_and_expose_caller_charptr(linker, "cmd", "find", XP_API::cmd_find);
        auto res7 = LinkerHelpers::wrap_and_expose(linker, XP_API::cmd_begin, "cmd", "begin");
        auto res8 = LinkerHelpers::wrap_and_expose(linker, XP_API::cmd_end, "cmd", "end");
        auto res9 = LinkerHelpers::wrap_and_expose(linker, XP_API::cmd_once, "cmd", "once");

    }




    // // Define a host function
    // static void host_test(Caller caller, int32_t arg, int32_t arg2, int32_t arg3) {
    //     std::cout << ">> api/ host_test() arg1: " << arg 
    //           << ", arg2: " << arg2 
    //           << ", arg3: " << arg3 << std::endl;
    // }

    // static int host_foo(Caller caller, int32_t arg) {
    //     std::cout << ">> api/ host_foo(" << arg << ")" << std::endl;
    //     return arg - 1;
    // }



    static int64_t cmd_find(Caller caller, char* cmd_name ) {
        std::cout << ">> api/ cmd_find(" << cmd_name << ")" << std::endl;

        // Example array of three char* values
        static const char* cmd_names[] = { "cmd/a", "cmd/b", "cmd/c" };

        for (int i = 0; i < 3; ++i) {
            if (std::strcmp(cmd_names[i], cmd_name) == 0) {
            return i;
            }
        }

        std::cerr << " * Invalid cmd name: " << cmd_name << std::endl;
        return -1;
    }


    static void cmd_begin(Caller caller, int32_t cmdh ) {
        std::cout << ">> api/ cmd_begin(" << cmdh << ")" << std::endl;
    }


    static void cmd_end(Caller caller, int32_t cmdh ) {
        std::cout << ">> api/ cmd_end(" << cmdh << ")" << std::endl;
    }


    static void cmd_once(Caller caller, int32_t cmdh ) {
        std::cout << ">> api/ cmd_once(" << cmdh << ")" << std::endl;
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







#endif
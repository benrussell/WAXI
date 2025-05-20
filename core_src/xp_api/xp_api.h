#ifndef XP_API_H
#define XP_API_H


#include <fstream>
#include <iostream>
#include <sstream>
#include <wasmtime.hh>
#include <cstring>
#include <iomanip>


#include "LinkerHelpers.h" //FIXME: rename this file


using namespace wasmtime;



// all the actual code binding is in these files.
namespace xp_api{}
#include "xp_api_cmd.h"
#include "xp_api_dref.h"


class XP_API{ //FIXME: this needs a new name.

public:

    static void init(wasmtime::Linker &linker, wasmtime::Store *store){
        
        // provide some host fns
	
        std::cout << ">> api/ export dref module" << std::endl;
        auto res3 = LinkerHelpers::wrap_and_expose_caller_charptr(linker, "dref", "find", xp_api::dref::find);
        auto res4 = LinkerHelpers::wrap_and_expose(linker, xp_api::dref::getFloat, "dref", "getFloat");
        auto res5 = LinkerHelpers::wrap_and_expose(linker, xp_api::dref::setFloat, "dref", "setFloat");

        std::cout << ">> api/ export cmd module" << std::endl;
        auto res6 = LinkerHelpers::wrap_and_expose_caller_charptr(linker, "cmd", "find", xp_api::cmd::find);
        auto res7 = LinkerHelpers::wrap_and_expose(linker, xp_api::cmd::begin, "cmd", "begin");
        auto res8 = LinkerHelpers::wrap_and_expose(linker, xp_api::cmd::end, "cmd", "end");
        auto res9 = LinkerHelpers::wrap_and_expose(linker, xp_api::cmd::once, "cmd", "once");

    }

};


#endif
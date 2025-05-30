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
#include "xp_api_cb.h"
#include "xp_api_cmd.h"
#include "xp_api_dref.h"
#include "xp_api_log.h"

class XP_API{ //FIXME: this needs a new name.

public:

    static void init(wasmtime::Linker &linker, wasmtime::Store *store){
        
        // provide some host fns
	
        {
        // std::cout << ">> api/ export dref module" << std::endl;
        auto res3 = LinkerHelpers::wrap_and_expose_caller_charptr(linker, "dref", "find", xp_api::dref::find);
        auto res4 = LinkerHelpers::wrap_and_expose(linker, xp_api::dref::getFloat, "dref", "getFloat");
        auto res5 = LinkerHelpers::wrap_and_expose(linker, xp_api::dref::setFloat, "dref", "setFloat");
        }

        {
        // std::cout << ">> api/ export cmd module" << std::endl;
        auto res6 = LinkerHelpers::wrap_and_expose_caller_charptr(linker, "cmd", "find", xp_api::cmd::find);
        auto res7 = LinkerHelpers::wrap_and_expose(linker, xp_api::cmd::begin, "cmd", "begin");
        auto res8 = LinkerHelpers::wrap_and_expose(linker, xp_api::cmd::end, "cmd", "end");
        auto res9 = LinkerHelpers::wrap_and_expose(linker, xp_api::cmd::once, "cmd", "once");
        }

        {
        // std::cout << ">> api/ export log module" << std::endl;
        auto res10 = LinkerHelpers::wrap_and_expose_caller_charptr(linker, "log", "raw", xp_api::log::raw);
        }

        {
        // std::cout << ">> api/ export cb module" << std::endl;
        auto res11 = LinkerHelpers::wrap_and_expose(linker, xp_api::cb::reg, "cb", "reg");
        auto res12 = LinkerHelpers::wrap_and_expose(linker, xp_api::cb::unreg, "cb", "unreg");
        //auto res13 = LinkerHelpers::wrap_and_expose(linker, xp_api::cb::set_schedule, "cb", "set_schedule");
        }

        {
        auto res11 = LinkerHelpers::wrap_and_expose(linker, xp_api::cb::reg_draw, "cb", "reg_draw");
        auto res12 = LinkerHelpers::wrap_and_expose(linker, xp_api::cb::unreg_draw, "cb", "unreg_draw");
        }


        std::cout << "waxi/ Init WASM X-Plane Interface" << std::endl;

    }

};


#endif
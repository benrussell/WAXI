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
#include "xp_api_gfx.h"
#include "xp_api_log.h"

class XP_API{ //FIXME: this needs a new name.

public:

    static void init(wasmtime::Linker &linker, wasmtime::Store *store){
        
        std::cout << "waxi/ Init WASM X-Plane Interface" << std::endl;
        // provide some host fns
	
        {
        // std::cout << ">> api/ export dref module" << std::endl;
        auto res3 = LinkerHelpers::wrap_and_expose_caller_charptr(linker,               "waxi_dref@1", "find", xp_api::dref::find);
        auto res4 = LinkerHelpers::wrap_and_expose(linker, xp_api::dref::getFloat,      "waxi_dref@1", "getFloat");
        auto res5 = LinkerHelpers::wrap_and_expose(linker, xp_api::dref::setFloat,      "waxi_dref@1", "setFloat");
        }

        {
        // std::cout << ">> api/ export cmd module" << std::endl;
        auto res6 = LinkerHelpers::wrap_and_expose_caller_charptr(linker,           "waxi_cmd@1", "find", xp_api::cmd::find);
        auto res7 = LinkerHelpers::wrap_and_expose(linker, xp_api::cmd::begin,      "waxi_cmd@1", "begin");
        auto res8 = LinkerHelpers::wrap_and_expose(linker, xp_api::cmd::end,        "waxi_cmd@1", "end");
        auto res9 = LinkerHelpers::wrap_and_expose(linker, xp_api::cmd::once,       "waxi_cmd@1", "once");
        }

        {
        // std::cout << ">> api/ export log module" << std::endl;
        auto res10 = LinkerHelpers::wrap_and_expose_caller_charptr(linker,          "waxi_log@1", "raw", xp_api::log::raw);
        }

        {
        // std::cout << ">> api/ export cb module" << std::endl;

        //flight loops
        auto res1 = LinkerHelpers::wrap_and_expose(linker, xp_api::cb::reg,        "waxi_cb@1", "reg");
        auto res2 = LinkerHelpers::wrap_and_expose(linker, xp_api::cb::unreg,      "waxi_cb@1", "unreg");
        //auto res2a = LinkerHelpers::wrap_and_expose(linker, xp_api::cb::set_schedule, "cb", "set_schedule");
        
        //draw callbacks
        auto res3 = LinkerHelpers::wrap_and_expose(linker, xp_api::cb::reg_draw,       "waxi_cb@1", "reg_draw");
        auto res4 = LinkerHelpers::wrap_and_expose(linker, xp_api::cb::unreg_draw,     "waxi_cb@1", "unreg_draw");
        }


        {
        //drawing tools!
        auto res11 = LinkerHelpers::wrap_and_expose(linker, xp_api::gfx::draw_dbg_tri, "waxi_gfx@1", "draw_dbg_tri");
        
        }


        

    }

};


#endif
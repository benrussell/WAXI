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

#include "xp_api_nvg.h"


class XP_API{ //FIXME: this needs a new name.

public:

    static void init(wasmtime::Linker &linker, wasmtime::Store *store){
        
        std::cout << "waxi/ Init WASM X-Plane Interface" << std::endl;
        // provide some host fns
	
        {
        // std::cout << ">> api/ export dref module" << std::endl;
        auto res3 = LinkHelp::wrap_and_expose_caller_charptr(linker,               "waxi_dref@1", "find", xp_api::dref::find);
        auto res4 = LinkHelp::wrap(linker, xp_api::dref::getFloat,      "waxi_dref@1", "getFloat");
        auto res5 = LinkHelp::wrap(linker, xp_api::dref::setFloat,      "waxi_dref@1", "setFloat");
        }

        {
        // std::cout << ">> api/ export cmd module" << std::endl;
        auto res6 = LinkHelp::wrap_and_expose_caller_charptr(linker,           "waxi_cmd@1", "find", xp_api::cmd::find);
        auto res7 = LinkHelp::wrap(linker, xp_api::cmd::begin,      "waxi_cmd@1", "begin");
        auto res8 = LinkHelp::wrap(linker, xp_api::cmd::end,        "waxi_cmd@1", "end");
        auto res9 = LinkHelp::wrap(linker, xp_api::cmd::once,       "waxi_cmd@1", "once");
        }

        {
        // std::cout << ">> api/ export log module" << std::endl;
        auto res10 = LinkHelp::wrap_and_expose_caller_charptr(linker,          "waxi_log@1", "raw", xp_api::log::raw);
        }

        {
        // std::cout << ">> api/ export cb module" << std::endl;

        //flight loops
        auto res1 = LinkHelp::wrap(linker, xp_api::cb::reg,        "waxi_cb@1", "reg");
        auto res2 = LinkHelp::wrap(linker, xp_api::cb::unreg,      "waxi_cb@1", "unreg");
        //auto res2a = LinkHelp::wrap_and_expose(linker, xp_api::cb::set_schedule, "cb", "set_schedule");
        
        //draw callbacks
        auto res3 = LinkHelp::wrap(linker, xp_api::cb::reg_draw,       "waxi_cb@1", "reg_draw");
        auto res4 = LinkHelp::wrap(linker, xp_api::cb::unreg_draw,     "waxi_cb@1", "unreg_draw");
        }


        {
        //drawing tools!
        auto res11 = LinkHelp::wrap(linker, xp_api::gfx::draw_dbg_tri, "waxi_gfx@1", "draw_dbg_tri");
        
        }




        {
        //nanovg proxy

        // All of these require a custom export wrapper

        auto res01 = LinkHelp::wrap(linker, xp_api::nvg_proxy::getContextHandle, "waxi_nvg@1", "getContextHandle");

        auto res1 = LinkHelp::wrap(linker, xp_api::nvg_proxy::renderCreate, "waxi_nvg@1", "xpRenderCreate");
        auto res2 = LinkHelp::wrap(linker, xp_api::nvg_proxy::renderCreateTexture, "waxi_nvg@1", "xpRenderCreateTexture");
        auto res3 = LinkHelp::wrap(linker, xp_api::nvg_proxy::renderDeleteTexture, "waxi_nvg@1", "xpRenderDeleteTexture");
        auto res4 = LinkHelp::wrap(linker, xp_api::nvg_proxy::renderUpdateTexture, "waxi_nvg@1", "xpRenderUpdateTexture");
        
        auto res5 = LinkHelp::wrap(linker, xp_api::nvg_proxy::renderGetTextureSize, "waxi_nvg@1", "xpRenderGetTextureSize");
        auto res6 = LinkHelp::wrap(linker, xp_api::nvg_proxy::renderViewport, "waxi_nvg@1", "xpRenderViewport");
        
        // these share sig
        auto res7 = LinkHelp::wrap(linker, xp_api::nvg_proxy::renderCancel, "waxi_nvg@1", "xpRenderCancel");
        auto res8 = LinkHelp::wrap(linker, xp_api::nvg_proxy::renderFlush, "waxi_nvg@1", "xpRenderFlush");
        
        auto res9 = LinkHelp::wrap_nvg_proxy_renderFill(linker, "waxi_nvg@1", "xpRenderFill", xp_api::nvg_proxy::renderFill);
        auto res10 = LinkHelp::wrap_nvg_proxy_renderStroke(linker, "waxi_nvg@1", "xpRenderStroke", xp_api::nvg_proxy::renderStroke);
        auto res11 = LinkHelp::wrap_nvg_proxy_renderTriangles(linker, "waxi_nvg@1", "xpRenderTriangles", xp_api::nvg_proxy::renderTriangles);
        
        auto res12 = LinkHelp::wrap(linker, xp_api::nvg_proxy::renderDelete, "waxi_nvg@1", "xpRenderDelete");
        
        }

        

    }

};


#endif
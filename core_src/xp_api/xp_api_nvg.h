

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



#include "nanovg.h"




using namespace wasmtime;

namespace xp_api
{
    class nvg_proxy
    {
    public:

        // Need a method to pass a ptr into WASM for nvg context.
        // This ptr might be a NvgProxy heap instanec.
        // It might be some other more general struct.
        // It might be a ctx handle created by nvg gl?
        // do we push this into plugin_start as a param?
        // do we expose a host fn that can request a new ctx?


    //the ptrs here cross the WASM memory boundary and need book keeping
        static int renderCreate( uint64_t uptr ){
            std::cout << "waxi/nvg_proxy/renderCreate: uptr: " << uptr << "\n";

            void* mem_ptr = reinterpret_cast<void*>(uptr);

            /*
            
            Obvious solution:
            #define NANOVG_GL2_IMPLEMENTATION 1
            #include "../obsolete/nanovg_gl2.h"

            Doesn't work easily because we havent got GL fns in scope here.
            #including <GL/glew.h> leads to an explosion of errors.

            Routing to the backend functions is going to take a little more work.
            Debug logging for now will move us fwd a bit with WASM data exchange.
            */
            //int res = glnvg__renderCreate( mem_ptr );

            return 1;
        };
        static int renderCreateTexture(uint64_t uptr, int type, int w, int h, int imageFlags, uint32_t data_wptr){

            // data* is going to be a uint32_t for WASM memory space.
            // copy data from wasm to host buffer

            std::cout << "waxi/nvg_proxy/renderCreateTexture: uptr: " << uptr 
            << " type: " << type
            << " w,h: " << w << "," << h
            << " imageFlags: " << imageFlags
            << " data_wptr: " << data_wptr
            << "\n";
            return 1;
        };
	    static int renderDeleteTexture(uint64_t uptr, int image){
            std::cout << "waxi/nvg_proxy/renderDeleteTexture: uptr: " << uptr
            << "image: " << image
            << "\n";
            return 1;
        };
	    static int renderUpdateTexture(uint64_t uptr, int image, int x, int y, int w, int h, uint32_t data_wptr){

            // data* is going to be a uint32_t for WASM memory space.
            // copy data from wasm to host buffer

            std::cout << "waxi/nvg_proxy/renderUpdateTexture: uptr: " << uptr 
            << " image: " << image
            << " x,y: " << x << "," << y
            << " w,h: " << w << "," << h
            << " data_wptr: " << data_wptr
            << "\n";
            return 1;
        };

        static int renderGetTextureSize(uint64_t uptr, int image, uint32_t w_wptr, uint32_t h_wptr){

            // w and h params are ptrs so we can copy from host nvg into wasm

            std::cout << "waxi/nvg_proxy/renderGetTextureSize: uptr: " << uptr
            << "image: " << image
            << " w_wptr,h_wptr: " << w_wptr << "," << h_wptr
            << "\n";
            return 1;
        };
        static void      renderViewport(uint64_t uptr, float width, float height, float devicePixelRatio){
            std::cout << "waxi/nvg_proxy/renderViewport: uptr: " << uptr
                      << " width: " << width << ", height: " << height
                      << ", devicePixelRatio: " << devicePixelRatio << "\n";
        };
        
        //shared
        static void renderCancel(uint64_t uptr){
            std::cout << "waxi/nvg_proxy/renderCancel: uptr: " << uptr << "\n";
        };
        static void renderFlush(uint64_t uptr){
            std::cout << "waxi/nvg_proxy/renderFlush: uptr: " << uptr << "\n";
        };
        

        //almost shared, might work?
        static void      renderFill(
            uint64_t uptr, 
            uint32_t paint_wptr, // NVGpaint* paint, 
            NVGcompositeOperationState compositeOperation, // this is a struct of four ints
            uint32_t scissor_wptr, //NVGscissor* scissor, 
            float fringe, 
            const uint32_t bounds_wptr, //const float* bounds, 
            const uint32_t paths_wptr, //const NVGpath* paths,
            int npaths
            ){

            //bounds is probably an array of float[4]?

            std::cout << "waxi/nvg_proxy/renderFill: uptr: " << uptr 
                    << "  paint: " << paint_wptr
            //          << ", compositeOperation: " << compositeOperation 
                    << ", scissor: " << scissor_wptr
                    << ", fringe: " << fringe 
                    << "  bounds: " << bounds_wptr
                    << ", paths: " << paths_wptr
                    << ", npaths: " << npaths
                    << "\n";
            
        };


        static void    renderStroke(
            uint64_t uptr, 
            uint32_t paint_wptr, // NVGpaint* paint, 
            NVGcompositeOperationState compositeOperation, // this is a struct of four ints
            uint32_t scissor_wptr, //NVGscissor* scissor, 
            float fringe, 
            float strokeWidth, 
            uint32_t paths_wptr, //const NVGpath* paths, 
            int npaths
        ){

            std::cout << "waxi/nvg_proxy/renderStroke: uptr: " << uptr 
                    << "  paint: " << paint_wptr
            //          << ", compositeOperation: " << compositeOperation 
                    << ", scissor: " << scissor_wptr
                    << ", fringe: " << fringe 
                    << ", strokeWidth: " << fringe 
                    << ", paths_wptr: " << paths_wptr
                    << ", npaths: " << npaths
                    << "\n";

        };

        
        static void renderTriangles(
            uint64_t uptr, 
            uint32_t paint_wptr, // NVGpaint* paint, 
            NVGcompositeOperationState compositeOperation, // this is a struct of four ints
            uint32_t scissor_wptr, //NVGscissor* scissor, 
            uint32_t verts_wptr, //const NVGvertex* verts, 
            int nverts, 
            float fringe 
        ){

            std::cout << "waxi/nvg_proxy/renderTriangles: uptr: " << uptr 
                    << "  paint: " << paint_wptr
            //          << ", compositeOperation: " << compositeOperation 
                    << ", scissor: " << scissor_wptr
                    << ", verts_wptr: " << verts_wptr
                    << ", nverts: " << nverts
                    << ", fringe: " << fringe 
                    << "\n";

        };
        

        //shared
        static void renderDelete(uint64_t uptr){
            std::cout << "waxi/nvg_proxy/renderDelete: uptr: " << uptr << "\n";

        };
        
    };
}
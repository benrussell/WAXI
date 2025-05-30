

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
        static int renderCreate( uint64_t ptr ){};
        static int renderCreateTexture(uint64_t uptr, int type, int w, int h, int imageFlags, const unsigned char* data){};
	    static int renderDeleteTexture(uint64_t uptr, int image){};
	    static int renderUpdateTexture(uint64_t uptr, int image, int x, int y, int w, int h, const unsigned char* data){};

        static int renderGetTextureSize(uint64_t uptr, int image, int* w, int* h){};
        static void      renderViewport(uint64_t uptr, float width, float height, float devicePixelRatio){};
        
        //shared
        static void renderCancel(uint64_t uptr){};
        static void renderFlush(uint64_t uptr){};
        
        //almost shared, might work?
        static void      renderFill(uint64_t uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, float fringe, const float* bounds, const NVGpath* paths, int npaths){};
        static void    renderStroke(uint64_t uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, float fringe, float strokeWidth, const NVGpath* paths, int npaths){};

        static void renderTriangles(uint64_t uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, const NVGvertex* verts, int nverts, float fringe){};
        
        //shared
        static void renderDelete(uint64_t uptr){};
        
    };
}
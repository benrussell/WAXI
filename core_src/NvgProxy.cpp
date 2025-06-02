
#include "NvgProxy.h"

#include <iostream>

#include <GL/glew.h>

#include "nanovg_gl.h"




#include <fstream>
#include <iostream>
#include <sstream>
#include <wasmtime.hh>
#include <cstring>
#include <iomanip>


#include "LinkerHelpers.h" //FIXME: rename this file

using namespace wasmtime;



NvgProxy::NvgProxy(){

    std::cout << "waxi/api/nvg: Custom Params NvgProxy wrapper\n";

    #if 1
    //struct NVGparams params;
    struct NVGcontext* ctx = NULL;
	struct GLNVGcontext* gl = (struct GLNVGcontext*)malloc(sizeof(struct GLNVGcontext));
	if (gl == NULL) goto error;
	memset(gl, 0, sizeof(struct GLNVGcontext));

    //printf("waxi/NvgProxy/ctor late bind glnvg__renderCreate ptr: %p\n", glnvg__renderCreate);
    //printf("waxi/NvgProxy/ctor GLNVGcontext ptr: %lu\n", gl);

    m_gl_ptr = (uint64_t)gl; //FIXME: This is a hack but its the magic we needed to get the correct contexxt in/out of WASM.


	memset(&m_params, 0, sizeof(m_params));
	m_params.renderCreate = glnvg__renderCreate;
	m_params.renderCreateTexture = glnvg__renderCreateTexture;
	m_params.renderDeleteTexture = glnvg__renderDeleteTexture;
	m_params.renderUpdateTexture = glnvg__renderUpdateTexture;
	m_params.renderGetTextureSize = glnvg__renderGetTextureSize;
	m_params.renderViewport = glnvg__renderViewport;
	m_params.renderFlush = glnvg__renderFlush;
	m_params.renderFill = glnvg__renderFill;
	m_params.renderStroke = glnvg__renderStroke;
	m_params.renderTriangles = glnvg__renderTriangles;
	m_params.renderDelete = glnvg__renderDelete;
	m_params.userPtr = gl;

    // //these are usually args to nvgCreateGL2(...)
    // int atlasw=NVG_ANTIALIAS | NVG_STENCIL_STROKES;
    // int atlash=0;
    // int edgeaa=0;

	// params.atlasWidth = atlasw;
	// params.atlasHeight = atlash;
	// params.edgeAntiAlias = edgeaa;

	//gl->edgeAntiAlias = edgeaa;

    ctx = nvgCreateInternal(&m_params);
	if (ctx == NULL) goto error;

	//return ctx;
    m_vg = ctx;

    //printf("NvgProxy created a new new context @: %lu\n", m_vg);

    if (m_vg == nullptr) {
		std::cout << " Init nvg: FAILED\n";
        //return; // Failed to initialize //FIXME: throw?
        throw std::runtime_error("NVG failed.");
    }
    return;

error:
	// 'gl' is freed by nvgDeleteInternal.
	if (ctx != NULL) nvgDeleteInternal(ctx);
	//return NULL;
    throw std::runtime_error("NVG failed - unknown error");
#endif




// Basic automatic GL context create - works for BKIn28
#if 0

    std::cout << " Init nvg! ********************\n";
    m_vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
//	m_vg = nvgCreateGL2(  NVG_DEBUG);
    if (m_vg == nullptr) {
		std::cout << " Init nvg: FAILED\n";
        //return; // Failed to initialize //FIXME: throw?
        throw std::runtime_error("NVG failed.");
    }

#endif


    //auto fh = glnvg__renderCreate;
    // Custom renderFlush functionality removed to avoid incomplete type errors.
    
}


NvgProxy::~NvgProxy(){
    nvgDeleteGL2(m_vg);
}



void NvgProxy::init(wasmtime::Linker &linker, wasmtime::Store *store){
    
    std::cout << "waxi/ Init WAXI/ nvg proxy - NOOP!" << std::endl;
    // provide some host fns

    {
    // // std::cout << ">> api/ export dref module" << std::endl;
    // auto res3 = LinkHelp::wrap_and_expose_caller_charptr(linker,               "waxi_dref@1", "find", xp_api::dref::find);
    // auto res4 = LinkHelp::wrap(linker, xp_api::dref::getFloat,      "waxi_dref@1", "getFloat");
    // auto res5 = LinkHelp::wrap(linker, xp_api::dref::setFloat,      "waxi_dref@1", "setFloat");
    }

}
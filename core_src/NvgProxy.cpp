
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

    struct NVGparams params;
    struct NVGcontext* ctx = NULL;
	struct GLNVGcontext* gl = (struct GLNVGcontext*)malloc(sizeof(struct GLNVGcontext));
	if (gl == NULL) goto error;
	memset(gl, 0, sizeof(struct GLNVGcontext));

	memset(&params, 0, sizeof(params));
	params.renderCreate = glnvg__renderCreate;
	params.renderCreateTexture = glnvg__renderCreateTexture;
	params.renderDeleteTexture = glnvg__renderDeleteTexture;
	params.renderUpdateTexture = glnvg__renderUpdateTexture;
	params.renderGetTextureSize = glnvg__renderGetTextureSize;
	params.renderViewport = glnvg__renderViewport;
	params.renderFlush = glnvg__renderFlush;
	params.renderFill = glnvg__renderFill;
	params.renderStroke = glnvg__renderStroke;
	params.renderTriangles = glnvg__renderTriangles;
	params.renderDelete = glnvg__renderDelete;
	params.userPtr = gl;

    // //these are usually args to nvgCreateGL2(...)
    // int atlasw=NVG_ANTIALIAS | NVG_STENCIL_STROKES;
    // int atlash=0;
    // int edgeaa=0;

	// params.atlasWidth = atlasw;
	// params.atlasHeight = atlash;
	// params.edgeAntiAlias = edgeaa;

	//gl->edgeAntiAlias = edgeaa;

    ctx = nvgCreateInternal(&params);
	if (ctx == NULL) goto error;

	//return ctx;
    m_vg = ctx;

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
    // auto res3 = LinkerHelpers::wrap_and_expose_caller_charptr(linker,               "waxi_dref@1", "find", xp_api::dref::find);
    // auto res4 = LinkerHelpers::wrap_and_expose(linker, xp_api::dref::getFloat,      "waxi_dref@1", "getFloat");
    // auto res5 = LinkerHelpers::wrap_and_expose(linker, xp_api::dref::setFloat,      "waxi_dref@1", "setFloat");
    }

}
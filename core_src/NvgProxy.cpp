
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


#include "LinkHelp.h" //FIXME: rename this file

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





void NvgProxy::draw_test(){

    auto lam_drawRose = [](auto vg){
// Draw tick marks around a circle
		float centerX = 400 / 2;
		float centerY = 400 / 2;
		float radius = 50;
		float tickLength = 20;


		// Save the current transformation state
		nvgSave(vg);

		// Translate to the center of the circle
		nvgTranslate(vg, centerX, centerY);

		// Apply the rotation transformation
		//nvgRotate(vg, dr_heading->getFloat());
        nvgRotate(vg, 7.f);

		// Translate back to the original position
		nvgTranslate(vg, -centerX, -centerY);

		for (int i = 0; i < 12; ++i) {
			float angle = (i / 12.0f) * NVG_PI * 2;
			float x1 = centerX + cos(angle) * radius;
			float y1 = centerY + sin(angle) * radius;
			float x2 = centerX + cos(angle) * (radius - tickLength);
			float y2 = centerY + sin(angle) * (radius - tickLength);

			nvgBeginPath(vg);
			nvgMoveTo(vg, x1, y1);
			nvgLineTo(vg, x2, y2);
			//nvgStrokeColor(vg, nvgRGBA(255, 192, 0, 255));
            nvgStrokeColor(vg, nvgRGBA(255, 0, 255, 255));
			nvgStrokeWidth(vg, 10.0f);
			nvgStroke(vg);
		}

		nvgRestore(vg);

	};

    
    //std::cout << "NvgProxy::draw_test() nvg draw..\n";
    nvgBeginFrame( m_vg, 400, 400, 1.f );
        lam_drawRose(m_vg);
    nvgEndFrame( m_vg );



}



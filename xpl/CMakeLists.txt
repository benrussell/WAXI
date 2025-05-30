
# Build the X-Plane plugin.
# Currently works on Linux Mint 21.
# Output filename is libWAXI_xpl.so

project(WAXI_xpl)


set( INC_GLEW 
        third_party/glew-2.2.0/include
)


set( INC_JSON 
        third_party/json-develop/include
)


set( INC_NVG
        third_party/nanovg-master/src
)

# X-Planen Mac only gives us GL2.1
add_definitions( -DNANOVG_GL2=1 -DNANOVG_GL_IMPLEMENTATION=1 )

set( NVG_ROOT third_party/nanovg-master/src )
set( NVG_SRC
        #${NVG_ROOT}/fontstash.h
        ${NVG_ROOT}/nanovg.c ${NVG_ROOT}/nanovg.h
        ${NVG_ROOT}/nanovg_gl.h
        ${NVG_ROOT}/nanovg_gl_utils.h
        #${NVG_ROOT}/stb_image.h
        #${NVG_ROOT}/stb_truetype.h
        )





# Common defines needed for XPLM_SDK v4 on Linux
add_definitions(-DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM400=1)

add_definitions(-DIBM=0 -DMAC=0 -DLIN=1 )


set( INC_XPLANE_SDK
        third_party/XPSDK410/CHeaders/Widgets
        third_party/XPSDK410/CHeaders/Wrappers
        third_party/XPSDK410/CHeaders/XPLM
        )

set( LIB_XPLANE_SDK third_party/XPSDK410/Libraries/Lin )

# SDK defines


include_directories(
        ${INC_GLEW}
        ${INC_JSON}
        ${INC_NVG}
        ${INC_XPLANE_SDK}
        ${INC_WASM_XPL_CORE}
)



set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(WAXI_xpl SHARED
        ${NVG_SRC}
        ../core_src/NvgProxy.cpp
        xplane_plugin_main.cpp
        )


target_link_libraries( WAXI_xpl  ${USE_LIBS} )



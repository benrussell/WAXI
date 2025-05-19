

project(wasm_xpl_plugin)


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
        ${INC_XPLANE_SDK}
        ${INC_WASM_XPL_CORE}
)





set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(wasm_xpl_plugin SHARED
        xplane_plugin_main.cpp
        )


target_link_libraries( wasm_xpl_plugin  ${USE_LIBS} )






project(wasm_xpl_plugin)


set( INC_XPLANE_SDK XP_SDK/CHeaders )
set( LIB_XPLANE_SDK XP_SDK/lib/linux )

# SDK defines




set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(wasm_xpl_plugin SHARED
        xplane_plugin_main.cpp
        )


target_link_libraries( wasm_xpl_plugin  ${USE_LIBS} )




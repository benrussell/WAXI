

project(wasm_xpl_plugin)


set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(wasm_xpl_plugin SHARED
        xplane_plugin_main.cpp
        )

# add_executable(wasm_xpl_plugin
#         main.cpp
#         )


target_link_libraries( wasm_xpl_plugin  ${USE_LIBS} )




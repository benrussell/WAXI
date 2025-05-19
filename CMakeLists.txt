cmake_minimum_required(VERSION 3.17)
project(wasm_xpl_root)

set(CMAKE_CXX_STANDARD 20)


set(INC_WASMTIME ../wasmtime/wasmtime_src/wasmtime/include)
set(LNK_WASMTIME ../wasmtime/wasmtime_src/wasmtime/lib)
set(LIB_WASMTIME wasmtime)


set(INC_WASM_XPL_CORE ${CMAKE_SOURCE_DIR}/core_src)


include_directories(
        ${INC_WASMTIME}
        ${INC_WASM_XPL_CORE}
        )

link_directories(
        ${LNK_WASMTIME}
)


set( USE_LIBS
        ${LIB_WASMTIME}
)


subdirs( "cli" "xpl" )


# add_executable(wasm_host
#         cli/main.cpp
#         )

# target_link_libraries( wasm_host  ${USE_LIBS} )




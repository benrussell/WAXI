

project(wasm_xpl_cli)


add_executable(wasm_xpl_cli
        main.cpp
        )

target_link_libraries( wasm_xpl_cli  ${USE_LIBS} )




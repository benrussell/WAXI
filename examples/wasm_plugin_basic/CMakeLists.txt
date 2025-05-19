cmake_minimum_required(VERSION 3.13)
project(wasm_plugin_basic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(INC_HOST_API ${CMAKE_SOURCE_DIR}/../host_api)

include_directories(
        ${INC_HOST_API}
        )


add_executable(wasm_plugin_basic.wasm wasm_plugin_basic.cpp)

# Pass exported functions to the linker (for Emscripten)
set_target_properties(wasm_plugin_basic.wasm PROPERTIES
    LINK_FLAGS "-Wl,--export=plugin_start -Wl,--export=plugin_stop -Wl,--export=plugin_enable -Wl,--export=plugin_disable -Wl,--export=plugin_message -Wl,--export=malloc -Wl,--export=free -Wl,--allow-undefined "
)



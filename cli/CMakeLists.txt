

project(WAXI_cli)


add_executable(WAXI_cli
        main.cpp
        )

target_link_libraries( WAXI_cli  ${USE_LIBS} )




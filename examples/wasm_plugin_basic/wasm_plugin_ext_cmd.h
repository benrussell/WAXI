#ifndef WASM_PLUGIN_EXT_CMD_H
#define WASM_PLUGIN_EXT_CMD_H

#include <cstring>
#include <iostream>
#include <cstdio>

#include "host_api.h"


void test_cmds(){
    printf("wasm/ test_cmds()\n");
    auto cmdh_b = cmd_find("cmd/b");
    auto cmdh_c = cmd_find("cmd/c");
    cmd_begin(cmdh_c);
    cmd_end(cmdh_c);
    cmd_once(cmdh_b);
}

#endif

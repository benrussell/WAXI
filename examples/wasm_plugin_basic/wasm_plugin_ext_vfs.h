#ifndef WASM_PLUGIN_EXT_VFS_H
#define WASM_PLUGIN_EXT_VFS_H

#include <cstring>
#include <iostream>
#include <cstdio>

#include <dirent.h>


#include "host_api.h"




void test_vfs(){

    // List all files in the current folder using filesystem APIs
    printf("wasm/ Listing files in the current folder:\n");

    DIR* dir = opendir(".");
    if (dir == nullptr) {
        printf("wasm/ vfs root is a nullptr?\n");
        perror("opendir");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        printf("wasm/     vfs://%s\n", entry->d_name);
    }

    closedir(dir);

}





#endif

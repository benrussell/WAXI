#ifndef WASM_PLUGIN_EXT_DREF_H
#define WASM_PLUGIN_EXT_DREF_H

#include <cstring>
#include <iostream>
#include <cstdio>

#include "host_api.h"


void test_drefs(){
    printf("wasm/ test_drefs()\n");

    {
        int32_t drh = dref_find("dref/a");
        float val_before = dref_getFloat( drh );
        dref_setFloat( drh, 1.23 );
        float val_after = dref_getFloat( drh );
        printf("wasm/   dref_getFloat(a) before: %f\n", val_before);
        printf("wasm/   dref_getFloat(a) after: %f\n", val_after);
    }

    {
        int32_t drh = dref_find("dref/b");
        float val_before = dref_getFloat( drh );
        dref_setFloat( drh, 9.23 );
        float val_after = dref_getFloat( drh );
        printf("wasm/   dref_getFloat(b) before: %f\n", val_before);
        printf("wasm/   dref_getFloat(b) after: %f\n", val_after);
    }

    int32_t drh = dref_find("dref/_invalid");

}



#endif

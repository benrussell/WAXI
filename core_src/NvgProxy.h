#ifndef NVG_PROXY_H
#define NVG_PROXY_H



#include "nanovg.h"


#include <fstream>
#include <iostream>
#include <sstream>
#include <wasmtime.hh>
#include <cstring>
#include <iomanip>


#include "LinkerHelpers.h" //FIXME: rename this file

using namespace wasmtime;




class NvgProxy{
    public:
    NVGcontext* m_vg;
    NVGparams m_params;

    uint64_t m_gl_ptr;


    public:
    NvgProxy();
    ~NvgProxy();

    //void export_to_wasm();

    void draw_test();


};


#endif
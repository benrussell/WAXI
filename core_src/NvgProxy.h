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
    private:
    NVGcontext* m_vg;


    public:
    NvgProxy();
    ~NvgProxy();

    //void export_to_wasm();

    void init(wasmtime::Linker &linker, wasmtime::Store *store);


};


#endif
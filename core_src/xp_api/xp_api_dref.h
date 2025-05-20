

#include <fstream>
#include <iostream>
#include <sstream>
#include <wasmtime.hh>
#include <cstring>
#include <iomanip>

// #include "WasmVM.h"

#include "LinkerHelpers.h"

using namespace wasmtime;

namespace xp_api
{
    class dref
    {
    public:
        static int64_t find(Caller caller, char *dref_name)
        {
            std::cout << ">> api/ dref_find(" << dref_name << ")" << std::endl;

            // Example array of three char* values
            static const char *dref_names[] = {"dref/a", "dref/b", "dref/c"};

            for (int i = 0; i < 3; ++i)
            {
                if (std::strcmp(dref_names[i], dref_name) == 0)
                {
                    return i;
                }
            }

            std::cerr << " * Invalid dref name: " << dref_name << std::endl;
            return -1;
        }

        static float m_dr_values[3];

        static float getFloat(Caller caller, int32_t drh)
        {
            // std::cout << ">> api/ dref_getFloat(" << drh << ")" << std::endl;
            if (drh >= 0)
            {
                return m_dr_values[drh];
            }
            return 0.f;
        }

        static void setFloat(Caller caller, int32_t drh, float value)
        {
            // std::cout << ">> api/ dref_setFloat(" << drh << ", " << value << ")" << std::endl;
            if (drh >= 0)
            {
                m_dr_values[drh] = value;
            }
            else
            {
                std::cerr << "Invalid drh" << std::endl;
            }
        }
    };
}

float xp_api::dref::m_dr_values[3];

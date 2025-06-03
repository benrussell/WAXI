

#include <fstream>
#include <iostream>
#include <sstream>
#include <wasmtime.hh>
#include <cstring>
#include <iomanip>

// #include "WasmVM.h"

#include "LinkHelp.h"

using namespace wasmtime;

namespace xp_api
{
    class cmd
    {
    public:
        static int64_t find(Caller caller, char *cmd_name)
        {
            std::cout << ">> api/ cmd_find(" << cmd_name << ")" << std::endl;

            // Example array of three char* values
            static const char *cmd_names[] = {"cmd/a", "cmd/b", "cmd/c"};

            for (int i = 0; i < 3; ++i)
            {
                if (std::strcmp(cmd_names[i], cmd_name) == 0)
                {
                    return i;
                }
            }

            std::cerr << " * Invalid cmd name: " << cmd_name << std::endl;
            return -1;
        }

        static void begin(Caller caller, int32_t cmdh)
        {
            std::cout << ">> api/ cmd_begin(" << cmdh << ")" << std::endl;
        }

        static void end(Caller caller, int32_t cmdh)
        {
            std::cout << ">> api/ cmd_end(" << cmdh << ")" << std::endl;
        }

        static void once(Caller caller, int32_t cmdh)
        {
            std::cout << ">> api/ cmd_once(" << cmdh << ")" << std::endl;
        }
    };
}
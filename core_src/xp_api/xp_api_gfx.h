

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
    class gfx
    {
    public:
        static void draw_dbg_tri(Caller caller, int x, int y)
        {
            //std::cout << "waxi/api/ gfx_draw_dbg_tri(" << x << "," << y << ")" << std::endl;


                glDisable( GL_CULL_FACE );

                glPushMatrix();

                glTranslatef( x * 1.f, y * 1.f, 0 );
                
                //glRotatef( rot, 0.f,0.f,1.f);
                //rot += 0.5f;

                glBegin(GL_TRIANGLES);
                            glColor3f( 1.f, 0.f, 0.f );
                            glVertex3f( -50.0f, -33.33f, 0.f );

                            glColor3f( 0.f, 1.f, 0.f );
                            glVertex3f( 50.0f, -33.33f, 0.f );

                            glColor3f( 0.f, 0.f, 1.f );
                            glVertex3f( 0.0f, 66.67f, 0.f );
                            glEnd();
                glPopMatrix();


                glEnable( GL_CULL_FACE );



            #if 0
            glPushMatrix();

                glTranslatef( x * 1.f, y * 1.f, 0 );

                glPushAttrib(GL_ALL_ATTRIB_BITS);
                glDisable(GL_CULL_FACE);

                // Set drawing color to green
                glColor3f(1.0f, 0.0f, 1.0f);

                // Draw a simple rectangle
                //glBegin(GL_LINE_LOOP);
                glBegin(GL_QUADS);
                    glVertex2f(50.0f, 50.0f);
                    glVertex2f(250.0f, 50.0f);
                    glVertex2f(250.0f, 150.0f);
                    glVertex2f(50.0f, 150.0f);
                glEnd();

                glPopAttrib();

            glPopMatrix();
#endif



            // // Example array of three char* values
            // static const char *cmd_names[] = {"cmd/a", "cmd/b", "cmd/c"};

            // for (int i = 0; i < 3; ++i)
            // {
            //     if (std::strcmp(cmd_names[i], cmd_name) == 0)
            //     {
            //         return i;
            //     }
            // }

            // std::cerr << " * Invalid cmd name: " << cmd_name << std::endl;
            // return -1;
        }

        // static void begin(Caller caller, int32_t cmdh)
        // {
        //     std::cout << ">> api/ cmd_begin(" << cmdh << ")" << std::endl;
        // }

        // static void end(Caller caller, int32_t cmdh)
        // {
        //     std::cout << ">> api/ cmd_end(" << cmdh << ")" << std::endl;
        // }

        // static void once(Caller caller, int32_t cmdh)
        // {
        //     std::cout << ">> api/ cmd_once(" << cmdh << ")" << std::endl;
        // }
    };
}
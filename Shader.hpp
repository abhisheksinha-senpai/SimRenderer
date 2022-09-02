#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <string.h>
#include "Definitions.hpp"
#include "ExternalSrc/Include/glad/glad.h"
#include "ExternalSrc/Include/glm/glm.hpp"

namespace Render
{
    class Shader
    {
        private:
        unsigned int shader_pgm;
        unsigned int src_ver_id;
        unsigned int src_frag_id;
        char *src_ver_mem;
        char *src_frag_mem;
        public:
        Shader(){};
        void create_vs_shader(const char* src_ver);
        void create_fs_shader(const char* src_frag);
        void compile();
        void use();
        void check_errors(unsigned int object, std::string type);
    };
}

#endif
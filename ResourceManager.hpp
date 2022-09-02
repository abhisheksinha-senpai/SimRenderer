#ifndef RM_H
#define RM_H

#include <iostream>
#include <sstream>
#include <string.h>
#include <fstream>

#include "Definitions.hpp"

namespace Render{
    class ResourceManager
    {
        private:
        std::string vertex_shader;
        std::string fragment_shader;
        public:
        ResourceManager(){};
        ReturnType load_shader(std::string file_name, std::string shader_type);
        std::string  get_vertex_shader();
        std::string  get_fragment_shader();
    };
}

#endif // !RM_H
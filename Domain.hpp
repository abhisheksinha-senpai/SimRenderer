#ifndef DOMAIN_H
#define DOMAIN_H

#include <iostream>
#include <vector>

#include "Definitions.hpp"
#include "Shader.hpp"

namespace Render
{
    class Domain
    {
        private:
        unsigned int VBO, VAO;
        unsigned int points;
        public:
        int *domain_classification;
        float *cur_vel_x;
        float *cur_vel_y;
        float *cur_vel_z;
        Shader shader;
        float *prev_vel_x;
        float *prev_vel_y;
        float *prev_vel_z;

        float *domain_array;

        std::vector<std::vector<int>> start_position;

        Domain(float *vel_x, float *vel_y, float *vel_z, int *dom, const char* domain_ver_shader, const char* domain_frag_shader);
        ~Domain();
        std::vector<std::vector<int>> get_start_location();
        void update_fields(float *vel_x, float *vel_y, float *vel_z);
        void draw_domain();
    };
}

#endif
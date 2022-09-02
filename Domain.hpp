#ifndef DOMAIN_H
#define DOMAIN_H

#include <iostream>
#include <vector>

#include "Definitions.hpp"

namespace Render
{
    class Domain
    {
        private:
        
        public:
        int *domain_classification;
        float *cur_vel_x;
        float *cur_vel_y;
        float *cur_vel_z;

        float *prev_vel_x;
        float *prev_vel_y;
        float *prev_vel_z;

        std::vector<std::vector<int>> start_position;

        Domain(float *vel_x, float *vel_y, float *vel_z, int *dom);
        ~Domain();
        std::vector<std::vector<int>> get_start_location();
        void update_fields(float *vel_x, float *vel_y, float *vel_z);
    };
}

#endif
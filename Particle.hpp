#ifndef PARTICLE_H
#define PARTICLE_H

#include <iostream>
#include <vector>
#include <random>

#include "ExternalSrc/Include/glm/glm.hpp"
#include "Shader.hpp"
#include "Domain.hpp"
#include "Definitions.hpp"

#define MAX_FLUID_PARTICLES 500000
namespace Render{
    struct FluidParticle
    {
        std::vector<float> position;
        std::vector<float>color;
        float life;
    };

    class Fluid{
        private:
        std::vector<FluidParticle> fluid_field;
        unsigned int VBO, VAO;
        Shader shader;
        unsigned int earliest_fluid_particle_index = 0;
        public:
        Fluid(const char* fluid_ver_shader, const char* fluid_frag_shader);
        ~Fluid();
        void generate_fluid_particle(Render::Domain &domain);
        void update_fluid_particle(FluidParticle &fluid_cell, Render::Domain &domain);
        void advect_fluid_particle(float delta_time, Render::Domain &domain);
        void draw_fluid_particle();
        bool check_fluid_particle_position(std::vector<float> &position, Render::Domain &domain);
        int get_earliest_created_fluid_particle();
    };
}
#endif
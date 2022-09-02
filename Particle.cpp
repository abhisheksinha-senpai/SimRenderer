#include "Particle.hpp"

Render::Fluid::Fluid(const char* fluid_ver_shader, const char* fluid_frag_shader)
{
    try
    {
        this->shader = Shader();
        this->shader.create_vs_shader(fluid_ver_shader);
        this->shader.create_fs_shader(fluid_frag_shader);
        this->shader.compile();
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
    }
    catch(std::exception e)
    {
        std::cout<<"Error while trying to create Fluid domain..."<<"\n";
        throw e;
    }
}

int Render::Fluid::get_earliest_created_fluid_particle()
{
    for(int i=this->earliest_fluid_particle_index;i<this->fluid_field.size();i++)
    {
        if(this->fluid_field[i].life<=0.0f)
        {
            this->earliest_fluid_particle_index = i;
            return i;
        } 
    }
    for(int i=0;i<this->earliest_fluid_particle_index;i++)
    {
        if(this->fluid_field[i].life<=0.0f)
        {
            this->earliest_fluid_particle_index = i;
            return i;
        }
    }
    this->earliest_fluid_particle_index = 0;
    return 0;
}

void Render::Fluid::generate_fluid_particle(Render::Domain &domain)
{
    std::vector<std::vector<int>>positions = domain.get_start_location();
    for(int i =0;i<positions.size();i++)
    {
        if(this->fluid_field.size()<MAX_FLUID_PARTICLES)
        {
            float scrn_x = (float)positions[i][0]*2.0/NX -1.0;
            float scrn_y = (float)positions[i][1]*2.0/NY -1.0;
            float scrn_z = (float)positions[i][2]*2.0/NZ -1.0;
            Render::FluidParticle fcell = {std::vector<float>{scrn_x, scrn_y, scrn_z}, std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f}, 10.0f};
            this->fluid_field.push_back(fcell);
        }
        else
        {
            int index = get_earliest_created_fluid_particle();
            update_fluid_particle(this->fluid_field[i], domain);
        }
    }
}

void Render::Fluid::draw_fluid_particle()
{
    int state_variables = 7;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    float dataArray[fluid_field.size()*state_variables];
    for(int i=0;i<this->fluid_field.size();i++)
    {
        int loc = i*state_variables;
        dataArray[loc] = fluid_field[i].position[0];
        dataArray[loc+1] = fluid_field[i].position[1];
        dataArray[loc+2] = fluid_field[i].position[2];

        dataArray[loc+3] = fluid_field[i].color[0];
        dataArray[loc+4] = fluid_field[i].color[1];
        dataArray[loc+5] = fluid_field[i].color[2];
        dataArray[loc+6] = fluid_field[i].color[3];
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(dataArray), dataArray, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)12);
    glEnableVertexAttribArray(1);
    
    this->shader.use();
    glDrawArrays(GL_POINTS, 0, fluid_field.size());
}

void Render::Fluid::advect_fluid_particle(float delta_time, Render::Domain &domain)
{
    try
    {
        for(auto &fluid_cell : fluid_field)
        {
            if(fluid_cell.life>0)
            {
                std::vector<float>new_pos(fluid_cell.position);
                int i = (int)((fluid_cell.position[0]+1)*NX/2);
                int j = (int)((fluid_cell.position[1]+1)*NY/2);
                int index = 0*NX*NY + NX*j + i;
                std::vector<float>delV = {(domain.cur_vel_x[index] + domain.prev_vel_x[index])/2, (domain.cur_vel_y[index] + domain.prev_vel_y[index])/2, (domain.cur_vel_z[index] + domain.prev_vel_z[index])/2}; 
                new_pos[0] += delV[0]*delta_time;
                new_pos[1] += delV[1]*delta_time;
                new_pos[2] += delV[2]*delta_time;
                int status = check_fluid_particle_position(new_pos, domain);
                if (status)
                {
                    fluid_cell.position.swap(new_pos);
                    fluid_cell.life -= delta_time;
                }
                else
                    update_fluid_particle(fluid_cell, domain);
            }
            else
            {
                update_fluid_particle(fluid_cell, domain);
            }
        }
    }
    catch(std::exception e)
    {
        std::cout<<"Unable to update fluid field"<<"\n";
        throw e;
    }
}

void Render::Fluid::update_fluid_particle(FluidParticle &fluid_cell, Render::Domain &domain)
{
    try
    {
        std::vector<std::vector<int>>position = domain.get_start_location();
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(0,position.size()-1);
        int loc_idx = distribution(generator);
        std::vector<float>loc = {position[loc_idx][0]*2.0f/NX - 1.0f,position[loc_idx][1]*2.0f/NY - 1.0f,position[loc_idx][1]*2.0f/NZ - 1.0f};
        fluid_cell.position = loc;
        fluid_cell.life = 10.0f;
    }
    catch(std::exception e)
    {
        std::cout<<"Unable to update fluid particles"<<"\n";
        throw e;
    }
}

bool Render::Fluid::check_fluid_particle_position(std::vector<float> &position, Render::Domain &domain)
{
    int i = (int)((position[0]+1)*NX/2);
    int j = (int)((position[1]+1)*NY/2);
    int index = 0*NX*NY + NX*j + i; 
    if (domain.domain_classification[index] ==4)
        return false;
    else
        return true;
}

Render::Fluid::~Fluid()
{
    glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
    std::cout<<"Fluid Domain deleted......."<<"\n";
}
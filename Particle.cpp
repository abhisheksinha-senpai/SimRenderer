#include "Particle.hpp"
#include <typeinfo>

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
    if(this->fluid_field.size()<MAX_FLUID_PARTICLES)
    {
        std::vector<std::vector<int>>positions = domain.get_start_location();
        for(int i =0;i<positions.size();i++)
        {
       
            float scrn_x = (float)positions[i][0]*2.0/NX -1.0;
            float scrn_y = (float)positions[i][1]*2.0/NY -1.0;
            float scrn_z = (float)positions[i][2]*1.0/NZ;
            Render::FluidParticle fcell = {std::vector<float>{scrn_x, scrn_y, scrn_z}, std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f}, 10.0f};
            this->fluid_field.push_back(fcell);
        }
        
    }
    // else
        // {
        //     int index = get_earliest_created_fluid_particle();
        //     update_fluid_particle(this->fluid_field[i], domain);
        // }
}

void Render::Fluid::draw_fluid_particle()
{
    int state_variables = 7;
    this->shader.use();
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    
    std::string model_id = "model";
    std::string proj_id = "projection";
    std::string view_id = "view";
    glUniformMatrix4fv(glGetUniformLocation(this->shader.get_shader_pgm(), model_id.c_str()), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(this->shader.get_shader_pgm(), proj_id.c_str()), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(this->shader.get_shader_pgm(), view_id.c_str()), 1, GL_FALSE, &view[0][0]);
    
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
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Render::Fluid::advect_fluid_particle(float delta_time, Render::Domain &domain)
{
    std::cout<<fluid_field.size()<<" ";
    try
    {
        for(auto &fluid_cell : fluid_field)
        {
           if(fluid_cell.life>0)
            {
                std::vector<float>new_pos(fluid_cell.position);
                int i = (int)((fluid_cell.position[0]+1)*NX/2);
                int j = (int)((fluid_cell.position[1]+1)*NY/2);
                int k = (int)((fluid_cell.position[2])*NZ);
                int index = k*NX*NY + NX*j + i;
                // std::cout<<i<<" "<<j<<" "<<k<<" "<<index<<std::endl;
                std::vector<float>delV = {(domain.cur_vel_x[index] + domain.prev_vel_x[index])/2, (domain.cur_vel_y[index] + domain.prev_vel_y[index])/2, (domain.cur_vel_z[index] + domain.prev_vel_z[index])/2};
                // std::cout<<delV[0]<<" "<<delV[1]<<" "<<delV[2]<<"\n"; 
                new_pos[0] += delV[0]*delta_time;
                new_pos[1] += delV[1]*delta_time;
                new_pos[2] += delV[2]*delta_time;
                if (abs(new_pos[0]) >=1.0f || abs(new_pos[1]) >=1.0f || abs(new_pos[2]) >=1.0f)
                    update_fluid_particle(fluid_cell, domain);
                else
                {
                    int status = check_fluid_particle_position(new_pos, domain);
                    if (status)
                    {
                        fluid_cell.position.swap(new_pos);
                        fluid_cell.life -= delta_time;
                    }
                    else
                        update_fluid_particle(fluid_cell, domain);
                }
            }
            else
                update_fluid_particle(fluid_cell, domain);
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
        int loc_idx = rand()%(position.size() - 0) + 0; 
        std::vector<float>loc = {position[loc_idx][0]*2.0f/NX - 1.0f, position[loc_idx][1]*2.0f/NY - 1.0f, position[loc_idx][2]*1.0f/NZ};
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
    int k = (int)((position[2])*NZ);
    int index = k*NX*NY + NX*j + i;
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
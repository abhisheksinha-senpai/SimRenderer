#include "Domain.hpp"

Render::Domain::Domain(float *vel_x, float *vel_y, float *vel_z, int *dom, const char* domain_ver_shader, const char* domain_frag_shader)
{
    this->points = 0;
    try
    {
        this->cur_vel_x = new float[NX*NY*NZ]();
        this->cur_vel_y = new float[NX*NY*NZ]();
        this->cur_vel_z = new float[NX*NY*NZ]();

        this->prev_vel_x = new float[NX*NY*NZ]();
        this->prev_vel_y = new float[NX*NY*NZ]();
        this->prev_vel_z = new float[NX*NY*NZ]();
        this->domain_classification = new int[NX*NY*NZ]();
    }
    catch(std::exception e)
    {
        throw std::overflow_error(std::string("Memory not available to assign"));
    }
    std::vector<float> domain_class;
    try
    {
        for(int k=0;k<NZ;k++)
        {
            for(int j=0;j<NY;j++)
            {
                for(int i=0;i<NX;i++)
                {
                    this->prev_vel_x[k*NX*NY+j*NX+i] = vel_x[k*NX*NY+j*NX+i];
                    this->prev_vel_y[k*NX*NY+j*NX+i] = vel_y[k*NX*NY+j*NX+i];
                    this->prev_vel_z[k*NX*NY+j*NX+i] = vel_z[k*NX*NY+j*NX+i];
                    this->domain_classification[k*NX*NY+j*NX+i] = dom[k*NX*NY+j*NX+i];

                    if(this->domain_classification[k*NX*NY+j*NX+i]==2)
                        this->start_position.push_back(std::vector<int>{i,j,k});
                    else if (this->domain_classification[k*NX*NY+j*NX+i]!=1 && this->domain_classification[k*NX*NY+j*NX+i]!=2)
                    {
                        domain_class.push_back(i*2.0f/NX-1);
                        domain_class.push_back(j*2.0f/NY-1);
                        domain_class.push_back(k*1.0f/NZ);
                        points += 1;
                    }
                }
            }
        }
    }
    catch(std::exception e)
    {
        std::cout<<"Unable to copy velocity fields to Fluid Domain"<<"\n";
        throw e;
    }
    try
    {
        this->domain_array = new float[domain_class.size()]();
        std::copy(domain_class.begin(), domain_class.end(), this->domain_array);
    }
    catch(std::exception e)
    {
        throw std::overflow_error(std::string("Memory not available to assign for visualization of boundary"));
    }
    try
    {
        this->shader = Shader();
        this->shader.create_vs_shader(domain_ver_shader);
        this->shader.create_fs_shader(domain_frag_shader);
        this->shader.compile();
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
    }
    catch(std::exception e)
    {
        std::cout<<"Error while trying to create boundary visualization..."<<"\n";
        throw e;
    }
}

void Render::Domain::update_fields(float *vel_x, float *vel_y, float *vel_z)
{
    try
    {
        for(int k=0;k<NZ;k++)
        {
            for(int j=0;j<NY;j++)
            {
                for(int i=0;i<NX;i++)
                {
                    this->prev_vel_x[k*NX*NY+j*NX+i] = cur_vel_x[k*NX*NY+j*NX+i];
                    this->prev_vel_y[k*NX*NY+j*NX+i] = cur_vel_y[k*NX*NY+j*NX+i];
                    this->prev_vel_z[k*NX*NY+j*NX+i] = cur_vel_z[k*NX*NY+j*NX+i];
                    
                    this->cur_vel_x[k*NX*NY+j*NX+i] = vel_x[k*NX*NY+j*NX+i];
                    this->cur_vel_y[k*NX*NY+j*NX+i] = vel_y[k*NX*NY+j*NX+i];
                    this->cur_vel_z[k*NX*NY+j*NX+i] = vel_z[k*NX*NY+j*NX+i];
                }
            }
        }
    }
    catch(std::exception e)
    {
        std::cout<<"Unable to update velocity fields of Fluid Domain"<<"\n";
        throw e;
    }
}

Render::Domain::~Domain()
{
    try
    {
        delete[] this->cur_vel_x;
        delete[] this->cur_vel_y;
        delete[] this->cur_vel_z;

        delete[] this->prev_vel_x;
        delete[] this->prev_vel_y;
        delete[] this->prev_vel_z;

        delete[] this->domain_classification;
        delete[] this->domain_array;

        glDeleteVertexArrays(1, &this->VAO);
	    glDeleteBuffers(1, &this->VBO);
        std::cout<<"Boundary Domain deleted....."<<"\n";
    }
    catch(std::exception e)
    {
        std::cout<<"Unable to delete Boundary Domain"<<"\n";
    }
}

std::vector<std::vector<int>> Render::Domain::get_start_location()
{
    return this->start_position;
}

void Render::Domain::draw_domain()
{
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
    this->shader.use();
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    std::string model_id = "model_domain";
    std::string proj_id = "projection_domain";
    std::string view_id = "view_domain";
    glUniformMatrix4fv(glGetUniformLocation(this->shader.get_shader_pgm(), model_id.c_str()), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(this->shader.get_shader_pgm(), proj_id.c_str()), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(this->shader.get_shader_pgm(), view_id.c_str()), 1, GL_FALSE, &view[0][0]);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    
    glBufferData(GL_ARRAY_BUFFER, this->points*sizeof(float), this->domain_array, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)12);
    // glEnableVertexAttribArray(1);
    
    this->shader.use();
    glDrawArrays(GL_POINTS, 0, this->points);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
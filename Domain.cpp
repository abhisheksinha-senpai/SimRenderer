#include "Domain.hpp"

Render::Domain::Domain(float *vel_x, float *vel_y, float *vel_z, int *dom)
{
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
                }
            }
        }
    }
    catch(std::exception e)
    {
        std::cout<<"Unable to copy velocity fields to Fluid Domain"<<"\n";
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
#include <iostream>
#include <sstream>
#include <string.h>
#include <fstream>
#include <vector>
#include <cmath>
#include <unistd.h>

#include "Definitions.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Domain.hpp"
#include "Particle.hpp"

#define SIZE 131072

using namespace std;
using namespace Render;

void processInput(GLFWwindow *window);

std::vector<std::vector<float>>vel_field_data;
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int GL_MAJOR = 4;
const unsigned int GL_MINOR = 2;

const unsigned int NZ = 16;
const unsigned int NX = 512;
const unsigned int NY = 128;

void reader(int num, string file)
{
	vel_field_data.clear();
	std::string src = "/home/abhishek/Research/LBM/";
	std::ifstream File;
	File.clear(std::istream::eofbit | std::istream::failbit);

	cout<<src+file+to_string(num)+".csv"<<"\n";
	
    try
	{
		File.open(src+file+to_string(num)+".csv");
	}
	catch (std::ios_base::failure& e) {
		cout << "Error: " << strerror(errno);
		throw;
	}
	string line, word;
	int count = 0;
	while (getline(File, line))
	{
		stringstream s(line);
		vector<string> row;
		if(s.str().size()>0)
		{
			while (getline(s, word, ','))
			{
				row.push_back(word);
			}
			vector<float>tmp2;
			tmp2.push_back(std::stof(row[0])*(float)SCR_WIDTH/NX);
			tmp2.push_back(std::stof(row[1])*(float)SCR_HEIGHT/NY);
			tmp2.push_back(std::stof(row[2])*1.0f/NZ);
			tmp2.push_back(2.0f*(std::stof(row[5])/NX)-1.0f);
			tmp2.push_back(2.0f*(std::stof(row[4])/NY)-1.0f);
			tmp2.push_back(std::stof(row[3])/NZ);
			tmp2.push_back(std::stof(row[6]));
			vel_field_data.push_back(tmp2);
		}
	}
	cout<<vel_field_data.size()<<endl;
	File.close();
}

int main(int argc, char **argv)
{
	float *vel_x, *vel_y, *vel_z;
	int *bound;

	reader(50, "00_vel");
	cout<<"Velocity field updated....."<<"\n";

	vel_x = new float[vel_field_data.size()]();
	cout<<"Vel X field updated....."<<"\n";
	vel_y = new float[vel_field_data.size()]();
	cout<<"Vel Y field updated....."<<"\n";
	vel_z = new float[vel_field_data.size()]();
	cout<<"Vel Z field updated....."<<"\n";
	bound = new int[vel_field_data.size()]();
	cout<<"Boundary field updated....."<<"\n";
	
	try 
	{
		ResourceManager r_manager = ResourceManager();
		r_manager.load_shader("ShaderFiles/VertexShaders/particle_shader.vs", "VERTEX");
		r_manager.load_shader("ShaderFiles/FragmentShaders/particle_shader.fs", "FRAGMENT");
		cout<<"Resource manager done......."<<"\n";
		Renderer sim_render = Renderer(SCR_WIDTH, SCR_HEIGHT, GL_MAJOR, GL_MINOR);
		cout<<"Renderer done......."<<"\n";
		sim_render.create_windowed_context(framebuffer_resize_callback);
		cout<<"Window created......."<<"\n";
		Fluid fluid_field = Fluid(r_manager.get_vertex_shader().c_str(), r_manager.get_fragment_shader().c_str());
		cout<<"Fluid Domain done......."<<"\n";
		for(int i=0; i<vel_field_data.size(); i++)
		{
			vel_x[i] = vel_field_data[i][0];
			vel_y[i] = vel_field_data[i][1];
			vel_z[i] = vel_field_data[i][2];
			bound[i] = vel_field_data[i][6];
		}
		
		r_manager.load_shader("ShaderFiles/VertexShaders/domain_shader.vs", "VERTEX");
		r_manager.load_shader("ShaderFiles/FragmentShaders/domain_shader.fs", "FRAGMENT");
		Domain fluid_domain = Domain(vel_x, vel_y, vel_z, bound,r_manager.get_vertex_shader().c_str(), r_manager.get_fragment_shader().c_str());
		cout<<"Boundary creation done...."<<"\n";
		int timer = 0;
		int count_loops =0;
		while (!glfwWindowShouldClose(sim_render.get_windowed_context()))
		{
			// input
			// -----
			// std::cout<<"dead 1"<<"\n";
			// processInput(sim_render.get_windowed_context());
			// std::cout<<"dead 2"<<"\n";
			fluid_field.generate_fluid_particle(fluid_domain);
			// std::cout<<"dead 3"<<"\n";
			if(timer%10 == 0)
			{
				reader(100+(timer/10)*50, "00_vel");
				//  std::cout<<"dead 4"<<"\n";
				for(int i=0; i<vel_field_data.size(); i++)
				{
					vel_x[i] = vel_field_data[i][0];
					vel_y[i] = vel_field_data[i][1];
					vel_z[i] = vel_field_data[i][2];
				}
				// std::cout<<"dead 5"<<"\n";
				fluid_domain.update_fields(vel_x, vel_y, vel_z);
			}
			// std::cout<<"dead 6"<<"\n";
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			// std::cout<<"dead 7"<<"\n";
			glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
			fluid_field.advect_fluid_particle(stof(string(argv[1])), fluid_domain);
			std::cout<<"timer: "<<timer<<"\n";

			fluid_field.draw_fluid_particle();
			fluid_domain.draw_domain();
			
			// std::cout<<"dead 10"<<"\n";
			glfwSwapBuffers(sim_render.get_windowed_context());
			// std::cout<<"dead 11"<<"\n";
			if(timer==189)
				count_loops +=1;
			timer = (timer+1)%190;
			// std::cout<<"dead 12"<<"\n";
			glfwPollEvents();
			if(count_loops == 3)
				break;
			// std::cout<<"dead 13"<<"\n";
		}
	}
	catch (std::exception &e)
	{
		cout<<"Things went bad....LOL"<<"\n";
		cout<<e.what()<<"\n";
	}

	delete[] vel_x;
	delete[] vel_y;
	delete[] vel_z;
	delete[] bound;
	
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


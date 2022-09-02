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

std::vector<std::vector<float>>vertices_x;
std::vector<std::vector<float>>vertices_y;
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int GL_MAJOR = 4;
const unsigned int GL_MINOR = 0;

const unsigned int NZ=2;
const unsigned int NX = 512;
const unsigned int NY=128;

void reader(int num, string file, bool X)
{
	
	std::string src = "/home/abhishek/Research/LBM/";
	std::ifstream File;
	File.clear(std::istream::eofbit | std::istream::failbit);

	cout<<src+file+to_string(num)+".csv"<<"\n";
	if(X)
		vertices_x.clear();
	else
		vertices_y.clear();
	
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
			tmp2.push_back(std::stof(row[0]));
			tmp2.push_back(2*(std::stof(row[3])/NX)-1);
			tmp2.push_back(2*(std::stof(row[2])/NY)-1);
			tmp2.push_back(std::stof(row[1])/NZ);
			tmp2.push_back(std::stof(row[4]));
			if(X)
				vertices_x.push_back(tmp2);
			else 
				vertices_y.push_back(tmp2);
		}
	}
	if(X)
		cout<<vertices_x.size()<<"\n";
	else
		cout<<vertices_y.size()<<"\n";
	File.close();
}

int main()
{
	try 
	{
		ResourceManager r_manager = ResourceManager();
		r_manager.load_shader("particle_shader.vs", "VERTEX");
		r_manager.load_shader("particle_shader.fs", "FRAGMENT");
		cout<<"Resource manager done......."<<"\n";
		Renderer sim_render = Renderer(SCR_WIDTH, SCR_HEIGHT, GL_MAJOR, GL_MINOR);
		cout<<"Renderer done......."<<"\n";
		sim_render.create_windowed_context(framebuffer_resize_callback);
		cout<<"Window created......."<<"\n";
		Fluid fluid_field = Fluid(r_manager.get_vertex_shader().c_str(), r_manager.get_fragment_shader().c_str());
		cout<<"Fluid Domain done......."<<"\n";
		reader(50, "00_ux", true);
		reader(50, "00_uy", false);

		float vel_x[vertices_x.size()];
		float vel_y[vertices_x.size()];
		float vel_z[vertices_x.size()];
		int bound[vertices_x.size()];

		for(int i=0; i<vertices_x.size(); i++)
		{
			vel_x[i] = vertices_x[i][0]*SCR_WIDTH/NX;
			vel_y[i] = vertices_y[i][0]*SCR_HEIGHT/NY;
			vel_z[i] = 0;
			bound[i] = vertices_x[i][4];
		}
		
		Domain fluid_domain = Domain(vel_x, vel_y, vel_z, bound);
		cout<<"Boundary creation done...."<<"\n";
		int timer = 0;
		while (!glfwWindowShouldClose(sim_render.get_windowed_context()))
		{
			// input
			// -----
			// std::cout<<"dead 1"<<"\n";
			processInput(sim_render.get_windowed_context());
			// std::cout<<"dead 2"<<"\n";
			fluid_field.generate_fluid_particle(fluid_domain);
			// std::cout<<"dead 3"<<"\n";
			if(timer%10 == 0)
			{
				reader(100+(timer/10)*50, "00_ux", true);
				// std::cout<<"dead 4"<<"\n";
				reader(100+(timer/10)*50, "00_uy", false);
				for(int i=0; i<vertices_x.size(); i++)
				{
					vel_x[i] = vertices_x[i][0]*SCR_WIDTH/NX;
					vel_y[i] = vertices_y[i][0]*SCR_HEIGHT/NY;
					vel_z[i] = 0;
					bound[i] = vertices_x[i][4];
				}
				// std::cout<<"dead 5"<<"\n";
				fluid_domain.update_fields(vel_x, vel_y, vel_z);
			}
			// std::cout<<"dead 6"<<"\n";
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			// std::cout<<"dead 7"<<"\n";
			glClear(GL_COLOR_BUFFER_BIT);
			// std::cout<<"dead 8"<<"\n";
			fluid_field.advect_fluid_particle(0.1, fluid_domain);
			std::cout<<"timer: "<<timer<<"\n";
			fluid_field.draw_fluid_particle();
			// std::cout<<"dead 10"<<"\n";
			glfwSwapBuffers(sim_render.get_windowed_context());
			// std::cout<<"dead 11"<<"\n";
			timer = (timer+1)%190;
			// std::cout<<"dead 12"<<"\n";
			glfwPollEvents();
			// std::cout<<"dead 13"<<"\n";
		}
	}
	catch (std::exception &e)
	{
		cout<<"Things went bad....LOL"<<"\n";
		cout<<e.what()<<"\n";
	}
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


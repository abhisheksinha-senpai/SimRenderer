#version 420 core
in vec4 fluid_particle_color;
out vec4 FragColor;

void main()
{
   FragColor = fluid_particle_color;
}
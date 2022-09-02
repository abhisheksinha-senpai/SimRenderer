#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 fluid_particle_color;

void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   fluid_particle_color = aColor;
}
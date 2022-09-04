#include "ResourceManager.hpp"

ReturnType Render::ResourceManager::load_shader(std::string file_name, std::string shader_type)
{
    try
    {
        std::ifstream shader_file;
        shader_file.clear(std::istream::eofbit | std::istream::failbit);
        try
        {
            shader_file.open(file_name);
        }
        catch (std::ios_base::failure& e) {
            std::cout << "Error: " << strerror(errno);
            throw;
        }

        std::stringstream shader;
        shader << shader_file.rdbuf();
        shader_file.close();
        if(shader_type == "VERTEX")
            this->vertex_shader = shader.str();
        else if(shader_type == "FRAGMENT")
            this->fragment_shader = shader.str();
       
    }
    catch(std::exception e)
    {
        throw std::runtime_error(std::string("ERROR::SHADER: Failed to read shader files"));
    }
   
    return SUCCESS;
}

std::string Render::ResourceManager::get_vertex_shader()
{
    return this->vertex_shader;
}
std::string  Render::ResourceManager::get_fragment_shader()
{
    return this->fragment_shader;
}
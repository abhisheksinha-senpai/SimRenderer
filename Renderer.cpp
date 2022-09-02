#include "Renderer.hpp"

Render::Renderer::Renderer(int width, int height, int gl_major, int gl_minor)
:screen_width(width), screen_height(height)
{
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
}

ReturnType Render::Renderer::create_windowed_context(void(*framebuffer_size_callback)(GLFWwindow *, int, int))
{
    this->window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", NULL, NULL);
	if (this->window == NULL)
		throw std::runtime_error("Failed to create GLFW window\n");
	
	glfwMakeContextCurrent(this->window);
    glfwSetFramebufferSizeCallback(this->window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("Failed to initialize GLAD");

    return SUCCESS;
}

GLFWwindow* Render::Renderer::get_windowed_context()
{
	return this->window;
}
#ifndef RENDERER_H
#define RENDERER_H

#include <functional>
#include <iostream>

#include "ExternalSrc/Include/glad/glad.h"
#include "ExternalSrc/Include/GLFW/glfw3.h"
#include "ExternalSrc/Include/glm/glm.hpp"
#include "Definitions.hpp"


namespace Render
{
    inline void framebuffer_resize_callback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    class Renderer
    {
        private:
        int screen_width = 800;
        int screen_height = 600;
        GLFWwindow* window;

        public:
        Renderer(int width, int height, int gl_major, int gl_minor);
        ReturnType create_windowed_context(void(*framebuffer_size_callback)(GLFWwindow *, int, int));
        GLFWwindow* get_windowed_context();
    };
}

#endif
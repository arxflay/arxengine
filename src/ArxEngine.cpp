#include "ArxEngine.h"
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>

ARX_NAMESPACE_BEGIN

bool InitializeGlad()
{
    std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> tempWin(glfwCreateWindow(1, 1, "", nullptr, nullptr), glfwDestroyWindow);
    glfwMakeContextCurrent(tempWin.get());
    bool ret = !!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(&glfwGetProcAddress));
    glfwMakeContextCurrent(nullptr);
    
    return ret;
}

int InitializeArxEngine()
{
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, defaults::GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, defaults::GL_VERSION_MINOR);

    if (!InitializeGlad())
        return -2;

    return 0;
}

void DeinitializeArxEngine()
{
    glfwTerminate();
}

void PollEvents()
{
    glfwPollEvents();
}

ARX_NAMESPACE_END

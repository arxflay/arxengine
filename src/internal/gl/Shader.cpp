#include "internal/gl/Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "logging/Logger.h"
#include <utility>

ARX_NAMESPACE_BEGIN

std::string LoadFile(std::string_view filename)
{
    std::stringstream stream;
    std::ifstream file;
    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    file.open(filename.data());
    stream << file.rdbuf();
    file.close();
    return stream.str();
}

enum ShaderType : unsigned int
{
    VertexShader = GL_VERTEX_SHADER,
    FragmentShader = GL_FRAGMENT_SHADER,
    GeometryShader = GL_GEOMETRY_SHADER
};

std::string_view ShaderTypeToString(ShaderType shaderType)
{
    switch (shaderType) 
    {
        case VertexShader:
            return "Vertex shader";
        case FragmentShader:
            return "Fragment shader";
        case GeometryShader:
            return "Geometry shader";
    }

    return "";
}

unsigned int CreateAndCompileShader(ShaderType shaderType, std::string_view data)
{
    unsigned int shader = glCreateShader(shaderType);
    const char *cstrData = data.data();
    glShaderSource(shader, 1, &cstrData, nullptr);
    glCompileShader(shader);

    int status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    char buffer[256] = {};
    
    if (!status)
    {
        glGetShaderInfoLog(shader, sizeof(buffer), nullptr, buffer);
        GLOG->Error("%s %s: %s", "Failed to compile ", ShaderTypeToString(shaderType).data(), buffer);
    }
    return shader;
}

unsigned int CreateAndLinkProgram(unsigned int vertexShader, unsigned int fragmentShader, std::optional<unsigned int> geometryShader)
{
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    if (geometryShader.has_value())
        glAttachShader(program, geometryShader.value());

    glLinkProgram(program);
    
    int status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    char buffer[256] = {};
    if (!status)
    {
        glGetProgramInfoLog(program, sizeof(buffer), nullptr, buffer);
        GLOG->Error("%s: %s", "Failed to link program", buffer);
    }

    return program;
}

Shader::~Shader()
{
    glDeleteProgram(m_programHandle);
}

/*static*/ Shader Shader::FromFiles(std::string_view vertexShaderFile, std::string_view fragmentShaderFile, std::optional<std::string_view> geometryShaderFile)
{
    std::string vertexShaderData = LoadFile(vertexShaderFile);
    std::string fragmentShaderData = LoadFile(fragmentShaderFile);
    std::optional<std::string> geometryShaderData = std::nullopt;
    if (geometryShaderFile.has_value())
        geometryShaderData = LoadFile(geometryShaderFile.value());

    return FromData(vertexShaderData, fragmentShaderData, geometryShaderData);
}

/*static*/ Shader Shader::FromData(std::string_view vertexShaderData, std::string_view fragmentShaderData, std::optional<std::string_view> geometryShaderData)
{
    Shader shader;

    unsigned int vertexShader = CreateAndCompileShader(ShaderType::VertexShader, vertexShaderData); 
    unsigned int fragmentShader = CreateAndCompileShader(ShaderType::FragmentShader, fragmentShaderData);
    std::optional<unsigned int> geometryShader = std::nullopt;
    if (geometryShaderData.has_value())
        geometryShader = CreateAndCompileShader(ShaderType::GeometryShader, geometryShaderData.value());
    shader.m_programHandle = CreateAndLinkProgram(vertexShader, fragmentShader, geometryShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (geometryShader.has_value())
        glDeleteShader(geometryShader.value());

    return shader;
}

Shader::Shader()
    : m_programHandle(0)
{
}

Shader::Shader(Shader &&shader) 
{
    m_programHandle = std::exchange(shader.m_programHandle, 0);
}

Shader &Shader::operator=(Shader &&shader)
{
    std::swap(m_programHandle, shader.m_programHandle);
    return *this;
}

void Shader::SetUniformInt(std::string_view name, int num)
{
    glUniform1i(glGetUniformLocation(m_programHandle, name.data()), num);
}

void Shader::SetUniformFloat(std::string_view name, float num)
{
    glUniform1f(glGetUniformLocation(m_programHandle, name.data()), num);
}

void Shader::SetUniformVec4(std::string_view name, const glm::vec4 &vec)
{
    glUniform4fv(glGetUniformLocation(m_programHandle, name.data()), 1, glm::value_ptr(vec));
}

void Shader::SetUniformMat4(std::string_view name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(m_programHandle, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}

unsigned int Shader::GetProgramHandle()
{
    return m_programHandle;
}

void Shader::UseShader()
{
    glUseProgram(m_programHandle);
}

ARX_NAMESPACE_END

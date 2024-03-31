#include "arxengine/internal/gl/ShaderImpl.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "arxengine/logging/Logger.h"
#include "arxengine/misc/Utils.h"
#include <utility>

ARX_NAMESPACE_BEGIN

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

unsigned int LinkProgram(unsigned int program, unsigned int vertexShader, unsigned int fragmentShader, std::optional<unsigned int> geometryShader)
{
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

ShaderImpl::~ShaderImpl()
{
    if (m_programHandle != 0)
        glDeleteProgram(m_programHandle);
}

void ShaderImpl::LoadFromFiles(std::string_view vertexShaderFile, std::string_view fragmentShaderFile, std::optional<std::string_view> geometryShaderFile)
{
    std::string vertexShaderData;
    Utils::LoadFile(vertexShaderFile, vertexShaderData);
    std::string fragmentShaderData;
    Utils::LoadFile(fragmentShaderFile, vertexShaderData);

    std::optional<std::string> geometryShaderData = std::nullopt;
    if (geometryShaderFile.has_value())
    {
        geometryShaderData.emplace();
        Utils::LoadFile(geometryShaderFile.value(), geometryShaderData.value());
    }

    LoadFromData(vertexShaderData, fragmentShaderData, geometryShaderData);
}

void ShaderImpl::LoadFromData(std::string_view vertexShaderData, std::string_view fragmentShaderData, std::optional<std::string_view> geometryShaderData)
{
    unsigned int vertexShader = CreateAndCompileShader(ShaderType::VertexShader, vertexShaderData); 
    unsigned int fragmentShader = CreateAndCompileShader(ShaderType::FragmentShader, fragmentShaderData);
    std::optional<unsigned int> geometryShader = std::nullopt;
    if (geometryShaderData.has_value())
        geometryShader = CreateAndCompileShader(ShaderType::GeometryShader, geometryShaderData.value());
    if (m_programHandle == 0)
        m_programHandle = glCreateProgram();

    LinkProgram(m_programHandle, vertexShader, fragmentShader, geometryShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (geometryShader.has_value())
        glDeleteShader(geometryShader.value());
}

ShaderImpl::ShaderImpl()
    : m_programHandle(0)
{
}

ShaderImpl::ShaderImpl(ShaderImpl &&shader) 
{
    m_programHandle = std::exchange(shader.m_programHandle, 0);
}

ShaderImpl &ShaderImpl::operator=(ShaderImpl &&shader)
{
    std::swap(m_programHandle, shader.m_programHandle);
    return *this;
}

void ShaderImpl::SetUniformInt(std::string_view name, int num)
{
    glUniform1i(glGetUniformLocation(m_programHandle, name.data()), num);
}

void ShaderImpl::SetUniformFloat(std::string_view name, float num)
{
    glUniform1f(glGetUniformLocation(m_programHandle, name.data()), num);
}

void ShaderImpl::SetUniformVec4(std::string_view name, const glm::vec4 &vec)
{
    glUniform4fv(glGetUniformLocation(m_programHandle, name.data()), 1, glm::value_ptr(vec));
}

void ShaderImpl::SetUniformMat4(std::string_view name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(m_programHandle, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderImpl::SetTransformMatrices(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
{
   SetUniformMat4(MODEL_MATRIX_NAME, modelMatrix);
   SetUniformMat4(VIEW_MATRIX_NAME, viewMatrix);
   SetUniformMat4(PROJECTION_MATRIX_NAME, projectionMatrix);
}

unsigned int ShaderImpl::GetProgramHandle()
{
    return m_programHandle;
}

void ShaderImpl::UseShader()
{
    glUseProgram(m_programHandle);
}

ARX_NAMESPACE_END

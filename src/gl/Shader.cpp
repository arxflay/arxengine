#include "arxengine/gl/Shader.h"
#include "arxengine/internal/gl/ShaderImpl.h"
ARX_NAMESPACE_BEGIN


Shader::Shader(UIControl *control)
    : UIObject(control) 
    , m_impl(std::make_unique<ShaderImpl>())
{
}

void Shader::SetUniformInt(std::string_view name, int num)
{
    m_impl->SetUniformInt(name, num);
}

void Shader::SetUniformFloat(std::string_view name, float num)
{
    m_impl->SetUniformFloat(name, num);
}

void Shader::SetUniformVec4(std::string_view name, const glm::vec4 &vec)
{
    m_impl->SetUniformVec4(name, vec);
}

void Shader::SetUniformMat4(std::string_view name, const glm::mat4 &mat)
{
    m_impl->SetUniformMat4(name, mat);
}

void Shader::SetTransformMatrices(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
{
    m_impl->SetTransformMatrices(modelMatrix, viewMatrix, projectionMatrix);
}

void Shader::UseShader()
{
    m_impl->UseShader();
}

void Shader::LoadFromFiles(std::string_view vertexShaderImplFile, std::string_view fragmentShaderImplFile, std::optional<std::string_view> geometryShaderImplFile)
{
    m_impl->LoadFromFiles(vertexShaderImplFile, fragmentShaderImplFile, geometryShaderImplFile);
}

void Shader::LoadFromData(std::string_view vertexShaderImplData, std::string_view fragmentShaderImplData, std::optional<std::string_view> geometryShaderImplData)
{
    m_impl->LoadFromData(vertexShaderImplData, fragmentShaderImplData, geometryShaderImplData);
}

ARX_NAMESPACE_END


#pragma once
#include "ArxDefines.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <string_view>
#include <optional>

ARX_NAMESPACE_BEGIN

class Shader final
{
public:
    static Shader FromFiles(std::string_view vertexShaderFile, std::string_view fragmentShaderFile, std::optional<std::string_view> geometryShaderFile);
    static Shader FromData(std::string_view vertexShaderData, std::string_view fragmentShaderData, std::optional<std::string_view> geometryShaderData);
    
    void SetUniformInt(std::string_view name, int num);
    void SetUniformFloat(std::string_view name, float num);
    void SetUniformVec4(std::string_view name, const glm::vec4 &vec);
    void SetUniformMat4(std::string_view name, const glm::mat4 &mat);
    
    unsigned int GetProgramHandle();
    void UseShader();
    ~Shader();

    Shader();

    //context unsafe
    Shader(Shader&&);
    Shader &operator=(Shader&&);

private:
    Shader(const Shader&) = delete;
    Shader &operator=(const Shader&) = delete;
    unsigned int m_programHandle;
};

ARX_NAMESPACE_END

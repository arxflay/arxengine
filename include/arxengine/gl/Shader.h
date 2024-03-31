#ifndef ARX_SHADER_H
#define ARX_SHADER_H

#include "arxengine/ui/UIObject.h"
#include "arxengine/gl/IShader.h"
ARX_NAMESPACE_BEGIN

class ShaderImpl;

class Shader final : public UIObject, public IShader 
{
public:
    Shader(UIControl *control);
    void LoadFromFiles(std::string_view vertexShaderImplFile, std::string_view fragmentShaderImplFile, std::optional<std::string_view> geometryShaderImplFile);
    void LoadFromData(std::string_view vertexShaderImplData, std::string_view fragmentShaderImplData, std::optional<std::string_view> geometryShaderImplData);
    void SetUniformInt(std::string_view name, int num) override;
    void SetUniformFloat(std::string_view name, float num) override;
    void SetUniformVec4(std::string_view name, const glm::vec4 &vec) override;
    void SetUniformMat4(std::string_view name, const glm::mat4 &mat) override;

    //all of them must have same name in shader!
    void SetTransformMatrices(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;
    
    void UseShader() override;

private:
    std::unique_ptr<ShaderImpl> m_impl;
};

ARX_NAMESPACE_END

#endif

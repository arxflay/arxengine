/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_SHADERIMPL_H
#define ARX_SHADERIMPL_H
#include "arxengine/ArxDefines.h"
#include "arxengine/gl/IShader.h"
#include <string_view>
#include <optional>

ARX_NAMESPACE_BEGIN

#define PROJECTION_MATRIX_NAME "projectionMatrix"
#define VIEW_MATRIX_NAME "viewMatrix"
#define MODEL_MATRIX_NAME "modelMatrix"

class ShaderImpl final : public IShader 
{
public:
    void LoadFromFiles(std::string_view vertexShaderImplFile, std::string_view fragmentShaderImplFile, std::optional<std::string_view> geometryShaderImplFile);
    void LoadFromData(std::string_view vertexShaderImplData, std::string_view fragmentShaderImplData, std::optional<std::string_view> geometryShaderImplData);
    
    void SetUniformInt(std::string_view name, int num) override;
    void SetUniformFloat(std::string_view name, float num) override;
    void SetUniformVec4(std::string_view name, const glm::vec4 &vec) override;
    void SetUniformMat4(std::string_view name, const glm::mat4 &mat) override;

    //all of them must have same name in shader!
    void SetTransformMatrices(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) override;
    
    unsigned int GetProgramHandle();
    void UseShader() override;
    ~ShaderImpl();

    ShaderImpl();

    //context unsafe
    ShaderImpl(ShaderImpl&&);
    ShaderImpl &operator=(ShaderImpl&&);

private:
    ShaderImpl(const ShaderImpl&) = delete;
    ShaderImpl &operator=(const ShaderImpl&) = delete;
    unsigned int m_programHandle;
};

ARX_NAMESPACE_END

#endif

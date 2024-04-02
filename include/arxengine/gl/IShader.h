#ifndef ARX_ISHADER_H
#define ARX_ISHADER_H

#include <string_view>
#include "arxengine/ArxDefines.h"
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS IShader
{
public:
    virtual ~IShader() = default;
    virtual void SetUniformInt(std::string_view name, int num) = 0;
    virtual void SetUniformFloat(std::string_view name, float num) = 0;
    virtual void SetUniformVec4(std::string_view name, const glm::vec4 &vec) = 0;
    virtual void SetUniformMat4(std::string_view name, const glm::mat4 &mat) = 0;

    //all of them must have same name in shader!
    virtual void SetTransformMatrices(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) = 0;
    virtual void UseShader() = 0;
};

ARX_NAMESPACE_END

#endif

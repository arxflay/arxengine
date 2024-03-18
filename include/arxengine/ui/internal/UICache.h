/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_UICACHE_H
#define ARX_UICACHE_H
#include "arxengine/ArxDefines.h"
#include "arxengine/internal/gl/VAO.h"
#include "arxengine/internal/gl/VBO.h"
#include "arxengine/internal/gl/Shader.h"
#include <map>
#include <string>
#include <array>
ARX_NAMESPACE_BEGIN

class UICache final
{
public:
    UICache();

    enum class VAO_ID
    {
        RECTANGLE
    };
    
    enum class VBO_ID
    {
        RECTANGLE
    };

    enum class SHADER_PROGRAM_ID
    {
        RECTANGLE,
        IMAGE, /*RECT VAO AND VBO*/
        TEXT /*RECT VAO AND VBO*/
    };

    using VAOMap = std::map<VAO_ID, VAO>;
    using VBOMap = std::map<VBO_ID, VBO>;
    using ShaderMap = std::map<SHADER_PROGRAM_ID, Shader>;

    VAOMap &GetVAOMap()
    {
        return m_vaos;
    }

    VBOMap &GetVBOMap()
    {
        return m_vbos;
    }

    ShaderMap &GetShaderMap()
    {
        return m_shaderPrograms;
    }

    void Init();


private:
    void InitRectangleData();
    void InitImageData();
    void InitTextData();

    bool m_initialized;
    VAOMap m_vaos; 
    VBOMap m_vbos;
    ShaderMap m_shaderPrograms;
};

ARX_NAMESPACE_END

#endif

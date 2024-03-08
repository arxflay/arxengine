#include "ui/internal/UICache.h"
#include <iostream>
ARX_NAMESPACE_BEGIN

UICache::UICache()
    : m_initialized(false)
{
}

constexpr std::string_view RECTANGLE_VERTEX_SHADER = R"(
    #version 330 core
    layout (location = 0) in vec2 Pos;
    uniform mat4 )" MODEL_MATRIX_NAME R"(;
    uniform mat4 )" VIEW_MATRIX_NAME R"(;
    uniform mat4 )" PROJECTION_MATRIX_NAME R"(;
    void main()
    {
        gl_Position = )" PROJECTION_MATRIX_NAME "*" VIEW_MATRIX_NAME "*" MODEL_MATRIX_NAME R"(* vec4(Pos, 0.0f, 1.0f);
    }
)";

constexpr std::string_view RECTANGLE_FRAGMENT_SHADER = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec4 color;
    void main()
    {
        FragColor = color;
    }
)";

void UICache::InitRectangleData()
{
    OldVAOGuard vaoGuard;
    OldVBOGuard vboGuard;
    VAO vao = VAO::Create();
    vao.Bind();
    std::array verticies{
        0.0f,  1.0f,  
        0.0f,  0.0f,
        1.0f,  0.0f, 
        1.0f,  0.0f,          
        1.0f,  1.0f,   
        0.0f,  1.0f
    };
    VBO vbo = VBO::Create();
    vbo.Bind();
    vbo.SetData(verticies.begin(), verticies.end());
    vbo.SetVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    vbo.EnableVertexAttribPointer(0);
    Shader shader = Shader::FromData(RECTANGLE_VERTEX_SHADER, RECTANGLE_FRAGMENT_SHADER, std::nullopt);
    m_vaos.insert(std::make_pair(VAO_ID::RECTANGLE, std::move(vao)));
    m_vbos.insert(std::make_pair(VBO_ID::RECTANGLE, std::move(vbo)));
    m_shaderPrograms.insert(std::make_pair(SHADER_PROGRAM_ID::RECTANGLE, std::move(shader)));
}

constexpr std::string_view IMAGE_VERTEX_SHADER = R"(
    #version 330 core
    layout (location = 0) in vec2 Pos;
    uniform mat4 )" MODEL_MATRIX_NAME R"(;
    uniform mat4 )" VIEW_MATRIX_NAME R"(;
    uniform mat4 )" PROJECTION_MATRIX_NAME R"(;
    out vec2 TexPos;
    void main()
    {
        gl_Position = )" PROJECTION_MATRIX_NAME "*" VIEW_MATRIX_NAME "*" MODEL_MATRIX_NAME R"(* vec4(Pos, 0.0f, 1.0f);
        TexPos = Pos;
    }
)";

constexpr std::string_view IMAGE_FRAGMENT_SHADER = R"(
    #version 330 core
    in vec2 TexPos;
    out vec4 FragColor;
    uniform sampler2D imageTex;
    void main()
    {
        FragColor = texture(imageTex, TexPos);
    }
)";


void UICache::InitImageData()
{
    Shader shader = Shader::FromData(IMAGE_VERTEX_SHADER, IMAGE_FRAGMENT_SHADER, std::nullopt);
    m_shaderPrograms.insert(std::make_pair(SHADER_PROGRAM_ID::IMAGE, std::move(shader)));
}

constexpr std::string_view TEXT_FRAGMENT_SHADER = R"(
    #version 330 core
    in vec2 TexPos;
    out vec4 FragColor;
    uniform sampler2D imageTex;
    uniform vec4 color;
    void main()
    {
        vec2 inversedTexPos = vec2(TexPos.x, 1 - TexPos.y);
        float intesity = texture(imageTex, inversedTexPos).r;
        FragColor = intesity * color;
    }
)";

void UICache::InitTextData()
{
    Shader shader = Shader::FromData(IMAGE_VERTEX_SHADER, TEXT_FRAGMENT_SHADER, std::nullopt);
    m_shaderPrograms.insert(std::make_pair(SHADER_PROGRAM_ID::TEXT, std::move(shader)));
}

void UICache::Init()
{
    if (!m_initialized)
    {
        InitRectangleData();
        InitImageData();
        InitTextData();
        m_initialized = true;
    }
}

ARX_NAMESPACE_END

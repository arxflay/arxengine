#include "internal/gl/VAO.h"
#include <utility>
ARX_NAMESPACE_BEGIN

OldVAOGuard::OldVAOGuard()
    : m_vao(VAO::GetCurrentlyBoundVAO())
{

}

OldVAOGuard::~OldVAOGuard()
{
    glBindVertexArray(m_vao);
}


/*static*/ VAO VAO::Create()
{
    VAO vao;
    glGenVertexArrays(1, &vao.m_vao);
    return vao;
}

/*static*/ std::vector<VAO> VAO::CreateMultiple(size_t count)
{
    std::vector<GLuint> vaosGL;
    vaosGL.resize(count);
    glGenVertexArrays(static_cast<GLsizei>(count), vaosGL.data());

    std::vector<VAO> vaos;
    vaos.resize(count);
    
    for (size_t i = 0; i < vaosGL.size(); i++)
        vaos[i].m_vao = vaosGL[i];
 
    return vaos;
}

void VAO::Bind()
{
    glBindVertexArray(m_vao);
}

void VAO::Unbind()
{
    if (VAO::GetCurrentlyBoundVAO() == m_vao)
        glBindVertexArray(0);
}

//void SetData(void *data, size_t count, BufferUsage usage = BufferUsage::Stream);

VAO::~VAO()
{
    if (!IsInvalid())
        glDeleteVertexArrays(1, &m_vao);
}

VAO::VAO()
    : m_vao(0)
{
}

/*static*/ GLuint VAO::GetCurrentlyBoundVAO()
{
    GLint vao = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
    return static_cast<GLuint>(vao);
}

VAO::VAO(VAO &&vao)
{
    m_vao = std::exchange(vao.m_vao, 0);
}

VAO &VAO::operator=(VAO &&vao)
{
    std::swap(m_vao, vao.m_vao);
    return *this;
}

bool VAO::IsInvalid()
{
    return m_vao == 0;
}

ARX_NAMESPACE_END

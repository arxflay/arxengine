#include "internal/gl/VBO.h"
#include <utility>

ARX_NAMESPACE_BEGIN

OldVBOGuard::OldVBOGuard()
    : m_vbo(VBO::GetCurrentlyBoundVBO())
{
}

OldVBOGuard::~OldVBOGuard()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

/*static*/ VBO VBO::Create()
{
    VBO vbo;
    glGenBuffers(1, &vbo.m_vbo);
    return vbo;
}
/*static*/ std::vector<VBO> VBO::CreateMultiple(size_t count)
{
    std::vector<GLuint> vbosGL;
    vbosGL.resize(count);
    glGenBuffers(static_cast<GLsizei>(count), vbosGL.data());

    std::vector<VBO> vbos;
    vbos.resize(count);
    
    for (size_t i = 0; i < vbos.size(); i++)
        vbos[i].m_vbo = vbosGL[i];
 
    return vbos;
}

void VBO::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

void VBO::Unbind()
{
    if (VBO::GetCurrentlyBoundVBO() == m_vbo)
        glBindBuffer(GL_ARRAY_BUFFER, 0);
}


//cleanup
VBO::~VBO()
{
    if (!IsInvalid())
        glDeleteBuffers(1, &m_vbo);
}


VBO::VBO()
    : m_vbo(0)
{
}

/*static*/ GLuint VBO::GetCurrentlyBoundVBO()
{
    GLint vbo;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
    return static_cast<GLuint>(vbo);
}

void VBO::SetVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset)
{
    OldVBOGuard guard; 
    glVertexAttribPointer(index, size, type, normalized, stride, (void*)offset);
}

void VBO::EnableVertexAttribPointer(GLuint index)
{
    OldVBOGuard guard;
    glEnableVertexAttribArray(index);
}

VBO::VBO(VBO &&vbo)
{
    m_vbo = std::exchange(vbo.m_vbo, 0);
}
    
VBO &VBO::operator=(VBO &&vbo)
{
    std::swap(m_vbo, vbo.m_vbo);
    return *this;
}

bool VBO::IsInvalid()
{
    return m_vbo == 0;
}

ARX_NAMESPACE_END



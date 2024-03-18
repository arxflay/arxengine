#ifndef ARX_VBO_H
#define ARX_VBO_H
#include "arxengine/ArxDefines.h"
#include <glad/glad.h>
#include <vector>
#include <type_traits>

ARX_NAMESPACE_BEGIN

//sets Gets old VBO and sets its as current VBO after it gets out of scope
class OldVBOGuard final
{
public:
    OldVBOGuard();
    ~OldVBOGuard();
private:
    GLuint m_vbo;
};

class VBO final
{
public:
    enum class BufferUsage : unsigned int
    {
        Stream = GL_STREAM_DRAW, //The data store contents will be modified once and used at most a few times.
        Static = GL_STATIC_DRAW, //The data store contents will be modified once and used many times.
        Dynamic = GL_DYNAMIC_DRAW //The data store contents will be modified repeatedly and used many times.
    };

    static VBO Create();
    static std::vector<VBO> CreateMultiple(size_t count);

    void Bind();
    void Unbind();
    
    template<typename It>
    std::enable_if_t<std::is_same_v<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>>
    SetData(It begin, It end, BufferUsage usage = BufferUsage::Stream)
    {
        OldVBOGuard guard;
        Bind();
        auto elementSize = sizeof(typename std::iterator_traits<It>::value_type);
        auto count = std::distance(begin, end);
        glBufferData(GL_ARRAY_BUFFER, elementSize * count, &(*begin), static_cast<GLenum>(usage)); 
    }

    static GLuint GetCurrentlyBoundVBO();
    void SetVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset);
    void EnableVertexAttribPointer(GLuint index);
    
    VBO();

    //cleanup
    ~VBO();

    //context unsafe
    VBO(VBO&&);
    VBO &operator=(VBO&&);

    bool IsInvalid(); 
private:
    VBO(const VBO&) = delete;
    VBO &operator=(const VBO&) = delete;

    unsigned int m_vbo;
};


ARX_NAMESPACE_END
#endif

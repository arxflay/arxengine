#ifndef ARX_VAO_H
#define ARX_VAO_H
#include "arxengine/ArxDefines.h"
#include <glad/glad.h>
#include <vector>
ARX_NAMESPACE_BEGIN

//sets Gets old VAO and sets its as current VAO after it gets out of scope
class OldVAOGuard final
{
public:
    OldVAOGuard();
    ~OldVAOGuard();
private:
    GLuint m_vao;
};

class VAO final
{
public:
    static VAO Create();
    static std::vector<VAO> CreateMultiple(size_t count);

    void Bind();
    void Unbind();

    static GLuint GetCurrentlyBoundVAO();
    
    VAO();

    //cleanup
    ~VAO();
    
    //context unsafe
    
    VAO(VAO&&);
    VAO &operator=(VAO&&);

    bool IsInvalid(); 
private:
    VAO(const VAO&) = delete;
    VAO &operator=(const VAO&) = delete;

    GLuint m_vao;
};

ARX_NAMESPACE_END

#endif

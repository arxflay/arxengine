#ifndef ARX_IMAGE_H
#define ARX_IMAGE_H
#include "ArxDefines.h"

#include <glm/vec2.hpp>
#include <string_view>
#include <vector>

ARX_NAMESPACE_BEGIN

class Image final
{
public:
    bool operator==(const Image &img) const;

    glm::ivec2 GetSize() const;
    int GetColorChannels() const;
    const std::vector<uint8_t> &GetData() const;
    
    int GetGLColorChannels() const;
    bool IsInvalid() const;
    
    //jpeg, png, gif
    static Image LoadFromFile(std::string_view filename, bool flipVerticaly = true);
    static Image LoadFromBinary(const std::vector<uint8_t> &binaryData, bool flipVerticaly = true);
private:
    Image();
    glm::ivec2 m_size;
    int m_colorChannels;
    std::vector<uint8_t> m_data;
};


ARX_NAMESPACE_END

#endif

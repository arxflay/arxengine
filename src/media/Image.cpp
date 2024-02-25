#include "media/Image.h"
#define STB_IMAGE_IMPLEMENTATION

#include "misc/WarningSupressionHelpers.h"

WARNING_PUSH
#ifdef _MSC_VER
    WARNING_DISABLE(4244)
#else
    WARNING_DISABLE(-Wconversion)
#endif
#include <stb_image.h>
WARNING_POP

#include <memory>
#include <glad/glad.h>

ARX_NAMESPACE_BEGIN

bool Image::operator==(const Image &img) const
{
    return (GetData() == img.GetData()) && (GetSize() == img.GetSize()) && (GetColorChannels() == img.GetColorChannels());
}

glm::ivec2 Image::GetSize() const { return m_size; }
int Image::GetColorChannels() const { return m_colorChannels; }
const std::vector<uint8_t> &Image::GetData() const { return m_data; }

int Image::GetGLColorChannels() const
{
    switch (m_colorChannels)
    {
        case 1:
            return GL_RED;
        case 2:
            return GL_RG;
        case 3:
            return GL_RGB;
        case 4:
            return GL_RGBA;
        default:
            return GL_INVALID_VALUE;
    }
}

bool Image::IsInvalid() const
{
    return m_data.empty() || !m_size.x || !m_size.y || GetGLColorChannels() == GL_INVALID_VALUE;
}

/*static*/ Image Image::LoadFromFile(std::string_view filename, bool flipVerticaly)
{
    stbi_set_flip_vertically_on_load(flipVerticaly);
    Image img;
    int width = 0, height = 0;
    std::unique_ptr<void, void(*)(void*)> data{ stbi_load(filename.data(), &width, &height, &img.m_colorChannels, 0), stbi_image_free };
    const uint8_t *dataBytePtr = reinterpret_cast<uint8_t*>(data.get());
    img.m_size = glm::ivec2(width, height);
    img.m_data.insert(img.m_data.end(), dataBytePtr, dataBytePtr + (width * height));
    return img;
}

/*static*/ Image Image::LoadFromBinary(const std::vector<uint8_t> &binaryData, bool flipVerticaly)
{
    stbi_set_flip_vertically_on_load(flipVerticaly);
    Image img;
    int width = 0, height = 0;
    std::unique_ptr<void, void(*)(void*)> data{ stbi_load_from_memory(binaryData.data(), static_cast<int>(binaryData.size()), &width, &height, &img.m_colorChannels, 0), stbi_image_free };
    const uint8_t *dataBytePtr = reinterpret_cast<uint8_t*>(data.get());
    img.m_size = glm::ivec2(width, height);
    img.m_data.insert(img.m_data.end(), dataBytePtr, dataBytePtr + (width * height));
    return img;

}

Image::Image()
    : m_size(0, 0), m_colorChannels(0)
{
}

ARX_NAMESPACE_END

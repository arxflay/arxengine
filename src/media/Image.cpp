#include "media/Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>

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
#include <iostream>

ARX_NAMESPACE_BEGIN

bool Image::operator==(const Image &img) const
{
    return (GetData() == img.GetData()) && (GetSize() == img.GetSize()) && (GetColorChannels() == img.GetColorChannels());
}

Size Image::GetSize() const { return m_size; }
int Image::GetColorChannels() const { return m_colorChannels; }
const std::vector<uint8_t> &Image::GetData() const { return m_data; }



bool Image::IsInvalid() const
{
    return m_data.empty() || m_size == Size(0, 0) || m_colorChannels > 4 || m_colorChannels < 0;
}

/*static*/ Image Image::LoadFromFile(std::string_view filename, bool flipVerticaly)
{
    stbi_set_flip_vertically_on_load(flipVerticaly);
    Image img;
    int width = 0, height = 0;
    std::unique_ptr<void, void(*)(void*)> data{ stbi_load(filename.data(), &width, &height, &img.m_colorChannels, 0), stbi_image_free };
    const uint8_t *dataBytePtr = reinterpret_cast<uint8_t*>(data.get());
    img.m_size = Size(width, height);
    img.m_data.insert(img.m_data.end(), dataBytePtr, dataBytePtr + (width * height) * img.m_colorChannels);
    return img;
}

/*static*/ Image Image::LoadFromBinary(const std::vector<uint8_t> &binaryData, bool flipVerticaly)
{
    stbi_set_flip_vertically_on_load(flipVerticaly);
    Image img;
    int width = 0, height = 0;
    std::unique_ptr<void, void(*)(void*)> data{ stbi_load_from_memory(binaryData.data(), static_cast<int>(binaryData.size()), &width, &height, &img.m_colorChannels, 0), stbi_image_free };
    const uint8_t *dataBytePtr = reinterpret_cast<uint8_t*>(data.get());
    img.m_size = Size(width, height);
    img.m_data.insert(img.m_data.end(), dataBytePtr, dataBytePtr + (width * height) * img.m_colorChannels);
    return img;

}

Image::Image()
    : m_size(0, 0), m_colorChannels(0)
{
}

ARX_NAMESPACE_END

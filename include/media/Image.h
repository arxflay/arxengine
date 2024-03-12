#ifndef ARX_IMAGE_H
#define ARX_IMAGE_H
#include "ArxDefines.h"

#include "ui/Size.h"
#include <string_view>
#include <vector>

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS Image final
{
public:
    bool operator==(const Image &img) const;
    Image();
    Size GetSize() const;
    int GetColorChannels() const;
    const std::vector<uint8_t> &GetData() const;
    bool IsInvalid() const;
    
    //jpeg, png, gif
    static Image LoadFromFile(std::string_view filename, bool flipVerticaly = true);
    static Image LoadFromBinary(const uint8_t *binaryData, size_t binaryDataLen, bool flipVerticaly = true);
    static Image LoadFromData(Size imageSize, int colorChannels, const uint8_t *data);
private:
    Size m_size;
    int m_colorChannels;
    std::vector<uint8_t> m_data;
};


ARX_NAMESPACE_END

#endif

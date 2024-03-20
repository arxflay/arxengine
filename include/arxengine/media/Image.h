/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_IMAGE_H
#define ARX_IMAGE_H
#include "arxengine/ArxDefines.h"

#include "arxengine/ui/Size.h"
#include <string_view>
#include <vector>

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS Image final
{
public:
    bool operator==(const Image &img) const;
    Image();
    SizeUL GetSize() const;
    unsigned int GetColorChannels() const;
    const std::vector<uint8_t> &GetData() const;
    bool IsInvalid() const;
    
    //jpeg, png, gif
    static Image LoadFromFile(std::string_view filename, bool flipVerticaly = true);
    static Image LoadFromBinary(const uint8_t *binaryData, size_t binaryDataLen, bool flipVerticaly = true);
    static Image LoadFromData(SizeUL imageSize, unsigned int colorChannels, const uint8_t *data);
private:
    SizeUL m_size;
    unsigned int m_colorChannels;
    std::vector<uint8_t> m_data;
};


ARX_NAMESPACE_END

#endif

#ifndef ARX_DEFAULTJSONCONVERTER_H
#define ARX_DEFAULTJSONCONVERTER_H
#include "arxengine/Json.h"
ARX_NAMESPACE_BEGIN
class DefaultJsonConvertor : public IJsonConverter
{
public:
    std::string ToJsonStr(const Json &j) const;
private:
    std::string ToJsonStrRec(const Json &j, unsigned int depth) const;
};

ARX_NAMESPACE_END

#endif

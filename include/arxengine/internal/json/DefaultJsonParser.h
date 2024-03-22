#ifndef ARX_DEFAULTJSONPARSER_H
#define ARX_DEFAULTJSONPARSER_H
#include "arxengine/Json.h"
ARX_NAMESPACE_BEGIN

class DefaultJsonParser : public IJsonParser
{
public:
    DefaultJsonParser(IJsonLexer &lexer);
    Json Parse(std::string_view text) override;
private:
    IJsonLexer &m_lexer;
};

ARX_NAMESPACE_END
#endif


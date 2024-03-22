#include "arxengine/internal/json/DefaultJsonParser.h"
#include <stack>
ARX_NAMESPACE_BEGIN


DefaultJsonParser::DefaultJsonParser(IJsonLexer &lexer)
    : m_lexer(lexer)
{
}

Json DefaultJsonParser::Parse(std::string_view text) 
{
    auto tokens(m_lexer.GetTokens(text));
    Json retVal;
    std::stack<Json*> topLevelObjects;
    Json *currentObj = &retVal;

    for (auto &[valType, val] : tokens)
    {
#ifdef DEBUG_ARX_JSON_PARSER
        std::cout << LexerValueTypeStr(valType) << std::endl;
#endif
        switch(valType)
        {
            case LexerValueType::OpenedCurlyBracket:
                if (currentObj->GetType() == JsonValueType::Array) 
                {
                    currentObj->PushBack(Json{ });
                    currentObj = &currentObj->operator[](currentObj->Count() - 1);
                }
                currentObj->Set(JsonObjectMap{ });
                topLevelObjects.push(currentObj);
                break;
            case LexerValueType::ClosedCurlyBracket:
                topLevelObjects.pop();
                if (!topLevelObjects.empty())                  
                    currentObj = topLevelObjects.top();
                break;
            case LexerValueType::OpenedSquareBracket:
                if (currentObj->GetType() == JsonValueType::Array) 
                {
                    currentObj->PushBack(Json{ });
                    currentObj = &currentObj->operator[](currentObj->Count() - 1);
                }
                currentObj->Set(std::vector<Json>{});
                topLevelObjects.push(currentObj);
                break;
            case LexerValueType::ClosedSquareBracket:
                topLevelObjects.pop();
                if (!topLevelObjects.empty())
                    currentObj = topLevelObjects.top();
                break;
            case LexerValueType::Number:
                if (topLevelObjects.top()->GetType() == JsonValueType::Array)
                    currentObj->PushBack(Json(std::stol(val)));
                else
                {
                    currentObj->Set(std::stoi(val));
                    currentObj = topLevelObjects.top();
                }
                break;
            case LexerValueType::String:
                if (topLevelObjects.top()->GetType() == JsonValueType::Array)
                    currentObj->PushBack(Json(std::move(val)));
                else
                {
                    currentObj->Set(val);
                    currentObj = topLevelObjects.top();
                }
                break;
            case LexerValueType::FloatNumber:
                if (topLevelObjects.top()->GetType() == JsonValueType::Array)
                    currentObj->PushBack(Json(std::stod(val)));
                else
                {
                    currentObj->Set(std::stod(val));
                    currentObj = topLevelObjects.top();
                }
                break;
            case LexerValueType::Bool:
                if (topLevelObjects.top()->GetType() == JsonValueType::Array)
                    currentObj->PushBack(Json((val == "true") ? true : false));
                else
                {
                    currentObj->Set((val == "true") ? true : false);
                    currentObj = topLevelObjects.top();
                }
                break;
            case LexerValueType::Null:
                if (topLevelObjects.top()->GetType() == JsonValueType::Array)
                    currentObj->PushBack(Json(nullptr));
                else
                {
                    currentObj->Set(nullptr);
                    currentObj = topLevelObjects.top();
                }
                break;
            case LexerValueType::Identifier:
                currentObj = &currentObj->operator[](val);
                break;
            default:
                throw ArxException(ArxException::ErrorCode::GenericError, "DefaultJsonParser unexpected enum");
                break;
        }
    }

    return retVal;       
}

ARX_NAMESPACE_END


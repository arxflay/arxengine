#include "arxengine/internal/json/DefaultJsonConvertor.h"
ARX_NAMESPACE_BEGIN

std::string DefaultJsonConvertor::ToJsonStr(const Json &j) const
{
    std::string out;
    try
    {
        out = ToJsonStrRec(j, 0);
    }
    catch(const std::exception &e)
    {
        throw ArxException(ArxException::ErrorCode::DefaultJsonConvertorFailure, e.what());
    }
    catch(...)
    {
        throw ArxException(ArxException::ErrorCode::DefaultJsonConvertorFailure);
    }
    return out;
}

std::string DefaultJsonConvertor::ToJsonStrRec(const Json &j, unsigned int depth) const
{
    std::string retVal;
    switch(j.GetType())
    {
        case JsonValueType::NullObj:
            retVal += "null";
            break;
        case JsonValueType::Bool:
            retVal += j.Get<bool>() ? "true" : "false";
            break;
        case JsonValueType::UInt:
            retVal +=  std::to_string(j.Get<uint64_t>());
            break;
        case JsonValueType::Int:
            retVal +=  std::to_string(j.Get<int64_t>());
            break;
        case JsonValueType::Floating:
            retVal +=  std::to_string(j.Get<double>());
            break;
        case JsonValueType::ObjectMap:
            {
                retVal += "{";
                retVal += '\n';
                unsigned int counter = 0;
                for (const auto &[key, storedObject] : j.GetConstRef<JsonObjectMap>())
                {
                    retVal += std::string(depth + 1, '\t');
                    retVal += '"' + key + '"' + ": ";
                    retVal += ToJsonStrRec(storedObject, depth + 1);

                    if (counter + 1 != j.Count())
                        retVal += ',';

                    retVal += '\n';

                    counter++;

                }
                retVal += std::string(depth, '\t') + "}" + "\n";
            }
            break;
        case JsonValueType::Array:
            {
                retVal += "[";
                unsigned int counter = 0;
                for (const auto &storedObject : j.GetConstRef<std::vector<Json>>())
                {
                    retVal += ToJsonStrRec(storedObject, depth + 1);
                    if (counter + 1 != j.Count())
                        retVal += ", ";
                    
                    counter++;
                }
                retVal += std::string(depth, '\t') + "]";
            }
            break;
        case JsonValueType::String:
            retVal +=  std::string("\"") + j.Get<std::string>() + "\"";
            break;
        default:
            break;
    }

    return retVal;
}

ARX_NAMESPACE_END


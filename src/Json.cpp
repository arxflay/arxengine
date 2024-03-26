#include "arxengine/Json.h"
#include "arxengine/internal/json/DefaultJsonConvertor.h"
#include "arxengine/internal/json/DefaultJsonLexer.h"
#include "arxengine/internal/json/DefaultJsonParser.h"
#include "arxengine/misc/Utils.h"

ARX_NAMESPACE_BEGIN
ENUM_FROM_TO_STRING_DEFINE(JsonValueType, "null", "bool", "int", "uint", "floating_point", "string", "array", "json_object_container")

JsonExpectionTypeError::JsonExpectionTypeError (JsonValueType expected, JsonValueType was) 
        : ArxException(ArxException::ErrorCode::JsonValueTypeMismatch, std::string("Type mismatch. Expected ") + StringFromJsonValueType(expected) 
        + " but was " + StringFromJsonValueType(was))
{
}

Json::JsonValue::JsonValue(JsonValue && value)
{          
    using std::swap;
    swap(m_value, value.m_value);
}

Json::JsonValue::JsonValue(const Json::JsonValue &value) 
    : JsonValue(value.Clone())
{
}

Json::JsonValue::~JsonValue()
{
    CleanUp();
}

Json::JsonValue &Json::JsonValue::operator=(Json::JsonValue && value)
{
    if (this == &value)
        return *this;

    using std::swap;
    swap(m_value, value.m_value);
    swap(m_type, value.m_type);

    return *this;
}

Json::JsonValue &Json::JsonValue::operator=(const Json::JsonValue &value)
{
    if (this == &value)
        return *this;

    *this = value.Clone();

    return *this;
}

JsonValueType Json::JsonValue::GetType()
{
    return m_type;
}

void Json::JsonValue::UpdateType(JsonValueType newType)
{
    if (m_type != newType)
    {
        CleanUp();
        m_type = newType;
        switch(m_type)
        {
            case JsonValueType::ObjectMap:
                m_value.json_container = new JsonObjectMap{};
                break;
            case JsonValueType::Array:
                m_value.json_array = new std::vector<Json>{};
                break;
            case JsonValueType::String:
                m_value.string_value = new std::string{};
                break;
            default:
                break;
        }
    }
}

Json::JsonValue Json::JsonValue::Clone() const
{
    JsonValue retVal;
    retVal.m_type = m_type;

    switch(m_type)
    {
        case JsonValueType::ObjectMap:
            retVal.m_value.json_container = new JsonObjectMap(*m_value.json_container);
            break;
        case JsonValueType::Array:
            retVal.m_value.json_array = new std::vector<Json>(*m_value.json_array);
            break;
        case JsonValueType::String:
            retVal.m_value.string_value = new std::string(*m_value.string_value);
            break;
        default:
            retVal.m_value = m_value;
            break;
    }

    return retVal;
}

void Json::JsonValue::CheckType(JsonValueType expected)
{
    if (m_type != expected)
        throw JsonExpectionTypeError(expected, m_type);
}

void Json::JsonValue::CleanUp()
{
    switch(m_type)
    {
        case JsonValueType::ObjectMap:
            delete m_value.json_container;
            break;
        case JsonValueType::Array:
            delete m_value.json_array;
            break;
        case JsonValueType::String:
            delete m_value.string_value;
            break;
        default:
            std::memset(&m_value, 0, sizeof(m_value));
            break;
    }
}

/*move constructor*/
Json::Json(Json &&value)
{
    using std::swap;
    swap(m_json_obj, value.m_json_obj);     
}

/*copy constructor*/
Json::Json(const Json &value)
{
    m_json_obj = value.m_json_obj;
}

/**
 * @brief Special version of constructor that converts const char* to std::string
 * 
 * @param value text
 */
Json::Json(const char *value)
{
    m_json_obj.SetValue(std::string(value));
}

/**
 * @brief Makes possible to construct Json in jsonish/python style
 * 
 * @param js 
 */
Json::Json(std::initializer_list<std::pair<std::string_view, Json>> js)
{
    m_json_obj.SetValue<JsonObjectMap>(JsonObjectMap{});

    for (auto &[key, val] : js)
        operator[](key) = val;
}

/**
 * @brief Special version of assignment that converts const char* to std::string
 * 
 * @param value text
 * @return Json& 
 */
Json &Json::operator=(const char *value)
{
    m_json_obj.SetValue(std::string(value));
    return *this;
}

/*move assignment*/
Json &Json::operator=(Json && j)
{
    using std::swap;
    swap(m_json_obj, j.m_json_obj);

    return *this;
}

/*copy assignment*/
Json &Json::operator=(const Json &j)
{
    if (this == &j)
        return *this;
    
    m_json_obj = j.m_json_obj;

    return *this;
}

/**
 * @brief Makes possible to assign object to Json in jsonish/python style
 * 
 * @param js 
 */
Json &Json::operator=(std::initializer_list<std::pair<std::string, Json>> js)
{
    Set<JsonObjectMap>(JsonObjectMap{});

    for (auto &[key, val] : js)
        operator[](key) = val;

    return *this;
}

/**
 * @brief 
 * 
 * @throw JsonExpectionTypeError - if Json object is not a JsonObjectMap
 * @param key 
 * @return const Json& 
 */
const Json &Json::operator[](std::string_view key) const
{
    return GetValueByKey(key);
}

/**
 * @brief 
 * 
 * @throw JsonExpectionTypeError - if Json object is not a JsonObjectMap
 * @param key 
 * @return const Json& 
 */
Json &Json::operator[](std::string_view key)
{
    return GetValueByKey(key);
}

/**
 * @brief 
 * 
 * @throw JsonExpectionTypeError - if Json object is not an array
 * @param key 
 * @return const Json& 
 */
const Json &Json::operator[](size_t index) const
{
    return GetValueByIndex(index);
}

/**
 * @brief 
 * 
 * @throw JsonExpectionTypeError - if Json object is not an array
 * @param key 
 * @return const Json& 
 */
Json &Json::operator[](size_t index)
{
    return const_cast<Json&>(GetValueByIndex(index));
}


/**
 * @brief Special version of Set that converts const char* to std::string
 * 
 * @param value text
 * @return Json& 
 */
void Json::Set(const char *value)
{
    m_json_obj.SetValue(std::string(value));
}


/**
 * @brief Adds object in the end of array
 * @throw JsonExpectionTypeError if object is not array
 * @param obj 
 */
void Json::PushBack(const Json &obj)
{
    std::vector<Json> &array = m_json_obj.GetValue<const std::vector<Json>&>();
    array.push_back(obj);
}

/**
 * @brief Adds object in the end of array
 * @throw JsonExpectionTypeError if object is not array
 * @param obj
 */
void Json::PushBack(Json &&obj)
{
    std::vector<Json> &array = m_json_obj.GetValue<std::vector<Json>&>();
    array.push_back(std::move(obj));
}

/**
 * @brief Gets amount of objects that are contained in array or objectContainer
 * @throw std::runtime_error if object is not an array/objectContainer
 * @return size_t 
 */
size_t Json::Count() const
{
    if (GetType() == JsonValueType::Array)
        return m_json_obj.GetValue<const std::vector<Json>&>().size();
    else if (GetType() == JsonValueType::ObjectMap)
        return m_json_obj.GetValue<const JsonObjectMap&>().size();
    else
        throw ArxException(ArxException::ErrorCode::JsonValueTypeMismatch, "not an array/object container");

    return 0;

}

/**
 * @brief Get the Type of object
 * 
 * @return JsonValueType 
 */
JsonValueType Json::GetType() const { return m_json_obj.GetType(); }


/**
 * @brief Get value by string key
 * @throw JsonExpectionTypeError - if Json object is not an JsonObjectMap
 * @param key string key
 * @return const Json& 
 */ 
const Json &Json::GetValueByKey(std::string_view key) const
{
    const JsonObjectMap &container = m_json_obj.GetValue<const JsonObjectMap&>();
    return container.at(key.data());
}

/**
 * @brief Get value by string key
 * @throw JsonExpectionTypeError - if Json object is not an JsonObjectMap
 * @param key string key
 * @return Json& 
 */ 
Json &Json::GetValueByKey(std::string_view key)
{
    if (m_json_obj.GetType() == JsonValueType::NullObj)
        m_json_obj.SetValue(JsonObjectMap{ });

    JsonObjectMap &container = m_json_obj.GetValue<JsonObjectMap&>();
    if (!container.count(key.data()))
        container[key.data()] = Json{ };

    return container[key.data()];
}

/**
 * @brief Get value by index
 * @throw JsonExpectionTypeError - if json object is not an std::vector<Json>
 * @param index unsigned integer
 * @return const Json& 
 */
const Json &Json::GetValueByIndex(size_t index) const
{
    const std::vector<Json> &array = m_json_obj.GetValue<const std::vector<Json>&>();
    return array.at(index);
}

/**
 * @brief Get value by index
 * @throw JsonExpectionTypeError - if Json object is not an std::vector<Json>
 * @param index unsigned integer
 * @return Json& 
 */
Json &Json::GetValueByIndex(size_t index)
{
    std::vector<Json> &array = m_json_obj.GetValue<std::vector<Json>&>();
    return array.at(index);
}

/**
 * @brief Parses text and returns Json object.
 * This version accept custom Json parser that matches interface IJsonParser
 * 
 * @param text 
 * @param parser 
 * @return Json 
 */
/* static */ Json Json::Parse(std::string_view text, IJsonParser &parser)
{
    return parser.Parse(text);
}

/**
 * @brief Parses text and returns Json object.
 * This version accept custom Json lexer that matches interface IJsonLexer
 * 
 * @param text 
 * @param lexer class that matches interface
 * @return Json 
 */
/* static */ Json Json::Parse(std::string_view text, IJsonLexer &lexer)
{
    return DefaultJsonParser(lexer).Parse(text);
}

/**
 * @brief Parses text and returns Json object.
 * 
 * @param text 
 * @return Json 
 */
/* static */ Json Json::Parse(std::string_view text)
{
    DefaultJsonLexer defaultLexer;
    return DefaultJsonParser(defaultLexer).Parse(text);
}

/**
 * @brief Loads json file, parses it and returns Json object
 * 
 * @param filename 
 * @return Json 
 */
/* static */ Json Json::FromFile(std::string_view filename)
{
    std::string data;
    Utils::LoadFile(filename, data);
    return Parse(data);
}

/**
 * @brief Converts json object to json string
 * This version accepts custom Converter that matches interface
 * 
 * @param j Json object
 * @return std::string 
 */ 
/* static */ std::string Json::ToJsonStr(const Json &j, const IJsonConverter &convertor)
{
    return convertor.ToJsonStr(j);
}

/**
 * @brief Converts json object to json string
 * 
 * @param j Json object
 * @return std::string 
 */ 
/* static */ std::string Json::ToJsonStr(const Json &j)
{
    DefaultJsonConvertor defaultConvertor;
    return defaultConvertor.ToJsonStr(j);
}

ENUM_FROM_TO_STRING_DEFINE(LexerValueType, "OpenedCurlyBracket", "ClosedCurlyBracket",
        "OpenedSquareBracket", "ClosedSquareBracket",
        "Separator", 
        "Operator",
        "Number",
        "FloatingNumber",
        "String",
        "Identifier",
        "Bool",
        "Null" );

ARX_NAMESPACE_END

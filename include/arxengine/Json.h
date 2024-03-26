#ifndef ARX_JSON_H
#define ARX_JSON_H

#include <string>
#include <functional>
#include <vector>
#include <array>
#include <map>
#include <exception>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include "arxengine/ArxDefines.h"
#include "arxengine/misc/EnumHelpers.h"
#include "arxengine/misc/TypeTraits.h"

ARX_NAMESPACE_BEGIN

class Json;

class ARX_EXPORTS JsonObjectMap : public std::map<std::string, Json>
{
    
};

class ARX_EXPORTS IJsonSerializable
{
public:
    virtual void FromJson(Json &json) const = 0;
};

class ARX_EXPORTS IJsonDeserializable
{
public:
    virtual void ToJson(Json &json) const = 0;
};

class ARX_EXPORTS IJsonObject : public IJsonSerializable, IJsonDeserializable
{
public:
    void FromJson(Json &json) const override = 0;
    void ToJson(Json &json) const override = 0;
};


enum class JsonValueType
{
    NullObj,
    Bool,
    Int,
    UInt,
    Floating,
    String,
    Array,
    ObjectMap
};

ENUM_FROM_TO_STRING_DECLARE(JsonValueType);

class ARX_EXPORTS JsonExpectionTypeError : public ArxException
{
public:
    JsonExpectionTypeError(JsonValueType expected, JsonValueType was); 
};

class IJsonLexer;
class IJsonParser;
class IJsonConverter;

class ARX_EXPORTS Json final
{
private:
    /*
     * @brief union-like object that stores data of specific json object  
     * @description this object is used by Json class
     */
    class ARX_EXPORTS JsonValue final
    {
    private:
        union value
        {
            bool bool_value; 
            int64_t int_value;
            uint64_t uint_value;
            double floating_value;
            JsonObjectMap *json_container;
            std::string *string_value; 
            std::vector<Json> *json_array;
        } m_value{ };

        JsonValueType m_type{ JsonValueType::NullObj };

    public:
        struct NonDefaultType
        {
        };
        JsonValue() = default;

        JsonValue(JsonValue && value);

        JsonValue(const JsonValue &value);
        ~JsonValue();

        JsonValue &operator=(JsonValue && value);
        JsonValue &operator=(const JsonValue &value);


        JsonValueType GetType();

        template <typename T>
        decltype(auto) GetValue()
        {
            using type = remove_cvref_t<T>;
            if constexpr(std::is_same_v<type, std::nullptr_t>)
            {
                CheckType(JsonValueType::NullObj);
                return nullptr;
            }
            else if constexpr(std::is_same_v<type, bool>)
            {
                CheckType(JsonValueType::Bool);
                return static_cast<T>(m_value.uint_value);
            }
            else if constexpr(std::is_same_v<type, double>)
            {
                CheckType(JsonValueType::Floating, std::array{ JsonValueType::Int, JsonValueType::UInt });
                return static_cast<T>(m_value.floating_value);
            }
            else if constexpr(std::is_unsigned_v<type>)
            {   
                CheckType(JsonValueType::UInt, std::array{ JsonValueType::Int, JsonValueType::Floating });
                return static_cast<T>(m_value.uint_value);
            }
            else if constexpr(std::is_signed_v<type>)
            {
                CheckType(JsonValueType::Int, std::array{ JsonValueType::UInt, JsonValueType::Floating });
                return static_cast<T>(m_value.int_value);
            }
            else if constexpr(std::is_same_v<type, std::vector<Json>>)
            {
                CheckType(JsonValueType::Array);
                return *(m_value.json_array);
            }
            else if constexpr(std::is_same_v<type, JsonObjectMap>)
            {
                CheckType(JsonValueType::ObjectMap);
                return *(m_value.json_container);
            }
            else if constexpr(std::is_same_v<type, std::string>)
            {
                CheckType(JsonValueType::String);
                return *(m_value.string_value);
            }
            else
                return NonDefaultType();
        }
        /**
         * @brief Set the Value object
         * 
         * @tparam T 
         * @param value 
         * @return true if is default type
         * @return false if non default type
         */
        template <typename T>
        auto SetValue(T && value)
        {
            using type = remove_cvref_t<T>;
            if constexpr (std::is_same_v<type, std::nullptr_t>)
            {
                UpdateType(JsonValueType::NullObj);
                return std::true_type{};
            }
            else if constexpr(std::is_same_v<type, bool>)
            {
                UpdateType(JsonValueType::Bool);
                m_value.bool_value = value;
                return std::true_type{};
            }
            else if constexpr(std::is_same_v<type, double>)
            {
                UpdateType(JsonValueType::Floating);
                m_value.floating_value = value;
                return std::true_type{};
            }
            else if constexpr(std::is_unsigned_v<type>)
            {
                UpdateType(JsonValueType::UInt);
                m_value.uint_value = value;
                return std::true_type{};
            }
            else if constexpr(std::is_signed_v<type>)
            {
                UpdateType(JsonValueType::Int);
                m_value.int_value = value;
                return std::true_type{};
            }
            else if constexpr(std::is_same_v<type, std::vector<Json>>)
            {
                UpdateType(JsonValueType::Array);
                *(m_value.json_array) = std::forward<T>(value);
                return std::true_type{};
            }
            else if constexpr(std::is_same_v<type, JsonObjectMap>)
            {
                UpdateType(JsonValueType::ObjectMap);
                *(m_value.json_container) = std::forward<T>(value);
                return std::true_type{};
            }
            else if constexpr(std::is_same_v<type, std::string> || std::is_same_v<type, const char*>)
            {
                UpdateType(JsonValueType::String);
                *(m_value.string_value) = std::forward<T>(value);
                return std::true_type{};
            }
            else
                return std::false_type{ };
        }

        void UpdateType(JsonValueType newType);
        JsonValue Clone() const;

        void CheckType(JsonValueType expected);

        template<size_t N>
        void CheckType(JsonValueType expected, const std::array<JsonValueType, N> &conversions)
        {
            if (m_type != expected && std::find(conversions.begin(), conversions.end(), expected) == conversions.end())
                throw JsonExpectionTypeError(expected, m_type);
        }

        void CleanUp();
    };

    struct is_json_default_type
    {
        template<typename T>
        static constexpr bool value() { 
            using type = remove_cvref_t<T>;
            return is_not_same_v<decltype(std::declval<JsonValue>().GetValue<remove_cvref_t<type>>()), JsonValue::NonDefaultType>
            && is_not_same_v<decltype(std::declval<JsonValue>().SetValue<type>(std::declval<type&&>())), JsonValue::NonDefaultType>; 
        }
    };

    template<typename T>
    static constexpr bool is_json_default_type_v = is_json_default_type::value<T>();

public:
    Json() = default;

    /*move constructor*/
    Json(Json &&value);

    /*copy constructor*/
    Json(const Json &value);

    /**
     * @brief Special version of constructor that converts const char* to std::string
     * 
     * @param value text
     */
    Json(const char *value);

    /**
     * @brief Constructs Json object from known types
     * 
     * @tparam T that matches concept JsonDefaultType or is JsonSerialiazable (has FromJson function)
     * @param value 
     */
    template<typename T, std::enable_if_t<is_json_default_type_v<T> || std::is_base_of_v<T, IJsonSerializable>, bool> = true>
    Json(T &&value)
    {
        Set<T>(std::forward<T>(value));
    }
    /**
     * @brief Makes possible to construct Json in jsonish/python style
     * 
     * @param js 
     */
    explicit Json(std::initializer_list<std::pair<std::string_view, Json>> js);
    
    /**
     * @brief Special version of assignment that converts const char* to std::string
     * 
     * @param value text
     * @return Json& 
     */
    Json &operator=(const char *value);

    /*move assignment*/
    Json &operator=(Json && j);

    /*copy assignment*/
    Json &operator=(const Json &j);

    template<typename T>
    std::enable_if_t<is_json_default_type_v<T> || std::is_base_of_v<T, IJsonSerializable>, Json> &operator=(T &&value)
    {
        Set<T>(std::forward<T>(value));
        return *this;
    }

    /**
     * @brief Makes possible to assign object to Json in jsonish/python style
     * 
     * @param js 
     */
    Json &operator=(std::initializer_list<std::pair<std::string, Json>> js);
    
    /**
     * @brief 
     * 
     * @throw JsonExpectionTypeError - if Json object is not a JsonObjectMap
     * @param key 
     * @return const Json& 
     */
    const Json &operator[](std::string_view key) const;

    /**
     * @brief 
     * 
     * @throw JsonExpectionTypeError - if Json object is not a JsonObjectMap
     * @param key 
     * @return const Json& 
     */
    Json &operator[](std::string_view key);

    /**
     * @brief 
     * 
     * @throw JsonExpectionTypeError - if Json object is not an array
     * @param key 
     * @return const Json& 
     */
    const Json &operator[](size_t index) const;

    /**
     * @brief 
     * 
     * @throw JsonExpectionTypeError - if Json object is not an array
     * @param key 
     * @return const Json& 
     */
    Json &operator[](size_t index);

    /**
     * @brief Get value from Json object
     * 
     * @tparam T object that mets requirements of JsonDefaultType
     * @return copy of value that is stored in Json
     */
    template<typename T>
    std::enable_if_t<is_json_default_type_v<T>, T> Get() const
    {
        return m_json_obj.GetValue<T>();
    }

    /**
     * @brief Get value from Json object
     * 
     * @tparam T object that mets requirements of JsonDeserialiazable
     * @return copy of value that is stored in Json
     */
    template<typename T>
    std::enable_if_t<std::is_base_of_v<T, IJsonDeserializable>, T> Get() const
    {
        T nonStandartValue;
        nonStandartValue.FromJson(*this);
        return nonStandartValue;
    }

    /**
     * @brief Get object stored in Json by const reference
     * 
     * @tparam T object that mets requirements of sonDefaultType
     * @return const reference to value that is stored in Json
     */
    template<typename T>
    std::enable_if_t<is_json_default_type_v<T>, std::add_lvalue_reference_t<std::add_const_t<T>>> GetConstRef() const
    {
        return m_json_obj.GetValue<std::add_lvalue_reference_t<std::add_const_t<T>>>();
    }

    /**
     * @brief Special version of Set that converts const char* to std::string
     * 
     * @param value text
     * @return Json& 
     */
    void Set(const char *value);
    
    /**
     * @brief Set value to Json object
     * 
     * @tparam T object that mets requirements of JsonDefaultType
     * @param value value that we want to store
     */
    template<typename T>
    std::enable_if_t<is_json_default_type_v<T>> Set(T &&value)
    {
        m_json_obj.SetValue(std::forward<T>(value));
    }

    /**
     * @brief Set value to Json object
     * 
     * @tparam T object that mets requirements of JsonSerialiazable
     * @param value value that we want to store
     */
    template<typename T>
    std::enable_if_t<std::is_base_of_v<T, IJsonSerializable>, T> Set(T &&value)
    {
        ToJson(*this, std::forward<T>(value));
    }

    /**
     * @brief Adds object in the end of array
     * @throw JsonExpectionTypeError if object is not array
     * @param obj 
     */
    void PushBack(const Json &obj);

    /**
     * @brief Adds object in the end of array
     * @throw JsonExpectionTypeError if object is not array
     * @param obj
     */
    void PushBack(Json &&obj);

    /**
     * @brief Gets amount of objects that are contained in array or objectContainer
     * @throw std::runtime_error if object is not an array/objectContainer
     * @return size_t 
     */
    size_t Count() const;

    /**
     * @brief Get the Type of object
     * 
     * @return JsonValueType 
     */
    JsonValueType GetType() const;


    /**
     * @brief Get value by string key
     * @throw JsonExpectionTypeError - if Json object is not an JsonObjectMap
     * @param key string key
     * @return const Json& 
     */ 
    const Json &GetValueByKey(std::string_view key) const;

    /**
     * @brief Get value by string key
     * @throw JsonExpectionTypeError - if Json object is not an JsonObjectMap
     * @param key string key
     * @return Json& 
     */ 
    Json &GetValueByKey(std::string_view key);

    /**
     * @brief Get value by index
     * @throw JsonExpectionTypeError - if json object is not an std::vector<Json>
     * @param index unsigned integer
     * @return const Json& 
     */
    const Json &GetValueByIndex(size_t index) const;

    /**
     * @brief Get value by index
     * @throw JsonExpectionTypeError - if Json object is not an std::vector<Json>
     * @param index unsigned integer
     * @return Json& 
     */
    Json &GetValueByIndex(size_t index);

    /**
     * @brief Parses text and returns Json object.
     * This version accept custom Json parser that matches interface IJsonParser
     * 
     * @param text 
     * @param parser 
     * @return Json 
     */
    static Json Parse(std::string_view text, IJsonParser &parser);

    /**
     * @brief Parses text and returns Json object.
     * This version accept custom Json lexer that matches interface IJsonLexer
     * 
     * @param text 
     * @param lexer class that matches interface
     * @return Json 
     */
    static Json Parse(std::string_view text, IJsonLexer &lexer);

    /**
     * @brief Parses text and returns Json object.
     * 
     * @param text 
     * @return Json 
     */
    static Json Parse(std::string_view text);

    /**
     * @brief Loads json file, parses it and returns Json object
     * 
     * @param filename 
     * @return Json 
     */
    static Json FromFile(std::string_view filename);

    /**
     * @brief Converts json object to json string
     * This version accepts custom Converter that matches interface
     * 
     * @param j Json object
     * @return std::string 
     */ 
    static std::string ToJsonStr(const Json &j, const IJsonConverter &convertor);

    /**
     * @brief Converts json object to json string
     * 
     * @param j Json object
     * @return std::string 
     */ 
    static std::string ToJsonStr(const Json &j);

private:
    mutable JsonValue m_json_obj{}; 
};

enum class LexerValueType
{
    OpenedCurlyBracket = 0, //'{'
    ClosedCurlyBracket, //'}'
    OpenedSquareBracket, //'['
    ClosedSquareBracket, //']'
    Separator, //',', not used 
    Operator, //':', not used
    Number, //digits
    FloatNumber, //digits with .
    String, //string
    Identifier, //string with alpha symbols, numbers or spaces
    Bool, //true or false
    Null //null
};

ENUM_FROM_TO_STRING_DECLARE(LexerValueType);

class ARX_EXPORTS IJsonLexer    
{
public:
    virtual std::vector<std::pair<LexerValueType, std::string>> GetTokens(std::string_view text) = 0;
};

class ARX_EXPORTS IJsonParser
{
public:
    virtual Json Parse(std::string_view text) = 0;
};

class ARX_EXPORTS IJsonConverter
{
public:
    virtual std::string ToJsonStr(const Json &j) const = 0;
};

ARX_NAMESPACE_END

#endif

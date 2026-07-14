#include "JsonValue.h"

using std::get;
using std::move;

JsonValue::JsonValue() : type_(Type::Null), value_(monostate{}) {}

JsonValue::JsonValue(nullptr_t) : type_(Type::Null), value_(monostate{}) {}

JsonValue::JsonValue(bool value) : type_(Type::Boolean), value_(value) {}

JsonValue::JsonValue(double value) : type_(Type::Number), value_(value) {}

JsonValue::JsonValue(int value) : type_(Type::Number), value_(static_cast<double>(value)) {}

JsonValue::JsonValue(const char* value) : type_(Type::String), value_(string(value)) {}

JsonValue::JsonValue(string value) : type_(Type::String), value_(move(value)) {}

JsonValue::JsonValue(ArrayType value) : type_(Type::Array), value_(move(value)) {}

JsonValue::JsonValue(ObjectType value) : type_(Type::Object), value_(move(value)) {}

bool JsonValue::AsBoolean() const
{
    if (type_ != Type::Boolean)
    {
        throw JsonTypeException("JsonValue is not a Boolean");
    }
    return get<bool>(value_);
}

double JsonValue::AsNumber() const
{
    if (type_ != Type::Number)
    {
        throw JsonTypeException("JsonValue is not a Number");
    }
    return get<double>(value_);
}

const string& JsonValue::AsString() const
{
    if (type_ != Type::String)
    {
        throw JsonTypeException("JsonValue is not a String");
    }
    return get<string>(value_);
}

const JsonValue::ArrayType& JsonValue::AsArray() const
{
    if (type_ != Type::Array)
    {
        throw JsonTypeException("JsonValue is not an Array");
    }
    return get<ArrayType>(value_);
}

const JsonValue::ObjectType& JsonValue::AsObject() const
{
    if (type_ != Type::Object)
    {
        throw JsonTypeException("JsonValue is not an Object");
    }
    return get<ObjectType>(value_);
}

JsonValue::ArrayType& JsonValue::AsArray()
{
    if (type_ != Type::Array)
    {
        throw JsonTypeException("JsonValue is not an Array");
    }
    return get<ArrayType>(value_);
}

JsonValue::ObjectType& JsonValue::AsObject()
{
    if (type_ != Type::Object)
    {
        throw JsonTypeException("JsonValue is not an Object");
    }
    return get<ObjectType>(value_);
}

JsonValue& JsonValue::operator[](size_t index)
{
    ArrayType& array = AsArray();
    if (index >= array.size())
    {
        throw JsonTypeException("Array index out of range");
    }
    return array[index];
}

const JsonValue& JsonValue::operator[](size_t index) const
{
    const ArrayType& array = AsArray();
    if (index >= array.size())
    {
        throw JsonTypeException("Array index out of range");
    }
    return array[index];
}

JsonValue& JsonValue::operator[](const string& key)
{
    ObjectType& object = AsObject();
    for (auto& entry : object)
    {
        if (entry.first == key)
        {
            return entry.second;
        }
    }
    object.emplace_back(key, JsonValue());
    return object.back().second;
}

const JsonValue& JsonValue::operator[](const string& key) const
{
    const ObjectType& object = AsObject();
    for (const auto& entry : object)
    {
        if (entry.first == key)
        {
            return entry.second;
        }
    }
    throw JsonTypeException("Object has no key: " + key);
}

bool JsonValue::HasKey(const string& key) const
{
    const ObjectType& object = AsObject();
    for (const auto& entry : object)
    {
        if (entry.first == key)
        {
            return true;
        }
    }
    return false;
}

JsonValue JsonValue::MakeArray()
{
    return JsonValue(ArrayType{});
}

JsonValue JsonValue::MakeObject()
{
    return JsonValue(ObjectType{});
}

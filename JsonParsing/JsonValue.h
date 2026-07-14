#pragma once

#include <string>
#include <variant>
#include <vector>
#include <utility>
#include <stdexcept>

using std::vector;

class JsonValue
{
public:
    enum class Type
    {
        Null,
        Boolean,
        Number,
        String,
        Array,
        Object
    };

    using ArrayType = vector<JsonValue>;
    using ObjectType = vector<std::pair<std::string, JsonValue>>;

    JsonValue();
    JsonValue(std::nullptr_t);
    JsonValue(bool value);
    JsonValue(double value);
    JsonValue(int value);
    JsonValue(const char* value);
    JsonValue(std::string value);
    JsonValue(ArrayType value);
    JsonValue(ObjectType value);

    Type GetType() const { return type_; }

    bool IsNull() const { return type_ == Type::Null; }
    bool IsBoolean() const { return type_ == Type::Boolean; }
    bool IsNumber() const { return type_ == Type::Number; }
    bool IsString() const { return type_ == Type::String; }
    bool IsArray() const { return type_ == Type::Array; }
    bool IsObject() const { return type_ == Type::Object; }

    bool AsBoolean() const;
    double AsNumber() const;
    const std::string& AsString() const;
    const ArrayType& AsArray() const;
    const ObjectType& AsObject() const;
    ArrayType& AsArray();
    ObjectType& AsObject();

    // Array element access (throws if not an array or index out of range).
    JsonValue& operator[](size_t index);
    const JsonValue& operator[](size_t index) const;

    // Object member access. Non-const version inserts a Null entry if the key
    // is missing (matches std::map::operator[] semantics); const version throws.
    JsonValue& operator[](const std::string& key);
    const JsonValue& operator[](const std::string& key) const;

    bool HasKey(const std::string& key) const;

    static JsonValue MakeArray();
    static JsonValue MakeObject();

private:
    Type type_;
    std::variant<std::monostate, bool, double, std::string, ArrayType, ObjectType> value_;
};

class JsonTypeException : public std::runtime_error
{
public:
    explicit JsonTypeException(const std::string& message) : std::runtime_error(message) {}
};

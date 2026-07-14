#pragma once

#include "JsonValue.h"

#include <stdexcept>
#include <string>

class JsonParseException : public std::runtime_error
{
public:
    explicit JsonParseException(const std::string& message) : std::runtime_error(message) {}
};

class JsonParser
{
public:
    static JsonValue Parse(const std::string& text);
    static JsonValue ParseFile(const std::string& path);

private:
    explicit JsonParser(const std::string& text);

    JsonValue ParseDocument();
    JsonValue ParseValue();
    JsonValue ParseObject();
    JsonValue ParseArray();
    JsonValue ParseString();
    JsonValue ParseNumber();
    JsonValue ParseLiteral();
    std::string ParseRawString();

    void SkipWhitespace();
    char Peek() const;
    char Advance();
    bool IsAtEnd() const;
    void Expect(char expected);
    [[noreturn]] void Fail(const std::string& message) const;

    std::string text_;
    size_t pos_;
};

#pragma once

#include "JsonValue.h"

#include <stdexcept>
#include <string>

using std::runtime_error;
using std::string;

class JsonParseException : public runtime_error
{
public:
    explicit JsonParseException(const string& message) : runtime_error(message) {}
};

class JsonParser
{
public:
    static JsonValue Parse(const string& text);
    static JsonValue ParseFile(const string& path);

private:
    explicit JsonParser(const string& text);

    JsonValue ParseDocument();
    JsonValue ParseValue();
    JsonValue ParseObject();
    JsonValue ParseArray();
    JsonValue ParseString();
    JsonValue ParseNumber();
    JsonValue ParseLiteral();
    string ParseRawString();

    void SkipWhitespace();
    char Peek() const;
    char Advance();
    bool IsAtEnd() const;
    void Expect(char expected);
    [[noreturn]] void Fail(const string& message) const;

    string text_;
    size_t pos_;
};

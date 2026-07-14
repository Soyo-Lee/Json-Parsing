#pragma once

#include "JsonValue.h"

#include <stdexcept>
#include <string>

class JsonWriteException : public std::runtime_error
{
public:
    explicit JsonWriteException(const std::string& message) : std::runtime_error(message) {}
};

class JsonWriter
{
public:
    static std::string Write(const JsonValue& value, bool pretty = false, int indentSize = 2);
    static void SaveToFile(const JsonValue& value, const std::string& path, bool pretty = true, int indentSize = 2);

private:
    static void WriteValue(const JsonValue& value, std::string& out, bool pretty, int indentSize, int depth);
    static void WriteString(const std::string& text, std::string& out);
    static void WriteNumber(double number, std::string& out);
    static void AppendIndent(std::string& out, int indentSize, int depth);
};

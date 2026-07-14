#pragma once

#include "JsonValue.h"

#include <stdexcept>
#include <string>

using std::runtime_error;
using std::string;

class JsonWriteException : public runtime_error
{
public:
    explicit JsonWriteException(const string& message) : runtime_error(message) {}
};

class JsonWriter
{
public:
    static string Write(const JsonValue& value, bool pretty = false, int indentSize = 2);
    static void SaveToFile(const JsonValue& value, const string& path, bool pretty = true, int indentSize = 2);

private:
    static void WriteValue(const JsonValue& value, string& out, bool pretty, int indentSize, int depth);
    static void WriteString(const string& text, string& out);
    static void WriteNumber(double number, string& out);
    static void AppendIndent(string& out, int indentSize, int depth);
};

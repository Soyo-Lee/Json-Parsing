#include "JsonWriter.h"

#include <array>
#include <charconv>
#include <cstdio>
#include <fstream>

std::string JsonWriter::Write(const JsonValue& value, bool pretty, int indentSize)
{
    std::string out;
    WriteValue(value, out, pretty, indentSize, 0);
    return out;
}

void JsonWriter::SaveToFile(const JsonValue& value, const std::string& path, bool pretty, int indentSize)
{
    std::ofstream file(path, std::ios::binary);
    if (!file)
    {
        throw JsonWriteException("Could not open file for writing: " + path);
    }
    file << Write(value, pretty, indentSize);
    if (!file)
    {
        throw JsonWriteException("Failed to write JSON to file: " + path);
    }
}

void JsonWriter::WriteValue(const JsonValue& value, std::string& out, bool pretty, int indentSize, int depth)
{
    switch (value.GetType())
    {
    case JsonValue::Type::Null:
        out += "null";
        break;

    case JsonValue::Type::Boolean:
        out += value.AsBoolean() ? "true" : "false";
        break;

    case JsonValue::Type::Number:
        WriteNumber(value.AsNumber(), out);
        break;

    case JsonValue::Type::String:
        WriteString(value.AsString(), out);
        break;

    case JsonValue::Type::Array:
    {
        const auto& array = value.AsArray();
        if (array.empty())
        {
            out += "[]";
            break;
        }
        out += '[';
        for (size_t i = 0; i < array.size(); ++i)
        {
            if (pretty)
            {
                out += '\n';
                AppendIndent(out, indentSize, depth + 1);
            }
            WriteValue(array[i], out, pretty, indentSize, depth + 1);
            if (i + 1 < array.size())
            {
                out += ',';
            }
        }
        if (pretty)
        {
            out += '\n';
            AppendIndent(out, indentSize, depth);
        }
        out += ']';
        break;
    }

    case JsonValue::Type::Object:
    {
        const auto& object = value.AsObject();
        if (object.empty())
        {
            out += "{}";
            break;
        }
        out += '{';
        for (size_t i = 0; i < object.size(); ++i)
        {
            if (pretty)
            {
                out += '\n';
                AppendIndent(out, indentSize, depth + 1);
            }
            WriteString(object[i].first, out);
            out += pretty ? ": " : ":";
            WriteValue(object[i].second, out, pretty, indentSize, depth + 1);
            if (i + 1 < object.size())
            {
                out += ',';
            }
        }
        if (pretty)
        {
            out += '\n';
            AppendIndent(out, indentSize, depth);
        }
        out += '}';
        break;
    }
    }
}

void JsonWriter::WriteString(const std::string& text, std::string& out)
{
    out += '"';
    for (unsigned char c : text)
    {
        switch (c)
        {
        case '"': out += "\\\""; break;
        case '\\': out += "\\\\"; break;
        case '\b': out += "\\b"; break;
        case '\f': out += "\\f"; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
            if (c < 0x20)
            {
                char buf[8];
                std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                out += buf;
            }
            else
            {
                out += static_cast<char>(c);
            }
        }
    }
    out += '"';
}

void JsonWriter::WriteNumber(double number, std::string& out)
{
    std::array<char, 64> buffer{};
    auto result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), number);
    out.append(buffer.data(), result.ptr);
}

void JsonWriter::AppendIndent(std::string& out, int indentSize, int depth)
{
    out.append(static_cast<size_t>(indentSize) * static_cast<size_t>(depth), ' ');
}

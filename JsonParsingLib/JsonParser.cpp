#include "JsonParser.h"

#include <cctype>
#include <fstream>
#include <sstream>

JsonValue JsonParser::Parse(const std::string& text)
{
    JsonParser parser(text);
    return parser.ParseDocument();
}

JsonValue JsonParser::ParseFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        throw JsonParseException("Could not open file: " + path);
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return Parse(buffer.str());
}

JsonParser::JsonParser(const std::string& text) : text_(text), pos_(0) {}

JsonValue JsonParser::ParseDocument()
{
    SkipWhitespace();
    JsonValue value = ParseValue();
    SkipWhitespace();
    if (!IsAtEnd())
    {
        Fail("Unexpected trailing characters after JSON value");
    }
    return value;
}

JsonValue JsonParser::ParseValue()
{
    SkipWhitespace();
    if (IsAtEnd())
    {
        Fail("Unexpected end of input while expecting a value");
    }

    char c = Peek();
    switch (c)
    {
    case '{':
        return ParseObject();
    case '[':
        return ParseArray();
    case '"':
        return ParseString();
    case 't':
    case 'f':
    case 'n':
        return ParseLiteral();
    default:
        if (c == '-' || std::isdigit(static_cast<unsigned char>(c)))
        {
            return ParseNumber();
        }
        Fail(std::string("Unexpected character '") + c + "' while expecting a value");
    }
}

JsonValue JsonParser::ParseObject()
{
    Expect('{');
    JsonValue::ObjectType members;

    SkipWhitespace();
    if (!IsAtEnd() && Peek() == '}')
    {
        Advance();
        return JsonValue(std::move(members));
    }

    while (true)
    {
        SkipWhitespace();
        if (IsAtEnd() || Peek() != '"')
        {
            Fail("Expected string key in object");
        }
        std::string key = ParseRawString();

        SkipWhitespace();
        Expect(':');

        JsonValue value = ParseValue();
        members.emplace_back(std::move(key), std::move(value));

        SkipWhitespace();
        if (IsAtEnd())
        {
            Fail("Unterminated object");
        }
        char next = Advance();
        if (next == ',')
        {
            SkipWhitespace();
            continue;
        }
        if (next == '}')
        {
            break;
        }
        Fail("Expected ',' or '}' in object");
    }

    return JsonValue(std::move(members));
}

JsonValue JsonParser::ParseArray()
{
    Expect('[');
    JsonValue::ArrayType elements;

    SkipWhitespace();
    if (!IsAtEnd() && Peek() == ']')
    {
        Advance();
        return JsonValue(std::move(elements));
    }

    while (true)
    {
        JsonValue value = ParseValue();
        elements.push_back(std::move(value));

        SkipWhitespace();
        if (IsAtEnd())
        {
            Fail("Unterminated array");
        }
        char next = Advance();
        if (next == ',')
        {
            SkipWhitespace();
            continue;
        }
        if (next == ']')
        {
            break;
        }
        Fail("Expected ',' or ']' in array");
    }

    return JsonValue(std::move(elements));
}

JsonValue JsonParser::ParseString()
{
    return JsonValue(ParseRawString());
}

std::string JsonParser::ParseRawString()
{
    Expect('"');
    std::string result;

    while (true)
    {
        if (IsAtEnd())
        {
            Fail("Unterminated string");
        }
        char c = Advance();
        if (c == '"')
        {
            break;
        }
        if (c == '\\')
        {
            if (IsAtEnd())
            {
                Fail("Unterminated escape sequence in string");
            }
            char escape = Advance();
            switch (escape)
            {
            case '"': result += '"'; break;
            case '\\': result += '\\'; break;
            case '/': result += '/'; break;
            case 'b': result += '\b'; break;
            case 'f': result += '\f'; break;
            case 'n': result += '\n'; break;
            case 'r': result += '\r'; break;
            case 't': result += '\t'; break;
            case 'u':
            {
                if (pos_ + 4 > text_.size())
                {
                    Fail("Invalid \\u escape sequence in string");
                }
                unsigned int codePoint = 0;
                for (int i = 0; i < 4; ++i)
                {
                    char hex = Advance();
                    codePoint <<= 4;
                    if (hex >= '0' && hex <= '9') codePoint |= (hex - '0');
                    else if (hex >= 'a' && hex <= 'f') codePoint |= (hex - 'a' + 10);
                    else if (hex >= 'A' && hex <= 'F') codePoint |= (hex - 'A' + 10);
                    else Fail("Invalid hex digit in \\u escape sequence");
                }
                // Encode as UTF-8 (BMP only; surrogate pairs are not decoded).
                if (codePoint <= 0x7F)
                {
                    result += static_cast<char>(codePoint);
                }
                else if (codePoint <= 0x7FF)
                {
                    result += static_cast<char>(0xC0 | (codePoint >> 6));
                    result += static_cast<char>(0x80 | (codePoint & 0x3F));
                }
                else
                {
                    result += static_cast<char>(0xE0 | (codePoint >> 12));
                    result += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
                    result += static_cast<char>(0x80 | (codePoint & 0x3F));
                }
                break;
            }
            default:
                Fail(std::string("Invalid escape character '") + escape + "' in string");
            }
        }
        else
        {
            result += c;
        }
    }

    return result;
}

JsonValue JsonParser::ParseNumber()
{
    size_t start = pos_;

    if (!IsAtEnd() && Peek() == '-')
    {
        Advance();
    }
    if (IsAtEnd() || !std::isdigit(static_cast<unsigned char>(Peek())))
    {
        Fail("Invalid number: expected digit");
    }
    while (!IsAtEnd() && std::isdigit(static_cast<unsigned char>(Peek())))
    {
        Advance();
    }

    if (!IsAtEnd() && Peek() == '.')
    {
        Advance();
        if (IsAtEnd() || !std::isdigit(static_cast<unsigned char>(Peek())))
        {
            Fail("Invalid number: expected digit after decimal point");
        }
        while (!IsAtEnd() && std::isdigit(static_cast<unsigned char>(Peek())))
        {
            Advance();
        }
    }

    if (!IsAtEnd() && (Peek() == 'e' || Peek() == 'E'))
    {
        Advance();
        if (!IsAtEnd() && (Peek() == '+' || Peek() == '-'))
        {
            Advance();
        }
        if (IsAtEnd() || !std::isdigit(static_cast<unsigned char>(Peek())))
        {
            Fail("Invalid number: expected digit in exponent");
        }
        while (!IsAtEnd() && std::isdigit(static_cast<unsigned char>(Peek())))
        {
            Advance();
        }
    }

    std::string numberText = text_.substr(start, pos_ - start);
    return JsonValue(std::stod(numberText));
}

JsonValue JsonParser::ParseLiteral()
{
    if (text_.compare(pos_, 4, "true") == 0)
    {
        pos_ += 4;
        return JsonValue(true);
    }
    if (text_.compare(pos_, 5, "false") == 0)
    {
        pos_ += 5;
        return JsonValue(false);
    }
    if (text_.compare(pos_, 4, "null") == 0)
    {
        pos_ += 4;
        return JsonValue(nullptr);
    }
    Fail("Invalid literal");
}

void JsonParser::SkipWhitespace()
{
    while (!IsAtEnd())
    {
        char c = Peek();
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
        {
            Advance();
        }
        else
        {
            break;
        }
    }
}

char JsonParser::Peek() const
{
    return text_[pos_];
}

char JsonParser::Advance()
{
    return text_[pos_++];
}

bool JsonParser::IsAtEnd() const
{
    return pos_ >= text_.size();
}

void JsonParser::Expect(char expected)
{
    if (IsAtEnd() || Peek() != expected)
    {
        Fail(std::string("Expected '") + expected + "'");
    }
    Advance();
}

void JsonParser::Fail(const std::string& message) const
{
    throw JsonParseException(message + " (at position " + std::to_string(pos_) + ")");
}

#include "JsonParser.h"

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>

namespace
{
class JsonParserTest : public ::testing::Test
{
protected:
    void TearDown() override
    {
        std::remove(path_.c_str());
    }

    std::string path_ = "json_parser_test_input.json";
};
} // namespace

TEST_F(JsonParserTest, ParsesNull)
{
    JsonValue value = JsonParser::Parse("null");
    EXPECT_TRUE(value.IsNull());
}

TEST_F(JsonParserTest, ParsesTrueAndFalse)
{
    EXPECT_TRUE(JsonParser::Parse("true").AsBoolean());
    EXPECT_FALSE(JsonParser::Parse("false").AsBoolean());
}

TEST_F(JsonParserTest, ParsesIntegerNumber)
{
    JsonValue value = JsonParser::Parse("42");
    EXPECT_TRUE(value.IsNumber());
    EXPECT_DOUBLE_EQ(value.AsNumber(), 42.0);
}

TEST_F(JsonParserTest, ParsesNegativeAndDecimalNumber)
{
    EXPECT_DOUBLE_EQ(JsonParser::Parse("-17").AsNumber(), -17.0);
    EXPECT_DOUBLE_EQ(JsonParser::Parse("3.14").AsNumber(), 3.14);
    EXPECT_DOUBLE_EQ(JsonParser::Parse("-0.5").AsNumber(), -0.5);
}

TEST_F(JsonParserTest, ParsesNumberWithExponent)
{
    EXPECT_DOUBLE_EQ(JsonParser::Parse("1e2").AsNumber(), 100.0);
    EXPECT_DOUBLE_EQ(JsonParser::Parse("1.5E+2").AsNumber(), 150.0);
    EXPECT_DOUBLE_EQ(JsonParser::Parse("2e-1").AsNumber(), 0.2);
}

TEST_F(JsonParserTest, ParsesSimpleString)
{
    JsonValue value = JsonParser::Parse("\"hello\"");
    EXPECT_TRUE(value.IsString());
    EXPECT_EQ(value.AsString(), "hello");
}

TEST_F(JsonParserTest, ParsesStringWithEscapes)
{
    JsonValue value = JsonParser::Parse(R"("line\nbreak\tand\\backslash\"quote")");
    EXPECT_EQ(value.AsString(), "line\nbreak\tand\\backslash\"quote");
}

TEST_F(JsonParserTest, ParsesStringWithUnicodeEscape)
{
    JsonValue value = JsonParser::Parse("\"\\u0041\\u0042\"");
    EXPECT_EQ(value.AsString(), "AB");
}

TEST_F(JsonParserTest, ParsesEmptyArray)
{
    JsonValue value = JsonParser::Parse("[]");
    EXPECT_TRUE(value.IsArray());
    EXPECT_EQ(value.AsArray().size(), 0u);
}

TEST_F(JsonParserTest, ParsesArrayOfMixedTypes)
{
    JsonValue value = JsonParser::Parse(R"([1, "two", true, null, 3.5])");
    ASSERT_TRUE(value.IsArray());
    ASSERT_EQ(value.AsArray().size(), 5u);
    EXPECT_DOUBLE_EQ(value[0].AsNumber(), 1.0);
    EXPECT_EQ(value[1].AsString(), "two");
    EXPECT_TRUE(value[2].AsBoolean());
    EXPECT_TRUE(value[3].IsNull());
    EXPECT_DOUBLE_EQ(value[4].AsNumber(), 3.5);
}

TEST_F(JsonParserTest, ParsesEmptyObject)
{
    JsonValue value = JsonParser::Parse("{}");
    EXPECT_TRUE(value.IsObject());
    EXPECT_EQ(value.AsObject().size(), 0u);
}

TEST_F(JsonParserTest, ParsesObjectWithMembers)
{
    JsonValue value = JsonParser::Parse(R"({"name": "Alice", "age": 30, "active": true})");
    ASSERT_TRUE(value.IsObject());
    EXPECT_EQ(value["name"].AsString(), "Alice");
    EXPECT_DOUBLE_EQ(value["age"].AsNumber(), 30.0);
    EXPECT_TRUE(value["active"].AsBoolean());
}

TEST_F(JsonParserTest, ParsesNestedObjectsAndArrays)
{
    JsonValue value = JsonParser::Parse(R"(
        {
            "user": {
                "name": "Bob",
                "tags": ["a", "b", "c"]
            },
            "count": 3
        }
    )");

    ASSERT_TRUE(value.IsObject());
    EXPECT_EQ(value["user"]["name"].AsString(), "Bob");
    ASSERT_EQ(value["user"]["tags"].AsArray().size(), 3u);
    EXPECT_EQ(value["user"]["tags"][1].AsString(), "b");
    EXPECT_DOUBLE_EQ(value["count"].AsNumber(), 3.0);
}

TEST_F(JsonParserTest, IgnoresLeadingAndTrailingWhitespace)
{
    JsonValue value = JsonParser::Parse("  \n\t [ 1 , 2 ]  \n");
    ASSERT_TRUE(value.IsArray());
    EXPECT_DOUBLE_EQ(value[0].AsNumber(), 1.0);
    EXPECT_DOUBLE_EQ(value[1].AsNumber(), 2.0);
}

TEST_F(JsonParserTest, ThrowsOnEmptyInput)
{
    EXPECT_THROW(JsonParser::Parse(""), JsonParseException);
}

TEST_F(JsonParserTest, ThrowsOnInvalidLiteral)
{
    EXPECT_THROW(JsonParser::Parse("nul"), JsonParseException);
    EXPECT_THROW(JsonParser::Parse("True"), JsonParseException);
}

TEST_F(JsonParserTest, ThrowsOnUnterminatedString)
{
    EXPECT_THROW(JsonParser::Parse("\"unterminated"), JsonParseException);
}

TEST_F(JsonParserTest, ThrowsOnTrailingGarbage)
{
    EXPECT_THROW(JsonParser::Parse("123 456"), JsonParseException);
}

TEST_F(JsonParserTest, ThrowsOnMismatchedBrackets)
{
    EXPECT_THROW(JsonParser::Parse("[1, 2"), JsonParseException);
    EXPECT_THROW(JsonParser::Parse("{\"a\": 1"), JsonParseException);
    EXPECT_THROW(JsonParser::Parse("[1, 2,]"), JsonParseException);
}

TEST_F(JsonParserTest, ParsesFromFile)
{
    {
        std::ofstream out(path_, std::ios::binary);
        out << R"({"key": "value"})";
    }

    JsonValue value = JsonParser::ParseFile(path_);
    EXPECT_EQ(value["key"].AsString(), "value");
}

TEST_F(JsonParserTest, ThrowsWhenFileDoesNotExist)
{
    EXPECT_THROW(JsonParser::ParseFile("no_such_file.json"), JsonParseException);
}

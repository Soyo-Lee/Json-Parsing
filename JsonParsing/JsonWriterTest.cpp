#include "JsonWriter.h"
#include "JsonParser.h"

#include <cstdio>
#include <gtest/gtest.h>

using std::remove;
using std::string;

namespace
{
class JsonWriterTest : public ::testing::Test
{
protected:
    void TearDown() override
    {
        remove(path_.c_str());
    }

    static bool DeepEquals(const JsonValue& a, const JsonValue& b)
    {
        if (a.GetType() != b.GetType())
        {
            return false;
        }
        switch (a.GetType())
        {
        case JsonValue::Type::Null:
            return true;
        case JsonValue::Type::Boolean:
            return a.AsBoolean() == b.AsBoolean();
        case JsonValue::Type::Number:
            return a.AsNumber() == b.AsNumber();
        case JsonValue::Type::String:
            return a.AsString() == b.AsString();
        case JsonValue::Type::Array:
        {
            const auto& arrayA = a.AsArray();
            const auto& arrayB = b.AsArray();
            if (arrayA.size() != arrayB.size())
            {
                return false;
            }
            for (size_t i = 0; i < arrayA.size(); ++i)
            {
                if (!DeepEquals(arrayA[i], arrayB[i]))
                {
                    return false;
                }
            }
            return true;
        }
        case JsonValue::Type::Object:
        {
            const auto& objectA = a.AsObject();
            const auto& objectB = b.AsObject();
            if (objectA.size() != objectB.size())
            {
                return false;
            }
            for (size_t i = 0; i < objectA.size(); ++i)
            {
                if (objectA[i].first != objectB[i].first || !DeepEquals(objectA[i].second, objectB[i].second))
                {
                    return false;
                }
            }
            return true;
        }
        }
        return false;
    }

    string path_ = "json_writer_test_output.json";
};
} // namespace

TEST_F(JsonWriterTest, WritesNull)
{
    EXPECT_EQ(JsonWriter::Write(JsonValue()), "null");
}

TEST_F(JsonWriterTest, WritesBoolean)
{
    EXPECT_EQ(JsonWriter::Write(JsonValue(true)), "true");
    EXPECT_EQ(JsonWriter::Write(JsonValue(false)), "false");
}

TEST_F(JsonWriterTest, WritesIntegralNumberWithoutTrailingZero)
{
    EXPECT_EQ(JsonWriter::Write(JsonValue(42)), "42");
    EXPECT_EQ(JsonWriter::Write(JsonValue(-3)), "-3");
    EXPECT_EQ(JsonWriter::Write(JsonValue(0)), "0");
}

TEST_F(JsonWriterTest, WritesFractionalNumber)
{
    EXPECT_EQ(JsonWriter::Write(JsonValue(3.14)), "3.14");
    EXPECT_EQ(JsonWriter::Write(JsonValue(-0.5)), "-0.5");
}

TEST_F(JsonWriterTest, WritesStringWithEscapes)
{
    JsonValue value(string("line\nbreak\t\"quoted\"\\slash"));
    EXPECT_EQ(JsonWriter::Write(value), "\"line\\nbreak\\t\\\"quoted\\\"\\\\slash\"");
}

TEST_F(JsonWriterTest, WritesEmptyArrayAndObjectCompact)
{
    EXPECT_EQ(JsonWriter::Write(JsonValue::MakeArray()), "[]");
    EXPECT_EQ(JsonWriter::Write(JsonValue::MakeObject()), "{}");
}

TEST_F(JsonWriterTest, WritesCompactArrayAndObject)
{
    JsonValue::ArrayType elements;
    elements.push_back(JsonValue(1));
    elements.push_back(JsonValue("two"));
    elements.push_back(JsonValue(true));
    EXPECT_EQ(JsonWriter::Write(JsonValue(elements)), R"([1,"two",true])");

    JsonValue object = JsonValue::MakeObject();
    object["name"] = JsonValue("Alice");
    object["age"] = JsonValue(30);
    EXPECT_EQ(JsonWriter::Write(object), R"({"name":"Alice","age":30})");
}

TEST_F(JsonWriterTest, PrettyPrintIndentsNestedStructures)
{
    JsonValue object = JsonValue::MakeObject();
    object["name"] = JsonValue("Alice");
    JsonValue::ArrayType tags;
    tags.push_back(JsonValue("a"));
    tags.push_back(JsonValue("b"));
    object["tags"] = JsonValue(tags);

    string expected =
        "{\n"
        "  \"name\": \"Alice\",\n"
        "  \"tags\": [\n"
        "    \"a\",\n"
        "    \"b\"\n"
        "  ]\n"
        "}";
    EXPECT_EQ(JsonWriter::Write(object, /*pretty=*/true), expected);
}

TEST_F(JsonWriterTest, RoundTripsThroughParser)
{
    JsonValue object = JsonValue::MakeObject();
    object["name"] = JsonValue("Bob");
    object["age"] = JsonValue(25);
    object["active"] = JsonValue(true);
    object["nickname"] = JsonValue();
    JsonValue::ArrayType scores;
    scores.push_back(JsonValue(1));
    scores.push_back(JsonValue(2.5));
    object["scores"] = JsonValue(scores);

    string json = JsonWriter::Write(object, /*pretty=*/true);
    JsonValue reparsed = JsonParser::Parse(json);

    EXPECT_TRUE(DeepEquals(object, reparsed));
}

TEST_F(JsonWriterTest, SavesToFileAndRoundTrips)
{
    JsonValue object = JsonValue::MakeObject();
    object["key"] = JsonValue("value");

    JsonWriter::SaveToFile(object, path_);

    JsonValue reparsed = JsonParser::ParseFile(path_);
    EXPECT_TRUE(DeepEquals(object, reparsed));
}

TEST_F(JsonWriterTest, SaveToFileThrowsWhenPathIsInvalid)
{
    JsonValue value(1);
    EXPECT_THROW(JsonWriter::SaveToFile(value, "no_such_directory/output.json"), JsonWriteException);
}

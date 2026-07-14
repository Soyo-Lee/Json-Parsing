#include "JsonValue.h"

#include <gtest/gtest.h>

TEST(JsonValueTest, DefaultConstructedIsNull)
{
    JsonValue value;
    EXPECT_TRUE(value.IsNull());
    EXPECT_EQ(value.GetType(), JsonValue::Type::Null);
}

TEST(JsonValueTest, NullptrConstructedIsNull)
{
    JsonValue value(nullptr);
    EXPECT_TRUE(value.IsNull());
}

TEST(JsonValueTest, BooleanConstruction)
{
    JsonValue value(true);
    EXPECT_TRUE(value.IsBoolean());
    EXPECT_TRUE(value.AsBoolean());

    JsonValue falseValue(false);
    EXPECT_TRUE(falseValue.IsBoolean());
    EXPECT_FALSE(falseValue.AsBoolean());
}

TEST(JsonValueTest, NumberConstructionFromDoubleAndInt)
{
    JsonValue doubleValue(3.14);
    EXPECT_TRUE(doubleValue.IsNumber());
    EXPECT_DOUBLE_EQ(doubleValue.AsNumber(), 3.14);

    JsonValue intValue(42);
    EXPECT_TRUE(intValue.IsNumber());
    EXPECT_DOUBLE_EQ(intValue.AsNumber(), 42.0);
}

TEST(JsonValueTest, StringConstructionFromStdStringAndCString)
{
    JsonValue stringValue(std::string("hello"));
    EXPECT_TRUE(stringValue.IsString());
    EXPECT_EQ(stringValue.AsString(), "hello");

    JsonValue cStringValue("world");
    EXPECT_TRUE(cStringValue.IsString());
    EXPECT_EQ(cStringValue.AsString(), "world");
}

TEST(JsonValueTest, ArrayConstructionAndIndexing)
{
    JsonValue::ArrayType elements;
    elements.push_back(JsonValue(1));
    elements.push_back(JsonValue("two"));
    elements.push_back(JsonValue(true));

    JsonValue array(elements);
    EXPECT_TRUE(array.IsArray());
    ASSERT_EQ(array.AsArray().size(), 3u);
    EXPECT_DOUBLE_EQ(array[0].AsNumber(), 1.0);
    EXPECT_EQ(array[1].AsString(), "two");
    EXPECT_TRUE(array[2].AsBoolean());
}

TEST(JsonValueTest, MakeArrayStartsEmptyAndSupportsPushBack)
{
    JsonValue array = JsonValue::MakeArray();
    EXPECT_TRUE(array.IsArray());
    EXPECT_EQ(array.AsArray().size(), 0u);

    array.AsArray().push_back(JsonValue(1));
    EXPECT_EQ(array.AsArray().size(), 1u);
}

TEST(JsonValueTest, ObjectConstructionAndKeyAccess)
{
    JsonValue object = JsonValue::MakeObject();
    EXPECT_TRUE(object.IsObject());

    object["name"] = JsonValue("Alice");
    object["age"] = JsonValue(30);

    EXPECT_TRUE(object.HasKey("name"));
    EXPECT_TRUE(object.HasKey("age"));
    EXPECT_FALSE(object.HasKey("missing"));
    EXPECT_EQ(object["name"].AsString(), "Alice");
    EXPECT_DOUBLE_EQ(object["age"].AsNumber(), 30.0);
}

TEST(JsonValueTest, ConstObjectKeyAccessThrowsWhenKeyMissing)
{
    JsonValue object = JsonValue::MakeObject();
    object["name"] = JsonValue("Alice");

    const JsonValue& constObject = object;
    EXPECT_EQ(constObject["name"].AsString(), "Alice");
    EXPECT_THROW(constObject["missing"], JsonTypeException);
}

TEST(JsonValueTest, MismatchedAccessorThrows)
{
    JsonValue value(true);
    EXPECT_THROW(value.AsNumber(), JsonTypeException);
    EXPECT_THROW(value.AsString(), JsonTypeException);
    EXPECT_THROW(value.AsArray(), JsonTypeException);
    EXPECT_THROW(value.AsObject(), JsonTypeException);
}

TEST(JsonValueTest, ArrayIndexOutOfRangeThrows)
{
    JsonValue array = JsonValue::MakeArray();
    array.AsArray().push_back(JsonValue(1));

    EXPECT_NO_THROW(array[0]);
    EXPECT_THROW(array[1], JsonTypeException);
}

TEST(JsonValueTest, IndexingNonArrayNonObjectThrows)
{
    JsonValue value(42);
    EXPECT_THROW(value[0], JsonTypeException);
    EXPECT_THROW(value["key"], JsonTypeException);
}

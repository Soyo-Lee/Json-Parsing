#ifdef _DEBUG

#include "gmock/gmock.h"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

#else

#include "JsonParser.h"
#include "JsonWriter.h"

#include <cstdio>

int main()
{
    JsonValue person = JsonValue::MakeObject();
    person["name"] = JsonValue("Alice");
    person["age"] = JsonValue(30);
    person["isStudent"] = JsonValue(false);
    person["nickname"] = JsonValue();

    JsonValue::ArrayType scores;
    scores.push_back(JsonValue(95));
    scores.push_back(JsonValue(88.5));
    scores.push_back(JsonValue(100));
    person["scores"] = JsonValue(scores);

    const char* path = "person.json";
    JsonWriter::SaveToFile(person, path, /*pretty=*/true);
    printf("Saved to %s:\n%s\n\n", path, JsonWriter::Write(person, /*pretty=*/true).c_str());

    JsonValue reparsed = JsonParser::ParseFile(path);
    printf("Reparsed from %s:\n", path);
    printf("  name: %s\n", reparsed["name"].AsString().c_str());
    printf("  age: %.0f\n", reparsed["age"].AsNumber());
    printf("  isStudent: %s\n", reparsed["isStudent"].AsBoolean() ? "true" : "false");
    printf("  scores[1]: %.1f\n", reparsed["scores"][1].AsNumber());

    return 0;
}

#endif

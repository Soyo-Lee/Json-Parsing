#include "JsonFileRepository.h"

#include <cstdio>
#include <gtest/gtest.h>

namespace
{
class JsonFileRepositoryTest : public ::testing::Test
{
protected:
    void TearDown() override
    {
        std::remove(path_.c_str());
    }

    std::string path_ = "json_file_repository_test.json";
};
} // namespace

TEST_F(JsonFileRepositoryTest, StartsEmptyWhenFileDoesNotExist)
{
    JsonFileRepository repo(path_);
    EXPECT_EQ(repo.ReadAll().size(), 0u);
}

TEST_F(JsonFileRepositoryTest, CreateAssignsIdWhenMissing)
{
    JsonFileRepository repo(path_);

    JsonValue record = JsonValue::MakeObject();
    record["name"] = JsonValue("Alice");

    JsonValue created = repo.Create(record);
    EXPECT_TRUE(created.HasKey("id"));
    EXPECT_EQ(created["id"].AsString(), "1");
    EXPECT_EQ(created["name"].AsString(), "Alice");
}

TEST_F(JsonFileRepositoryTest, CreateKeepsProvidedId)
{
    JsonFileRepository repo(path_);

    JsonValue record = JsonValue::MakeObject();
    record["id"] = JsonValue("custom-id");
    record["name"] = JsonValue("Bob");

    JsonValue created = repo.Create(record);
    EXPECT_EQ(created["id"].AsString(), "custom-id");
}

TEST_F(JsonFileRepositoryTest, CreateAssignsSequentialIds)
{
    JsonFileRepository repo(path_);

    JsonValue first = repo.Create(JsonValue::MakeObject());
    JsonValue second = repo.Create(JsonValue::MakeObject());

    EXPECT_EQ(first["id"].AsString(), "1");
    EXPECT_EQ(second["id"].AsString(), "2");
}

TEST_F(JsonFileRepositoryTest, CreateThrowsWhenRecordIsNotObject)
{
    JsonFileRepository repo(path_);
    EXPECT_THROW(repo.Create(JsonValue(1)), JsonTypeException);
}

TEST_F(JsonFileRepositoryTest, ReadAllReturnsAllCreatedRecords)
{
    JsonFileRepository repo(path_);

    JsonValue a = JsonValue::MakeObject();
    a["name"] = JsonValue("Alice");
    JsonValue b = JsonValue::MakeObject();
    b["name"] = JsonValue("Bob");

    repo.Create(a);
    repo.Create(b);

    ASSERT_EQ(repo.ReadAll().size(), 2u);
    EXPECT_EQ(repo.ReadAll()[0]["name"].AsString(), "Alice");
    EXPECT_EQ(repo.ReadAll()[1]["name"].AsString(), "Bob");
}

TEST_F(JsonFileRepositoryTest, ReadByIdFindsRecord)
{
    JsonFileRepository repo(path_);
    JsonValue created = repo.Create(JsonValue::MakeObject());

    const JsonValue* found = repo.ReadById(created["id"].AsString());
    ASSERT_NE(found, nullptr);
    EXPECT_EQ((*found)["id"].AsString(), created["id"].AsString());
}

TEST_F(JsonFileRepositoryTest, ReadByIdReturnsNullptrWhenMissing)
{
    JsonFileRepository repo(path_);
    repo.Create(JsonValue::MakeObject());

    EXPECT_EQ(repo.ReadById("no-such-id"), nullptr);
}

TEST_F(JsonFileRepositoryTest, CreatePersistsAcrossInstances)
{
    {
        JsonFileRepository repo(path_);
        JsonValue record = JsonValue::MakeObject();
        record["name"] = JsonValue("Carol");
        repo.Create(record);
    }

    JsonFileRepository reopened(path_);
    ASSERT_EQ(reopened.ReadAll().size(), 1u);
    EXPECT_EQ(reopened.ReadAll()[0]["name"].AsString(), "Carol");
}

TEST_F(JsonFileRepositoryTest, UpdateMergesFieldsIntoExistingRecord)
{
    JsonFileRepository repo(path_);
    JsonValue record = JsonValue::MakeObject();
    record["name"] = JsonValue("Alice");
    record["age"] = JsonValue(30);
    JsonValue created = repo.Create(record);

    JsonValue::ObjectType fields;
    fields.emplace_back("age", JsonValue(31));
    bool updated = repo.Update(created["id"].AsString(), fields);

    EXPECT_TRUE(updated);
    const JsonValue* found = repo.ReadById(created["id"].AsString());
    ASSERT_NE(found, nullptr);
    EXPECT_EQ((*found)["name"].AsString(), "Alice");
    EXPECT_DOUBLE_EQ((*found)["age"].AsNumber(), 31.0);
}

TEST_F(JsonFileRepositoryTest, UpdateAddsNewFieldNotPreviouslyPresent)
{
    JsonFileRepository repo(path_);
    JsonValue created = repo.Create(JsonValue::MakeObject());

    JsonValue::ObjectType fields;
    fields.emplace_back("email", JsonValue("alice@example.com"));
    repo.Update(created["id"].AsString(), fields);

    const JsonValue* found = repo.ReadById(created["id"].AsString());
    ASSERT_NE(found, nullptr);
    EXPECT_EQ((*found)["email"].AsString(), "alice@example.com");
}

TEST_F(JsonFileRepositoryTest, UpdateReturnsFalseWhenIdMissing)
{
    JsonFileRepository repo(path_);
    JsonValue::ObjectType fields;
    fields.emplace_back("name", JsonValue("Nobody"));

    EXPECT_FALSE(repo.Update("no-such-id", fields));
}

TEST_F(JsonFileRepositoryTest, UpdatePersistsAcrossInstances)
{
    std::string id;
    {
        JsonFileRepository repo(path_);
        JsonValue created = repo.Create(JsonValue::MakeObject());
        id = created["id"].AsString();

        JsonValue::ObjectType fields;
        fields.emplace_back("name", JsonValue("Dave"));
        repo.Update(id, fields);
    }

    JsonFileRepository reopened(path_);
    const JsonValue* found = reopened.ReadById(id);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ((*found)["name"].AsString(), "Dave");
}

TEST_F(JsonFileRepositoryTest, DeleteRemovesRecord)
{
    JsonFileRepository repo(path_);
    JsonValue created = repo.Create(JsonValue::MakeObject());

    bool deleted = repo.Delete(created["id"].AsString());

    EXPECT_TRUE(deleted);
    EXPECT_EQ(repo.ReadById(created["id"].AsString()), nullptr);
    EXPECT_EQ(repo.ReadAll().size(), 0u);
}

TEST_F(JsonFileRepositoryTest, DeleteOnlyRemovesMatchingRecord)
{
    JsonFileRepository repo(path_);
    JsonValue first = repo.Create(JsonValue::MakeObject());
    JsonValue second = repo.Create(JsonValue::MakeObject());

    repo.Delete(first["id"].AsString());

    EXPECT_EQ(repo.ReadAll().size(), 1u);
    EXPECT_NE(repo.ReadById(second["id"].AsString()), nullptr);
}

TEST_F(JsonFileRepositoryTest, DeleteReturnsFalseWhenIdMissing)
{
    JsonFileRepository repo(path_);
    EXPECT_FALSE(repo.Delete("no-such-id"));
}

TEST_F(JsonFileRepositoryTest, DeletePersistsAcrossInstances)
{
    std::string id;
    {
        JsonFileRepository repo(path_);
        JsonValue created = repo.Create(JsonValue::MakeObject());
        id = created["id"].AsString();
        repo.Delete(id);
    }

    JsonFileRepository reopened(path_);
    EXPECT_EQ(reopened.ReadAll().size(), 0u);
}

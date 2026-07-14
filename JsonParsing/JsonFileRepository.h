#pragma once

#include "ICrudRepository.h"
#include "JsonValue.h"

#include <string>

using std::string;

// JSON 파일을 저장소로 쓰는 ICrudRepository 구현체.
// 레코드는 idField(기본값 "id") 문자열 필드로 식별되는 JsonValue Object들의 배열로 저장된다.
class JsonFileRepository : public ICrudRepository
{
public:
    explicit JsonFileRepository(string filePath, string idField = "id");

    JsonValue Create(JsonValue record) override;
    const JsonValue::ArrayType& ReadAll() const override;
    const JsonValue* ReadById(const string& id) const override;
    bool Update(const string& id, const JsonValue::ObjectType& fields) override;
    bool Delete(const string& id) override;

private:
    void Load();
    void Save() const;
    string GenerateNextId() const;

    string filePath_;
    string idField_;
    JsonValue records_;
};

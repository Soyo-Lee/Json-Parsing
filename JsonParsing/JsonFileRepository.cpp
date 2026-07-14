#include "JsonFileRepository.h"

#include "JsonParser.h"
#include "JsonWriter.h"

#include <algorithm>
#include <fstream>

using std::exception;
using std::ifstream;
using std::ios;
using std::max;
using std::move;
using std::stoull;
using std::to_string;

JsonFileRepository::JsonFileRepository(string filePath, string idField)
    : filePath_(move(filePath)), idField_(move(idField)), records_(JsonValue::MakeArray())
{
    Load();
}

void JsonFileRepository::Load()
{
    ifstream existing(filePath_, ios::binary);
    if (existing)
    {
        records_ = JsonParser::ParseFile(filePath_);
    }
    else
    {
        records_ = JsonValue::MakeArray();
    }
}

void JsonFileRepository::Save() const
{
    JsonWriter::SaveToFile(records_, filePath_, /*pretty=*/true);
}

string JsonFileRepository::GenerateNextId() const
{
    unsigned long long maxId = 0;
    for (const JsonValue& entry : records_.AsArray())
    {
        if (entry.IsObject() && entry.HasKey(idField_) && entry[idField_].IsString())
        {
            try
            {
                maxId = max(maxId, stoull(entry[idField_].AsString()));
            }
            catch (const exception&)
            {
                // Non-numeric id (e.g. a caller-supplied id): ignore it for auto-numbering purposes.
            }
        }
    }
    return to_string(maxId + 1);
}

JsonValue JsonFileRepository::Create(JsonValue record)
{
    if (!record.IsObject())
    {
        throw JsonTypeException("JsonFileRepository::Create requires an Object record");
    }

    if (!record.HasKey(idField_))
    {
        record[idField_] = JsonValue(GenerateNextId());
    }

    records_.AsArray().push_back(record);
    Save();
    return record;
}

const JsonValue::ArrayType& JsonFileRepository::ReadAll() const
{
    return records_.AsArray();
}

const JsonValue* JsonFileRepository::ReadById(const string& id) const
{
    for (const JsonValue& entry : records_.AsArray())
    {
        if (entry.IsObject() && entry.HasKey(idField_) && entry[idField_].AsString() == id)
        {
            return &entry;
        }
    }
    return nullptr;
}

bool JsonFileRepository::Update(const string& id, const JsonValue::ObjectType& fields)
{
    for (JsonValue& entry : records_.AsArray())
    {
        if (entry.IsObject() && entry.HasKey(idField_) && entry[idField_].AsString() == id)
        {
            for (const auto& field : fields)
            {
                entry[field.first] = field.second;
            }
            Save();
            return true;
        }
    }
    return false;
}

bool JsonFileRepository::Delete(const string& id)
{
    JsonValue::ArrayType& array = records_.AsArray();
    for (auto it = array.begin(); it != array.end(); ++it)
    {
        if (it->IsObject() && it->HasKey(idField_) && (*it)[idField_].AsString() == id)
        {
            array.erase(it);
            Save();
            return true;
        }
    }
    return false;
}

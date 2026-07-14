#ifdef _DEBUG

#include "gmock/gmock.h"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

#else

#include "JsonFileRepository.h"
#include "JsonWriter.h"

#include <iostream>
#include <string>

namespace
{

void PrintRecord(const JsonValue& record)
{
    std::cout << JsonWriter::Write(record, /*pretty=*/true) << "\n";
}

JsonValue::ObjectType ReadFieldsFromConsole()
{
    std::cout << "필드를 \"키=값\" 형태로 한 줄씩 입력하세요. 빈 줄을 입력하면 종료합니다.\n";
    JsonValue::ObjectType fields;
    std::string line;
    while (std::getline(std::cin, line) && !line.empty())
    {
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos)
        {
            std::cout << "형식이 올바르지 않습니다 (키=값). 다시 입력하세요.\n";
            continue;
        }
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        fields.emplace_back(std::move(key), JsonValue(std::move(value)));
    }
    return fields;
}

void HandleCreate(JsonFileRepository& repo)
{
    JsonValue record(ReadFieldsFromConsole());
    JsonValue created = repo.Create(record);
    std::cout << "생성됨:\n";
    PrintRecord(created);
}

void HandleReadAll(const JsonFileRepository& repo)
{
    const auto& all = repo.ReadAll();
    if (all.empty())
    {
        std::cout << "저장된 데이터가 없습니다.\n";
        return;
    }
    for (const JsonValue& record : all)
    {
        PrintRecord(record);
    }
}

void HandleReadById(const JsonFileRepository& repo)
{
    std::cout << "조회할 ID를 입력하세요: ";
    std::string id;
    std::getline(std::cin, id);

    const JsonValue* found = repo.ReadById(id);
    if (found == nullptr)
    {
        std::cout << "해당 ID의 데이터를 찾을 수 없습니다.\n";
        return;
    }
    PrintRecord(*found);
}

void HandleRead(const JsonFileRepository& repo)
{
    std::cout << "1. 전체 목록 보기\n2. ID로 검색\n선택: ";
    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "2")
    {
        HandleReadById(repo);
    }
    else
    {
        HandleReadAll(repo);
    }
}

void HandleUpdate(JsonFileRepository& repo)
{
    std::cout << "수정할 ID를 입력하세요: ";
    std::string id;
    std::getline(std::cin, id);

    if (repo.ReadById(id) == nullptr)
    {
        std::cout << "해당 ID의 데이터를 찾을 수 없습니다.\n";
        return;
    }

    JsonValue::ObjectType fields = ReadFieldsFromConsole();
    bool updated = repo.Update(id, fields);
    std::cout << (updated ? "수정되었습니다.\n" : "수정에 실패했습니다.\n");
}

void HandleDelete(JsonFileRepository& repo)
{
    std::cout << "삭제할 ID를 입력하세요: ";
    std::string id;
    std::getline(std::cin, id);

    std::cout << "정말 삭제하시겠습니까? (y/n): ";
    std::string confirm;
    std::getline(std::cin, confirm);
    if (confirm != "y" && confirm != "Y")
    {
        std::cout << "삭제를 취소했습니다.\n";
        return;
    }

    bool deleted = repo.Delete(id);
    std::cout << (deleted ? "삭제되었습니다.\n" : "해당 ID의 데이터를 찾을 수 없습니다.\n");
}

void PrintMenu()
{
    std::cout << "\n=== JSON CRUD 콘솔 앱 ===\n"
                  "1. Create - 새 데이터 추가\n"
                  "2. Read   - 조회 (전체 목록 / ID 검색)\n"
                  "3. Update - 데이터 수정\n"
                  "4. Delete - 데이터 삭제\n"
                  "0. 종료\n"
                  "선택: ";
}

} // namespace

int main()
{
    JsonFileRepository repo("records.json");

    std::string choice;
    while (PrintMenu(), std::getline(std::cin, choice))
    {
        if (choice == "1") HandleCreate(repo);
        else if (choice == "2") HandleRead(repo);
        else if (choice == "3") HandleUpdate(repo);
        else if (choice == "4") HandleDelete(repo);
        else if (choice == "0") break;
        else std::cout << "올바른 번호를 입력하세요.\n";
    }

    std::cout << "종료합니다.\n";
    return 0;
}

#endif

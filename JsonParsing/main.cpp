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

#include <Windows.h>

#include <iostream>
#include <string>

using std::cin;
using std::cout;
using std::getline;
using std::move;
using std::string;

namespace
{

// getline만 쓰면 CRLF로 리다이렉트된 입력에서 개행 앞의 '\r'이 문자열 끝에
// 남아 "1" 같은 비교가 "1\r"과 맞지 않아 실패한다. 콘솔에서 사람이 직접 입력할
// 때는 보통 문제되지 않지만, 파일/파이프 입력을 대비해 항상 이 함수로 읽는다.
bool ReadLine(string& out)
{
    if (!getline(cin, out))
    {
        return false;
    }
    if (!out.empty() && out.back() == '\r')
    {
        out.pop_back();
    }
    return true;
}

void PrintRecord(const JsonValue& record)
{
    cout << JsonWriter::Write(record, /*pretty=*/true) << "\n";
}

JsonValue::ObjectType ReadFieldsFromConsole()
{
    cout << "필드를 \"키=값\" 형태로 한 줄씩 입력하세요. 빈 줄을 입력하면 종료합니다.\n";
    JsonValue::ObjectType fields;
    string line;
    while (ReadLine(line) && !line.empty())
    {
        size_t equalsPos = line.find('=');
        if (equalsPos == string::npos)
        {
            cout << "형식이 올바르지 않습니다 (키=값). 다시 입력하세요.\n";
            continue;
        }
        string key = line.substr(0, equalsPos);
        string value = line.substr(equalsPos + 1);
        fields.emplace_back(move(key), JsonValue(move(value)));
    }
    return fields;
}

void HandleCreate(JsonFileRepository& repo)
{
    JsonValue record(ReadFieldsFromConsole());
    JsonValue created = repo.Create(record);
    cout << "생성됨:\n";
    PrintRecord(created);
}

void HandleReadAll(const JsonFileRepository& repo)
{
    const auto& all = repo.ReadAll();
    if (all.empty())
    {
        cout << "저장된 데이터가 없습니다.\n";
        return;
    }
    for (const JsonValue& record : all)
    {
        PrintRecord(record);
    }
}

void HandleReadById(const JsonFileRepository& repo)
{
    cout << "조회할 ID를 입력하세요: ";
    string id;
    ReadLine(id);

    const JsonValue* found = repo.ReadById(id);
    if (found == nullptr)
    {
        cout << "해당 ID의 데이터를 찾을 수 없습니다.\n";
        return;
    }
    PrintRecord(*found);
}

void HandleRead(const JsonFileRepository& repo)
{
    cout << "1. 전체 목록 보기\n2. ID로 검색\n선택: ";
    string choice;
    ReadLine(choice);

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
    cout << "수정할 ID를 입력하세요: ";
    string id;
    ReadLine(id);

    if (repo.ReadById(id) == nullptr)
    {
        cout << "해당 ID의 데이터를 찾을 수 없습니다.\n";
        return;
    }

    JsonValue::ObjectType fields = ReadFieldsFromConsole();
    bool updated = repo.Update(id, fields);
    cout << (updated ? "수정되었습니다.\n" : "수정에 실패했습니다.\n");
}

void HandleDelete(JsonFileRepository& repo)
{
    cout << "삭제할 ID를 입력하세요: ";
    string id;
    ReadLine(id);

    cout << "정말 삭제하시겠습니까? (y/n): ";
    string confirm;
    ReadLine(confirm);
    if (confirm != "y" && confirm != "Y")
    {
        cout << "삭제를 취소했습니다.\n";
        return;
    }

    bool deleted = repo.Delete(id);
    cout << (deleted ? "삭제되었습니다.\n" : "해당 ID의 데이터를 찾을 수 없습니다.\n");
}

void PrintMenu()
{
    cout << "\n=== JSON CRUD 콘솔 앱 ===\n"
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
    // 소스 문자열 리터럴은 /utf-8 컴파일 옵션으로 UTF-8 바이트로 인코딩된다.
    // 콘솔의 입출력 코드페이지도 UTF-8(65001)로 맞추지 않으면 시스템 기본
    // 코드페이지(한국어 Windows는 CP949)로 잘못 해석되어 한글이 깨진다.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    JsonFileRepository repo("records.json");

    string choice;
    while (PrintMenu(), ReadLine(choice))
    {
        if (choice == "1") HandleCreate(repo);
        else if (choice == "2") HandleRead(repo);
        else if (choice == "3") HandleUpdate(repo);
        else if (choice == "4") HandleDelete(repo);
        else if (choice == "0") break;
        else cout << "올바른 번호를 입력하세요.\n";
    }

    cout << "종료합니다.\n";
    return 0;
}

#endif

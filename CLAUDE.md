# CLAUDE.md

이 파일은 이 저장소에서 작업할 때 Claude Code(claude.ai/code)에게 제공하는 가이드입니다.

## 프로젝트 개요

JsonParsing은 Windows 콘솔 애플리케이션(C++20, Visual Studio)으로, 외부 라이브러리(nlohmann/json 등) 없이 JSON 파싱/직렬화를 직접 구현하고, 그 위에 JSON 파일을 저장소로 쓰는 CRUD(Create/Read/Update/Delete) 리포지토리를 얹은 프로젝트다. `[CRA_AI] Day2_1_Agentic Engineering.pdf` 32페이지 과제(PoC 구조를 유지한 채 JSON 파일 기반 CRUD 콘솔 앱 개발)를 기반으로 한다.

## 빌드

CMake/Makefile 없이 Visual Studio MSBuild 프로젝트로 구성되어 있다. Visual Studio 또는 MSBuild로 직접 빌드한다:

```
msbuild JsonParsing.slnx /p:Configuration=Debug /p:Platform=x64
msbuild JsonParsing.slnx /p:Configuration=Release /p:Platform=x64
```

지원 플랫폼: `x64`, `Win32`(x86). 툴셋 `v145`, 언어 표준 `stdcpp20`. `ConfigurationType`은 `Application`(콘솔 `.exe`)이며 정적/동적 라이브러리 타겟이 아니다 — 여기서 "라이브러리"는 별도의 `.lib`/`.dll` 프로젝트가 아니라 콘솔 앱에 링크되는 내부 JSON 모듈을 의미한다.

## 인코딩(한글 깨짐 방지)

- 모든 파일(소스 코드, `docs/*.md`, `CLAUDE.md` 등)은 **BOM 없는 UTF-8**로 저장한다. Windows 기본 코드페이지(949, CP949/EUC-KR)로 읽거나 저장하면 한글이 깨진다.
- C++ 소스에 한글 문자열 리터럴(예: 콘솔 프롬프트 텍스트)을 추가할 계획이라면, `AssemblyCar` 프로젝트와 동일하게 컴파일러가 소스를 UTF-8로 해석하도록 강제해야 한다. `JsonParsing.vcxproj`의 4개 빌드 구성(Debug/Release × x86/x64) 모두 `<AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>`를 이미 지정하고 있다 — 이 옵션을 제거하지 말 것.
- 이 저장소는 아직 Korean 콘솔 UI 문자열을 포함하지 않지만(Phase 5에서 메뉴 텍스트 추가 예정), 추가 시 위 두 조건(UTF-8 무BOM 저장 + `/utf-8` 컴파일 옵션)을 반드시 함께 지켜야 한다.
- 터미널/도구에서 한글이 깨져 보이면 십중팔구 파일 인코딩이나 도구의 로케일 가정(예: `pdftotext`가 기본 인코딩으로 추출) 문제이지, 내용 자체가 손상된 것은 아니다 — 원본 파일을 UTF-8로 다시 열어 확인한다.

## 실행

Release에서 `main()`은 예시 `JsonValue` 오브젝트 트리를 만들어 `JsonWriter::SaveToFile`로 `person.json`에 저장(pretty-print)한 뒤, `JsonParser::ParseFile`로 다시 읽어 일부 필드를 출력하는 왕복(round-trip) 데모를 실행한다. Debug에서는 `main()`이 gtest 실행기 역할을 한다(아래 Tests 참고).

## 테스트

GoogleTest/GoogleMock은 vcpkg 매니페스트(`vcpkg.json`, vcxproj의 `VcpkgEnableManifest=true`)로 받아온다 — 첫 Debug 빌드 시 `gtest`/`gmock`을 자동으로 fetch/build한다(네트워크 필요). `main.cpp`는 형제 프로젝트 `AssemblyCar`와 동일하게 `#ifdef _DEBUG` 분기를 쓴다: Debug에서는 `testing::InitGoogleTest` + `testing::InitGoogleMock` + `RUN_ALL_TESTS()`를 실행, Release에서는 위의 왕복 데모를 실행한다. 즉 `x64/Debug/JsonParsing.exe`를 실행하는 것 자체가 테스트 실행이다.

개발은 TDD로 진행하며, `docs/phase1.md`(JsonValue) ~ `docs/phase5.md`(CRUD Update/Delete + 콘솔 앱)에 단계별로 기록한다. 각 단계는 테스트(RED)를 먼저 작성한 뒤 구현(GREEN)한다.

## 아키텍처

- `JsonValue.h`/`.cpp` — 값 타입. null/boolean/number(`double`)/string/array/object를 `std::variant<std::monostate, bool, double, std::string, ArrayType, ObjectType>`로 표현한다. `ArrayType = std::vector<JsonValue>`, `ObjectType = std::vector<std::pair<std::string, JsonValue>>`(삽입 순서 보존, `std::map`과 다름). 타입 확인(`IsNull`/`IsBoolean`/...), 타입별 접근자(`AsBoolean`/`AsNumber`/`AsString`/`AsArray`/`AsObject`), 배열 인덱스/객체 키용 `operator[]`. 타입이 맞지 않으면 `JsonTypeException`을 던지며, 객체에 없는 키로 접근 시 `const` 버전은 예외를 던지고 비-`const` 버전은 `std::map::operator[]`처럼 `Null` 항목을 자동 삽입한다.
- `JsonValueTest.cpp` — 생성, 타입 판별, 접근자, 인덱싱, 예외 동작에 대한 gtest 케이스.
- `JsonParser.h`/`.cpp` — 재귀 하강 파서, JSON 텍스트 → `JsonValue` 트리. `Parse(text)`는 문자열을, `ParseFile(path)`는 파일을 읽어 파싱한다. null/bool/number(음수·소수·지수 포함)/string(이스케이프, `\uXXXX` 포함 — BMP만 지원, 서로게이트 쌍 미지원)/array/object를 처리하며 토큰 사이 공백을 건너뛴다. 구문 오류(빈 입력, 잘못된 리터럴, 종료되지 않은 문자열, 짝이 안 맞는 괄호, 후행 문자, 파일 없음)는 실패 위치(byte offset)를 담은 `JsonParseException`을 던진다.
- `JsonParserTest.cpp` — 각 값 타입, 중첩, 공백 허용, 위 오류 경로들을 다루는 gtest 케이스.
- `JsonWriter.h`/`.cpp` — `JsonValue` 트리 → JSON 텍스트. `Write(value, pretty, indentSize)`는 문자열로 직렬화(기본은 compact), `SaveToFile(value, path, pretty, indentSize)`는 파일로 저장하며 파일을 열거나 쓸 수 없으면 `JsonWriteException`을 던진다. 숫자는 `std::to_chars`로 최단 왕복 표현을 사용(정수는 `.0` 없이 출력); 문자열은 `"`, `\`, 제어 문자를 다시 이스케이프한다.
- `JsonWriterTest.cpp` — 각 값 타입 출력, compact/pretty 중첩, `JsonParser`를 통한 왕복(메모리 상/파일 저장 양쪽)을 다루는 gtest 케이스.
- `ICrudRepository.h` — 저장 방식(파일/DB 등)에 무관하게 호출 측이 의존하는 CRUD 인터페이스. `Create(record)`(id 없으면 자동 채번), `ReadAll()`, `ReadById(id)`(없으면 `nullptr`), `Update(id, fields)`, `Delete(id)` 네 메서드를 선언한다.
- `JsonFileRepository.h`/`.cpp` — `ICrudRepository`의 JSON 파일 기반 구현체. 생성자에서 파일이 있으면 `JsonParser::ParseFile`로 불러오고 없으면 빈 배열로 시작한다. 레코드는 `idField`(기본값 `"id"`) 문자열 필드로 식별되는 `JsonValue` Object들의 배열이며, `Create`가 성공할 때마다 `JsonWriter::SaveToFile`로 즉시 저장한다. `Update`/`Delete`는 아직 미구현으로 `CrudNotImplementedException`을 던지는 자리표시자다(Phase 5에서 구현 예정, `docs/phase5.md` 참고).
- `JsonFileRepositoryTest.cpp` — id 자동/수동 채번, 목록/단건 조회, 인스턴스 간 파일 영속성, 잘못된 타입, Update/Delete 미구현 확인을 다루는 gtest 케이스.

새 파일이 추가될 때마다 위 패턴(파일명 — 무엇을 담당하는지)으로 이 섹션을 갱신한다.

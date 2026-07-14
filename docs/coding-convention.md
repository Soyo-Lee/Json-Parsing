# C++ 코딩 컨벤션

이 문서는 [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)와 [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)를 기반으로, 이 프로젝트에서 실제로 따르는 컨벤션을 정리한다. 두 가이드가 서로 다르게 권장하는 지점(예외 사용, `using` 선언)은 이 프로젝트의 결정을 명시하고 이유를 남긴다.

## 1. 파일 구성

- 헤더 가드는 `#pragma once`를 사용한다 (Core Guidelines [SF.8](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#sf8-use-include-guards-for-all-h-files)).
- 클래스 하나당 헤더(`.h`) + 구현(`.cpp`) 한 쌍으로 분리한다 (`JsonValue.h`/`.cpp`, `JsonParser.h`/`.cpp` 등). Google Style Guide의 [Header Files](https://google.github.io/styleguide/cppguide.html#Header_Files) 원칙을 따른다.
- 테스트는 대상 파일과 짝을 이루는 `<TypeName>Test.cpp`로 만든다 (`JsonValueTest.cpp` 등).
- `#include` 순서: 자기 자신의 헤더 → 프로젝트 내 다른 헤더 → 표준 라이브러리 헤더. 그룹 사이에 빈 줄을 둔다.

## 2. 네이밍

Google Style Guide의 [Naming](https://google.github.io/styleguide/cppguide.html#Naming) 규칙을 따른다.

| 대상 | 규칙 | 예시 |
|------|------|------|
| 타입(클래스/구조체/열거형) | `PascalCase` | `JsonValue`, `JsonParser`, `Type::Boolean` |
| 함수/메서드 | `PascalCase` | `AsBoolean()`, `ReadAll()`, `ParseFile()` |
| 지역 변수/매개변수 | `camelCase` | `filePath`, `idField`, `indentSize` |
| private 멤버 변수 | `camelCase` + 뒤에 `_` | `type_`, `value_`, `pos_`, `text_` |
| 상수/열거자 | `PascalCase` | `Type::Null`, `Type::Object` |

## 3. 서식(포매팅)

- 들여쓰기는 스페이스 4칸, 탭 사용 금지.
- 중괄호는 함수/클래스/제어문 모두 여는 중괄호를 다음 줄에 두는 Allman 스타일을 쓴다(이 프로젝트가 실제로 채택한 스타일이며, Google Style Guide의 [Formatting](https://google.github.io/styleguide/cppguide.html#Formatting) 기본값인 첨부형(attached brace)과 다르다는 점을 인지하고 일관되게 유지한다).
- 한 줄 길이는 100자를 넘지 않도록 한다(Core Guidelines에는 규정이 없으며, Google Style Guide의 기존 80자 규칙을 현대 모니터 환경에 맞게 완화한 값).

## 4. `std::` 네임스페이스 사용 — 예외 규정

Google Style Guide는 [Using-declarations](https://google.github.io/styleguide/cppguide.html#Using-Declarations)에서 헤더 파일 전역 스코프의 `using` 선언을 금지한다(다른 파일에 의도치 않게 이름이 새어나가기 때문). 이 프로젝트는 **팀 결정으로 이 규칙에 예외를 둔다**:

- `std::` 타입/함수를 사용할 때는 파일(헤더/구현) 상단, `#include` 바로 아래에 `using std::X;` 형태로 선언하고, 이후 본문에서는 `std::` 접두사 없이 사용한다.

  ```cpp
  #include <string>
  #include <vector>

  using std::string;
  using std::vector;

  class JsonValue
  {
      using ArrayType = vector<JsonValue>;
      const string& AsString() const;
      ...
  };
  ```

- 헤더에 선언하면 그 헤더를 포함하는 모든 파일에 이름이 노출된다는 점(Google이 우려하는 바로 그 문제)을 인지하고 감수한다. 여러 헤더가 같은 `using std::string;`을 중복 선언해도 동일 선언 재선언은 오류가 아니므로 문제없다.
- `using namespace std;`처럼 네임스페이스 전체를 끌어오는 것은 금지한다 — 반드시 이름 단위로 `using std::X;`를 선언한다.
- 지역 변수 이름이 `using`으로 끌어온 이름과 같아도(예: `JsonWriter.cpp`의 지역 변수 `array`가 `using std::array;`와 이름이 겹침) 지역 스코프가 우선하므로 허용한다. 다만 실제로 혼동을 준다면 변수명을 바꾼다.

## 5. 오류 처리 — 예외 사용

Google Style Guide는 [Exceptions](https://google.github.io/styleguide/cppguide.html#Exceptions)에서 예외 사용을 원칙적으로 금지한다(기존의 방대한 예외 미사용 코드베이스와의 호환성이 이유). 이 프로젝트는 **팀 결정으로 예외를 사용한다** — Core Guidelines의 [E.2](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#e2-throw-an-exception-to-signal-that-a-function-cant-perform-its-assigned-task) 원칙(정상 흐름으로 처리할 수 없는 실패는 예외로 알린다)을 따른다.

- 각 계층은 `std::runtime_error`/`std::logic_error`를 상속한 전용 예외 타입을 하나씩 둔다: `JsonTypeException`(`JsonValue`), `JsonParseException`(`JsonParser`), `JsonWriteException`(`JsonWriter`).
- 예외 메시지는 실패 원인과 (가능하다면) 위치 정보를 담는다 (`JsonParseException`은 실패한 byte offset을 포함).
- 반환값으로 성공/실패를 표현하는 것이 자연스러운 경우(예: `Update`/`Delete`가 대상 id를 못 찾은 경우)는 예외 대신 `bool` 반환을 쓴다 — "예외적인 상황"과 "정상적으로 발생 가능한 결과"를 구분한다 (Core Guidelines [E.3](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#e3-use-exceptions-for-error-handling-only)).

## 6. 클래스 설계

- Rule of Zero를 기본으로 한다: 소멸자/복사/이동 생성자를 직접 작성하지 않고 컴파일러 기본 동작에 맡긴다(Core Guidelines [C.20](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c20-if-you-can-avoid-defining-any-default-operations-do)). 예외적으로 인터페이스 기반 클래스는 가상 소멸자를 명시한다 (`ICrudRepository::~ICrudRepository() = default;`, Core Guidelines [C.127](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c127-a-class-with-a-virtual-function-should-have-a-virtual-or-protected-destructor)).
- 값 타입(`JsonValue`)의 단일 인자 생성자는 의도적으로 `explicit`을 붙이지 않는다 — `JsonValue value = "hello";`처럼 암시적 변환이 되는 것이 이 타입의 설계 목적이기 때문이다. Core Guidelines [C.46](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c46-by-default-declare-single-argument-constructors-explicit)의 기본 권장(암시적 변환 원치 않으면 `explicit`)과 다른 지점이므로, 새로 단일 인자 생성자를 추가할 때는 암시적 변환이 실제로 필요한지부터 확인한다.
- 값을 바꾸지 않는 메서드는 `const`로 선언한다. 참조로 전달해도 되는 인자는 `const T&`로 받는다 (Core Guidelines [Con.1](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#con1-by-default-make-objects-immutable), [F.16](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#f16-for-in-parameters-pass-cheaply-copied-types-by-value-and-others-by-reference-to-const)).
- 소유권이 있는 원시 포인터(`new`/`delete`, owning raw pointer)를 쓰지 않는다. 컨테이너(`vector`, `pair`)와 값 타입으로 소유권을 표현한다(Core Guidelines [R.1](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#r1-manage-resources-automatically-using-resource-handles-and-raii-guard-thanks-to-mike-mowbray)). `ReadById`처럼 소유권 없는 조회 결과는 관찰용 raw pointer(`const JsonValue*`, 없으면 `nullptr`)로 표현한다(Core Guidelines [F.60](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#f60-prefer-t-over-t-when-there-is-no-need-to-use-nullptr)의 관찰용 포인터 용례).

## 7. 테스트

- GoogleTest를 사용하고, 여러 테스트가 상태(임시 파일 경로 등)를 공유하면 `TEST()` 대신 `::testing::Test`를 상속한 fixture와 `TEST_F()`를 쓴다.
- 파일/외부 자원을 만드는 테스트는 fixture의 `TearDown()`에서 정리한다 — 개별 테스트 본문에 정리 코드를 반복하지 않는다.
- 테스트 이름은 `동작_조건` 순서로 서술적으로 짓는다 (`CreateAssignsIdWhenMissing`, `ThrowsOnUnterminatedString` 등).

## 8. 커밋 메시지

`docs/COMMIT_CONVENTION.md`를 따른다.

## 참고 자료

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

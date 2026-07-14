# Phase 1 — JsonValue (데이터 모델)

- `JsonValue.h` / `JsonValue.cpp`
- null, boolean, number(double), string, array, object 여섯 가지 타입을 표현하는 값 타입.
- `std::variant<std::monostate, bool, double, std::string, ArrayType, ObjectType>` 기반, `ArrayType = std::vector<JsonValue>`, `ObjectType = std::vector<std::pair<std::string, JsonValue>>` (삽입 순서 보존).
- 타입 확인(`IsNull`/`IsBoolean`/...), 값 접근(`AsBoolean`/`AsNumber`/`AsString`/`AsArray`/`AsObject`), 배열/객체 인덱싱(`operator[]`) 제공.
- 타입이 맞지 않는 접근은 `JsonTypeException`을 던진다.

## 상태

- [x] `JsonValue.h` 작성 완료
- [x] `JsonValueTest.cpp` 작성 완료 (RED)
- [x] `JsonValue.cpp` 작성 완료 (GREEN) — 12개 테스트 모두 통과
- [x] `main.cpp`에서 `_DEBUG` 빌드 시 gtest 실행하도록 연결

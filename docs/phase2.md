# Phase 2 — JsonParser (파싱)

- `JsonParser.h` / `JsonParser.cpp`
- JSON 문자열 → `JsonValue` 트리로 변환하는 재귀 하강 파서.
- `JsonParser::Parse(const std::string& text)` — 문자열 파싱.
- `JsonParser::ParseFile(const std::string& path)` — 파일을 읽어 파싱.
- 내부적으로 공백 스킵, 리터럴(`true`/`false`/`null`), 숫자, 문자열(이스케이프 포함), 배열, 객체를 각각 파싱하는 private 메서드로 구성.
- 문법 오류 시 위치 정보를 포함한 `JsonParseException`을 던진다.

## 상태

- [ ] `JsonParser.h` / `JsonParser.cpp` 작성

# Phase 2 — JsonParser (파싱)

- `JsonParser.h` / `JsonParser.cpp`
- JSON 문자열 → `JsonValue` 트리로 변환하는 재귀 하강 파서.
- `JsonParser::Parse(const std::string& text)` — 문자열 파싱.
- `JsonParser::ParseFile(const std::string& path)` — 파일을 읽어 파싱.
- 내부적으로 공백 스킵, 리터럴(`true`/`false`/`null`), 숫자, 문자열(이스케이프 포함), 배열, 객체를 각각 파싱하는 private 메서드로 구성.
- 문법 오류 시 위치 정보를 포함한 `JsonParseException`을 던진다.

## 상태

- [x] `JsonParserTest.cpp` 작성 완료 (RED) — 21개 테스트
- [x] `JsonParser.h` / `JsonParser.cpp` 작성 완료 (GREEN) — 재귀 하강 파서, `\uXXXX` 이스케이프는 BMP 범위만 UTF-8로 인코딩 (서로게이트 쌍 미지원)
- [x] 전체 테스트(33개) 통과 확인

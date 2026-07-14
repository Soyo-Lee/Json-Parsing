# Phase 3 — JsonWriter (직렬화/저장) + 데모

- `JsonWriter.h` / `JsonWriter.cpp`
- `JsonValue` 트리 → JSON 문자열로 직렬화 (`Write`, pretty-print 옵션 포함).
- `JsonWriter::SaveToFile(const JsonValue&, const std::string& path, bool pretty)` — 파일로 저장.
- `main.cpp`에서 값 트리를 구성 → 파일로 저장 → 다시 파싱해서 왕복(round-trip) 검증하는 데모 작성.

## 상태

- [x] `JsonWriterTest.cpp` 작성 완료 (RED) — 11개 테스트 (compact/pretty 출력, 이스케이프, 파서 왕복, 파일 저장/실패)
- [x] `JsonWriter.h` / `JsonWriter.cpp` 작성 완료 (GREEN) — `std::to_chars`로 숫자를 최단 왕복 표현으로 출력 (정수는 `.0` 없이)
- [x] `main.cpp`의 Release 분기에서 값 트리 구성 → 파일 저장 → 재파싱 데모 작성, Release 빌드로 직접 실행 확인
- [x] 전체 테스트(44개) 통과 확인

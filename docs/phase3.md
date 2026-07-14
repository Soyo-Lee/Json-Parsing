# Phase 3 — JsonWriter (직렬화/저장) + 데모

- `JsonWriter.h` / `JsonWriter.cpp`
- `JsonValue` 트리 → JSON 문자열로 직렬화 (`Write`, pretty-print 옵션 포함).
- `JsonWriter::SaveToFile(const JsonValue&, const std::string& path, bool pretty)` — 파일로 저장.
- `main.cpp`에서 값 트리를 구성 → 파일로 저장 → 다시 파싱해서 왕복(round-trip) 검증하는 데모 작성.

## 상태

- [ ] `JsonWriter.h` / `JsonWriter.cpp` 작성
- [ ] `main.cpp` 데모 작성

# Phase 5 — CRUD 리포지토리 (Update/Delete) 및 콘솔 앱

Phase 4에서 만든 `ICrudRepository`/`JsonFileRepository`에 실제 Update/Delete 동작을 채우고, PDF 32페이지 요구사항대로 콘솔 메뉴(전체 목록, ID 검색, 필드 수정, 삭제)를 통해 사용할 수 있게 연결한다.

## 예정 작업

- [ ] `JsonFileRepository::Update(id, fields)` 구현 — id로 레코드를 찾아 `fields`의 각 키를 병합 적용, 저장.
- [ ] `JsonFileRepository::Delete(id)` 구현 — id로 레코드를 찾아 배열에서 제거, 저장.
- [ ] Update/Delete에 대한 RED 테스트 작성 후 구현 (GREEN).
- [ ] 콘솔 메뉴(생성/조회/수정/삭제)를 `main.cpp` 또는 별도 앱 레이어에 연결.

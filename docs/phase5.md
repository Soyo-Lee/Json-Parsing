# Phase 5 — CRUD 리포지토리 (Update/Delete) 및 콘솔 앱

Phase 4에서 만든 `ICrudRepository`/`JsonFileRepository`에 실제 Update/Delete 동작을 채우고, PDF 32페이지 요구사항대로 콘솔 메뉴(전체 목록, ID 검색, 필드 수정, 삭제)를 통해 사용할 수 있게 연결한다.

## 상태

- [x] `JsonFileRepository::Update(id, fields)` 구현 — id로 레코드를 찾아 `fields`의 각 키를 병합 적용(없으면 추가, 있으면 덮어씀), 저장. id가 없으면 `false`.
- [x] `JsonFileRepository::Delete(id)` 구현 — id로 레코드를 찾아 배열에서 제거, 저장. id가 없으면 `false`.
- [x] Update/Delete RED 테스트(8개: 병합, 신규 필드 추가, 미존재 id, 영속성, 다른 레코드 보존 등) 작성 후 구현 (GREEN).
- [x] 더 이상 쓰이지 않는 `CrudNotImplementedException`을 `ICrudRepository.h`에서 제거.
- [x] 콘솔 메뉴(Create/Read/Update/Delete/종료)를 `main.cpp` Release 분기에 연결, Release 빌드로 전체 흐름(생성→조회→수정→삭제) 직접 실행 확인.
- [x] 전체 테스트(61개) 통과 확인.

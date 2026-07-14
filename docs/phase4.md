# Phase 4 — CRUD 리포지토리 (Create/Read)

`[CRA_AI] Day2_1_Agentic Engineering.pdf` 32페이지 과제: `JsonParsing` 라이브러리(PoC)의 구조를 유지한 채, JSON 파일로 데이터를 관리하는 CRUD 콘솔 애플리케이션을 만든다. 이번 단계에서는 향후 Update/Delete까지 이어 쓸 수 있는 인터페이스를 먼저 설계하고, Create/Read만 구현한다.

## 설계

- `ICrudRepository` — Create/Read/Update/Delete 네 개 메서드를 모두 선언한 추상 인터페이스. 호출 측은 항상 이 인터페이스에만 의존하도록 하여, 저장 방식(파일/DB 등)이 바뀌어도 영향받지 않게 한다.
  - `Create(JsonValue record)` — 레코드를 저장하고, id 필드가 없으면 자동 채번하여 반환한다.
  - `ReadAll()` — 전체 레코드 목록(`JsonValue::ArrayType`)을 반환한다.
  - `ReadById(id)` — id로 레코드를 찾는다. 없으면 `nullptr`.
  - `Update(id, fields)` — id로 찾은 레코드에 `fields`를 병합 적용한다. (다음 단계에서 구현)
  - `Delete(id)` — id로 레코드를 삭제한다. (다음 단계에서 구현)
- `JsonFileRepository` — `ICrudRepository`의 JSON 파일 기반 구현체.
  - 생성자에서 파일이 있으면 `JsonParser::ParseFile`로 불러오고, 없으면 빈 배열로 시작한다.
  - `Create`/`ReadAll`/`ReadById`를 구현하고, 변경이 생길 때마다 `JsonWriter::SaveToFile`로 즉시 저장한다.
  - `Update`/`Delete`는 이번 단계에서 `CrudNotImplementedException`을 던지는 자리표시자만 둔다 (Phase 5에서 구현 예정).

## 상태

- [x] `ICrudRepository.h` 작성 완료 (Create/Read/Update/Delete 네 메서드 모두 선언)
- [x] `JsonFileRepositoryTest.cpp` 작성 완료 (RED) — Create/Read 11개 테스트 (id 자동/수동 채번, 목록/단건 조회, 파일 영속성, 잘못된 타입, Update/Delete 미구현 확인 포함)
- [x] `JsonFileRepository.h` / `.cpp` 작성 완료 (GREEN) — Create/ReadAll/ReadById 구현, Update/Delete는 `CrudNotImplementedException`을 던지는 자리표시자
- [x] 전체 테스트(55개) 통과 확인

# PRD (Product Requirements Document)

> 출처: `[CRA_AI] Day2_1_Agentic Engineering.pdf` p.32 — [도전] PoC 활용하여 Application 개발

## 요구사항 (PDF 원문 요약)

기존에 진행했던 PoC(Proof of Concept, `JsonValue`/`JsonParser`/`JsonWriter`) 코드의 구조와 로직을 참고하여, 데이터를 JSON 파일로 관리하는 CRUD(Create, Read, Update, Delete) 콘솔 애플리케이션을 개발한다.

- PoC에서 사용된 코드 구조를 유지한 상태로, CRUD 구현
- **Create**: 새로운 데이터를 입력 받아 JSON 파일에 저장.
- **Read**: 전체 목록 보기 및 특정 ID/키 값으로 검색 기능.
- **Update**: 기존 데이터를 선택하여 특정 필드 수정.
- **Delete**: 특정 데이터를 안전하게 삭제.

## 구현 내용

PoC 단계(`JsonValue`/`JsonParser`/`JsonWriter`, `docs/phase1~3.md`)의 구조를 그대로 유지하고, 그 위에 CRUD 계층을 추가했다(`docs/phase4~5.md`).

### 인터페이스 우선 설계

`ICrudRepository`에 Create/Read/Update/Delete 네 메서드를 모두 선언해, 저장 방식(파일/DB 등)이 바뀌어도 호출 측 코드(콘솔 앱)는 영향받지 않도록 했다. 실제 구현은 JSON 파일 기반의 `JsonFileRepository` 하나뿐이지만, 다른 저장소 구현으로 교체하더라도 `ICrudRepository`만 바라보는 코드는 그대로 재사용 가능하다.

### 데이터 모델

레코드는 `JsonValue` Object이며, `id`라는 문자열 필드로 식별한다(`idField` 생성자 인자로 필드명 변경 가능). 필드 구성이 고정되어 있지 않아 임의의 키/값 데이터를 저장할 수 있다.

- **Create**: 넘어온 레코드에 `id` 필드가 없으면 기존 레코드들의 최대 숫자 id + 1로 자동 채번한다. 있으면 그대로 사용한다(수동 지정 허용).
- **Read**: `ReadAll()`로 전체 목록을, `ReadById(id)`로 단건을 조회한다(없으면 `nullptr`).
- **Update**: `id`로 레코드를 찾아 전달된 필드들을 기존 레코드에 병합(merge)한다 — 없는 키는 추가, 있는 키는 덮어쓴다. 부분 수정이 가능하다.
- **Delete**: `id`로 레코드를 찾아 배열에서 제거한다.

Create/Update/Delete가 성공할 때마다 즉시 `JsonWriter::SaveToFile`로 파일에 저장하여(pretty-print), 프로세스가 중간에 종료되어도 마지막 변경까지 반영된 상태를 유지한다.

### 콘솔 UI (`main.cpp`, Release 빌드)

```
=== JSON CRUD 콘솔 앱 ===
1. Create - 새 데이터 추가
2. Read   - 조회 (전체 목록 / ID 검색)
3. Update - 데이터 수정
4. Delete - 데이터 삭제
0. 종료
선택:
```

- **Create**: "키=값" 형식으로 한 줄씩 입력받아(빈 줄 입력 시 종료) 새 레코드를 만든다.
- **Read**: 전체 목록 보기 또는 ID로 검색 중 선택한다.
- **Update**: ID를 먼저 확인한 뒤, 수정할 필드를 "키=값" 형식으로 입력받아 병합 적용한다.
- **Delete**: ID 확인 후 `y` 확인을 받아야 삭제한다(오삭제 방지).

Debug 빌드에서는 `main()`이 이 콘솔 앱 대신 gtest 실행기로 동작한다(`docs/phase1.md` Tests 절 참고).

### 테스트

`JsonFileRepositoryTest.cpp`에서 Create(자동/수동 채번, 목록/영속성), Read(전체/단건/미존재), Update(필드 병합/신규 필드 추가/미존재 id/영속성), Delete(삭제/다른 레코드 보존/미존재 id/영속성)를 TDD로 검증했다. 전체 스위트(`JsonValue`/`JsonParser`/`JsonWriter`/`JsonFileRepository`) 합계 61개 테스트가 모두 통과한다.

## 향후 계획

- `JsonParsing` 코드(JsonValue/Parser/Writer/CRUD)를 별도 정적 라이브러리(.lib) 프로젝트로 분리하는 작업은 별도 브랜치에서 진행한다(현재 `master`는 콘솔 앱 단일 프로젝트 구조를 유지).

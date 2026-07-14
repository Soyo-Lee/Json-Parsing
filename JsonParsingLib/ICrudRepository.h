#pragma once

#include "JsonValue.h"

#include <string>

// 저장 방식(파일/DB 등)에 무관하게 호출 측이 의존할 수 있는 CRUD 인터페이스.
class ICrudRepository
{
public:
    virtual ~ICrudRepository() = default;

    // record를 저장한다. record(Object)에 id 필드가 없으면 자동으로 채번하여 채운다.
    // 저장된 레코드(채번된 id 포함)를 반환한다.
    virtual JsonValue Create(JsonValue record) = 0;

    // 저장된 전체 레코드 목록을 반환한다.
    virtual const JsonValue::ArrayType& ReadAll() const = 0;

    // id에 해당하는 레코드를 찾는다. 없으면 nullptr을 반환한다.
    virtual const JsonValue* ReadById(const std::string& id) const = 0;

    // id에 해당하는 레코드에 fields의 각 항목을 병합 적용한다. 레코드가 없으면 false.
    virtual bool Update(const std::string& id, const JsonValue::ObjectType& fields) = 0;

    // id에 해당하는 레코드를 삭제한다. 레코드가 없으면 false.
    virtual bool Delete(const std::string& id) = 0;
};

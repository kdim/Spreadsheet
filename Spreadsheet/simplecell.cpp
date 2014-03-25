#include "stdafx.h"
#include <functional>
#include "simplecell.h"
#include "cellfactory.h"

namespace {

static CellRegisterHelper registernothingcell(1,
  [](const std::wstring &valuestring)->bool {return true; },
  [](const std::wstring &valuestring,
        const CellCollectionWeakPtr &collection,
        const std::wstring &name) {
    return std::make_shared<NothingCell>(valuestring, collection, name);
  });

static CellRegisterHelper registernumcell(2,
  &(NumCell::IsNewValueNumberType),
  [](const std::wstring &valuestring,
        const CellCollectionWeakPtr &collection,
        const std::wstring &name) {
    return std::make_shared<NumCell>(valuestring, collection, name);
  });

static CellRegisterHelper registerstringcell(3,
  &(StrCell::IsNewValueStringType),
  [](const std::wstring &valuestring,
        const CellCollectionWeakPtr &collection,
        const std::wstring &name) {
    return std::make_shared<StrCell>(valuestring, collection, name);
  });

}

const std::wstring NothingCell::Print() {
  return std::wstring();
}

bool NothingCell::OnChangeValue(const std::wstring &new_value) {
  return new_value.compare(L"") == 0;
}

NumCell::NumCell(std::wstring edited_value,
    CellCollectionWeakPtr collection,
    std::wstring name) : 
        CellAbstract(edited_value, collection, name) {
  if (!TryChangeEditedValue(edited_value))
    value_ = NAN;
}

const std::wstring NumCell::Print() {
  return  edited_value();
}

bool NumCell::IsNewValueNumberType(const std::wstring &new_value)
{
  try {
    auto tmp = STR_TO_NUMBER(new_value);
    return true;
  } catch (...) {
    return false;
  }
}
bool NumCell::OnChangeValue(const std::wstring &new_value) {
  try {
    value_ = STR_TO_NUMBER(new_value);
    return true;
  } catch (...) {
    return false;
  }
}

const std::wstring StrCell::Print() {
  auto str = edited_value();
  return std::wstring(str.begin() + 1, str.end());
}

StrCell::StrCell(std::wstring edited_value,
  CellCollectionWeakPtr collection,
  std::wstring name) : 
      CellAbstract(edited_value, collection, name)  {};

bool StrCell::IsNewValueStringType(const std::wstring &new_value)
{
  return new_value[0] == L'\'';
}

bool StrCell::OnChangeValue(const std::wstring &new_value) {
  return IsNewValueStringType(new_value);
}


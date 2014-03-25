#include "stdafx.h"
#include "expressioncell.h"

#include <functional>
#include <sstream>

#include "cellfactory.h"
#include "simplecell.h"

namespace {

  static CellRegisterHelper registernothingcell(4,
    &(ExpressionCell::IsNewValueExpressionType),
    [](const std::wstring &valuestring,
          const CellCollectionWeakPtr &collection,
          const std::wstring &name) {
      return std::make_shared<ExpressionCell>(valuestring, collection, name);
    });
}

ExpressionCell::ExpressionCell(std::wstring edited_value,
    CellCollectionWeakPtr collection,
    std::wstring name) : is_value_calculated_(false),
        CellAbstract(edited_value, collection, name) {
  exprtokens_ = Parse(std::wstring(edited_value.begin() + 1, edited_value.end()));
}

void ExpressionCell::ReplaceThat(CellSharedPtr that) {
  CellAbstract::ReplaceThat(that);
  is_value_calculated_ = false;
}

void ExpressionCell::Recalculate() {
  GetVarByNameFunction getvar;
  if (collection_.use_count() != 0) {
    auto collection_ptr = collection_.lock();
    getvar = [&collection_ptr](const std::wstring &varname) -> ExpressionResult {
      auto cell = collection_ptr->GetCell(varname);
      if (typeid(*cell) == typeid(NumCell))
        return { true, dynamic_cast<NumCell&>(*cell).value(), { ParsingErrorType::kNone } };
      if (typeid(*cell) == typeid(ExpressionCell)) {
        auto value = dynamic_cast<ExpressionCell&>(*cell).value();
        if (value.iscalculated)
          return { true, value.value, { ParsingErrorType::kNone } };
        else
          return { false, NAN, { ParsingErrorType::kVariableHasNoValue } };
      }
      return { false, NAN, { ParsingErrorType::kVariableNotDefined } };
    };
  } else {
    getvar = [](const std::wstring &varname) -> ExpressionResult {
      return { false, NAN, { ParsingErrorType::kNoVariableSourceCollection } };
    };
  }

  is_value_calculated_ = true;
  value_.iscalculated = false;
  value_.parsingerror.type = ParsingErrorType::kRecursion;
  value_ = Calculate(exprtokens_, getvar);
}

bool ExpressionCell::IsNewValueExpressionType(const std::wstring &new_value)
{
  return new_value[0] == L'=';
}

ExpressionResult ExpressionCell::value() {
  if (!is_value_calculated_)
    Recalculate();
  return value_;
}

const std::wstring ExpressionCell::Print() {
  auto currentvalue = value();
  if (currentvalue.iscalculated) {
    return std::to_wstring(currentvalue.value);
  } else {
    return std::wstring(L"#ERROR");
  }
}

bool ExpressionCell::OnChangeValue(const std::wstring &new_value) {
  exprtokens_ = Parse(std::wstring(new_value.begin() + 1, new_value.end()));
  is_value_calculated_ = false;
  return true;
}

void ExpressionCell::OnDependencyCellValueChange(const CellAbstract &source) {
  is_value_calculated_ = false;
}
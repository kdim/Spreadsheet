// expressioncell.h
#pragma once

#include "cell.h"
#include "expression.h"

class ExpressionCell : public CellAbstract
{
public:
  ExpressionCell(std::wstring edited_value,
    CellCollectionWeakPtr collection = std::weak_ptr<CellCollection>(),
    std::wstring name = L"");

  const std::wstring Print() override;
  ExpressionResult value();

  void ReplaceThat(CellSharedPtr that) override;

  static bool IsNewValueExpressionType(const std::wstring &new_value);
protected:
  bool OnChangeValue(const std::wstring &new_value) override;
  void OnDependencyCellValueChange(const CellAbstract &source) override;
  void Recalculate();
private:
  bool is_value_calculated_;
  ExpressionResult value_;
  Expressions exprtokens_;
};


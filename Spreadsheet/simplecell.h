// simplecell.h
#pragma once

#include "cell.h"

class NothingCell : public CellAbstract
{
public:
  NothingCell(std::wstring edited_value = L"",
    CellCollectionWeakPtr collection = std::weak_ptr<CellCollection>(),
    std::wstring name = L"") : CellAbstract(edited_value, collection, name)  {};

  const std::wstring Print() override;
  bool OnChangeValue(const std::wstring &new_value) override;
protected:
private:
};

class NumCell : public CellAbstract
{
public:
  NumCell(std::wstring edited_value = L"",
    CellCollectionWeakPtr collection = std::weak_ptr<CellCollection>(),
    std::wstring name = L"");
  NUMBER value() { return value_; }
  const std::wstring Print() override;
  static bool IsNewValueNumberType(const std::wstring &new_value);
protected:
  bool OnChangeValue(const std::wstring &new_value) override;
private:
  NUMBER value_;
};

class StrCell : public CellAbstract
{
public:
  StrCell(std::wstring edited_value = L"",
    CellCollectionWeakPtr collection = std::weak_ptr<CellCollection>(),
    std::wstring name = L"");
  const std::wstring Print() override;
  static bool IsNewValueStringType(const std::wstring &new_value);
protected:
  bool OnChangeValue(const std::wstring &new_value) override;
private:
};


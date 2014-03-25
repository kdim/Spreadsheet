// cellfactory.h
#pragma once

#include <vector>
#include <utility>
#include <memory>
#include <string>
#include <functional>

#include "cell.h"

typedef std::function<bool(const std::wstring &valuestring)> IsStringThisTypeFunction;
typedef bool(*IsStringThisTypePtr)(const std::wstring &valuestring);
typedef std::function<CellSharedPtr(const std::wstring &valuestring,
  const CellCollectionWeakPtr &collection,
  const std::wstring &name)> MakeSharedCellFromStringFunction;

struct CellBuildingInfo {
  int priority; // bad. Should I make "before/after that type" sorting?
  IsStringThisTypeFunction isthistype;
  MakeSharedCellFromStringFunction makesharedfromsting;
};

class CellFactory
{
public:
  CellFactory(const CellFactory &copysource) = delete;
  CellFactory(CellFactory &&movesource) = delete;
  CellFactory& operator=(CellFactory &arg) = delete;

  void RegisterCellType(CellBuildingInfo&& cellbuildinfo);
  CellSharedPtr MakeCellSharedPtr(std::wstring valuestring, 
    const CellCollectionWeakPtr &collection = std::weak_ptr<CellCollection>(), 
    const std::wstring &name = L"");
  CellSharedPtr ReplaceCellSharedPtr(CellSharedPtr original,
    const std::wstring &valuestring);
private:
  friend CellFactory &GetCellFactory();
  CellFactory() {};
  std::vector<CellBuildingInfo> buildinfo_;
};

class CellRegisterHelper
{
public:
  explicit CellRegisterHelper(CellBuildingInfo&& cellbuildinfo);
  CellRegisterHelper(int priority,
    IsStringThisTypeFunction&& isthistype,
    MakeSharedCellFromStringFunction&& makesharedfromsting);
};

CellFactory &GetCellFactory();

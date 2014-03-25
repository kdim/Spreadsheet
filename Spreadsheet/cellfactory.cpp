#include "stdafx.h"
#include "cellfactory.h"

#include <algorithm>

#include "simplecell.h"

CellFactory &GetCellFactory()
{
  static CellFactory* cellfactory = new CellFactory();
  return *cellfactory;
}

void CellFactory::RegisterCellType(CellBuildingInfo && cellbuildinfo)
{
  auto placetoinsert = std::find_if(buildinfo_.begin(), buildinfo_.end(),
    [&cellbuildinfo](const CellBuildingInfo &item) {return item.priority < cellbuildinfo.priority; });
  buildinfo_.insert(placetoinsert, cellbuildinfo);

}

CellSharedPtr CellFactory::MakeCellSharedPtr(std::wstring valuestring,
    const CellCollectionWeakPtr &collection,
    const std::wstring &name)
{
  for (auto &cellinfo : buildinfo_)
  {
    if (cellinfo.isthistype(valuestring))
      return cellinfo.makesharedfromsting(valuestring, collection, name);
  }
  return std::shared_ptr<NothingCell>();
}

CellSharedPtr CellFactory::ReplaceCellSharedPtr(CellSharedPtr original, 
    const std::wstring &valuestring)
{
  for (auto &cellinfo : buildinfo_)
  {
    if (cellinfo.isthistype(valuestring)) {
      auto result = cellinfo.makesharedfromsting(valuestring, std::weak_ptr<CellCollection>(), L"");
      result->ReplaceThat(original);
      return result;
    }
  }
  return std::shared_ptr<NothingCell>();
}

CellRegisterHelper::CellRegisterHelper(CellBuildingInfo&& cellbuildinfo)
{
  GetCellFactory().RegisterCellType(std::forward<CellBuildingInfo>(cellbuildinfo));
}

CellRegisterHelper::CellRegisterHelper(int priority,
  IsStringThisTypeFunction&& isthistype,
  MakeSharedCellFromStringFunction&& makesharedfromsting)
{
  CellBuildingInfo cellbuildinfo = { priority, isthistype, makesharedfromsting };
  GetCellFactory().RegisterCellType(std::forward<CellBuildingInfo>(cellbuildinfo));
}

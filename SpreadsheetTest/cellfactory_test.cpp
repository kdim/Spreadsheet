
#include "stdafx.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <type_traits>

#include "../Spreadsheet/cell.h"
#include "../Spreadsheet/simplecell.h"
#include "../Spreadsheet/cellfactory.h"

using ::testing::_;
using std::wstring;
using std::get;

TEST(CellFactoryTest, NothingType) {
  NothingCell nothingcell(L"1", std::weak_ptr<CellCollection>(), L"A1");
  EXPECT_STRCASEEQ(typeid(*GetCellFactory().MakeCellSharedPtr(L"")).name(), typeid(nothingcell).name());
  EXPECT_STRCASENE(typeid(*GetCellFactory().MakeCellSharedPtr(L"1")).name(), typeid(nothingcell).name());
  EXPECT_STRCASENE(typeid(*GetCellFactory().MakeCellSharedPtr(L"-1")).name(), typeid(nothingcell).name());
  EXPECT_STRCASENE(typeid(*GetCellFactory().MakeCellSharedPtr(L"'abc")).name(), typeid(nothingcell).name());
  EXPECT_STRCASENE(typeid(*GetCellFactory().MakeCellSharedPtr(L"' abc")).name(), typeid(nothingcell).name());
  EXPECT_STRCASEEQ(typeid(*GetCellFactory().MakeCellSharedPtr(L"abc")).name(), typeid(nothingcell).name());
}

TEST(CellFactoryTest, NumType) {
  NumCell numcell(L"1", std::weak_ptr<CellCollection>(), L"A1");
  EXPECT_STRCASENE(typeid(*GetCellFactory().MakeCellSharedPtr(L"")).name(), typeid(numcell).name());
  EXPECT_STRCASEEQ(typeid(*GetCellFactory().MakeCellSharedPtr(L"1")).name(), typeid(numcell).name());
  EXPECT_STRCASEEQ(typeid(*GetCellFactory().MakeCellSharedPtr(L"-1")).name(), typeid(numcell).name());
  EXPECT_STRCASEEQ(typeid(*GetCellFactory().MakeCellSharedPtr(L" 1 ")).name(), typeid(numcell).name());
  EXPECT_STRCASEEQ(typeid(*GetCellFactory().MakeCellSharedPtr(L" 1 abc")).name(), typeid(numcell).name());
  EXPECT_STRCASENE(typeid(*GetCellFactory().MakeCellSharedPtr(L"abc")).name(), typeid(numcell).name());
}

TEST(CellFactoryTest, StrType) {
  StrCell strcell(L"1", std::weak_ptr<CellCollection>(), L"A1");
  EXPECT_STRCASENE(typeid(*GetCellFactory().MakeCellSharedPtr(L"")).name(), typeid(strcell).name());
  EXPECT_STRCASEEQ(typeid(*GetCellFactory().MakeCellSharedPtr(L"'abc")).name(), typeid(strcell).name());
  EXPECT_STRCASEEQ(typeid(*GetCellFactory().MakeCellSharedPtr(L"' abc")).name(), typeid(strcell).name());
  EXPECT_STRCASEEQ(typeid(*GetCellFactory().MakeCellSharedPtr(L"' abc ")).name(), typeid(strcell).name());
  EXPECT_STRCASEEQ(typeid(*GetCellFactory().MakeCellSharedPtr(L"'1 abc")).name(), typeid(strcell).name());
  EXPECT_STRCASENE(typeid(*GetCellFactory().MakeCellSharedPtr(L"abc")).name(), typeid(strcell).name());
}


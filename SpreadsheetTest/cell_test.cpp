#include "stdafx.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define TEST_DEFINITION

#include "../Spreadsheet/cell.h"
#include "../Spreadsheet/simplecell.h"

#include "mock_cell.h"

using ::testing::_;
using std::wstring;
using std::get;

TEST(CellTest, Name) {
  auto cell_a = std::make_shared<NothingCell>(L"200", std::weak_ptr<CellCollection>(), L"B2");
  auto name = cell_a->name();
  name += L" ha ha ha";
  EXPECT_STREQ(cell_a->name().c_str(), L"B2");
}

TEST(CellTest, AddDependencyLink) {
  auto cell_a = std::make_shared<MockCellAbstract>();
  auto cell_b = std::make_shared<NothingCell>(L"200", std::weak_ptr<CellCollection>(), L"B2");

  EXPECT_CALL(*(cell_a.get()), AddNotifyLink(_, _));

  cell_b->AddDependencyLink(cell_a);
}

TEST(CellTest, OnDependencyCellDestroy) {
  auto cell_b = std::make_shared<NothingCell>(L"200", std::weak_ptr<CellCollection>(), L"B2");
  auto cell_c = std::make_shared<MockCellAbstract>();

  EXPECT_CALL(*(cell_c.get()), OnDependencyCellDestroy(_));

  cell_b->AddNotifyLink(cell_c);

  cell_b.reset();
}

TEST(CellTest, FilterLostPointerForDependencyAfterDeletionOfDependency) {
  auto cell_a1 = std::make_shared<MockCellAbstract>();
  auto cell_a2 = std::make_shared<MockCellAbstract>();
  auto cell_b = std::make_shared<NothingCell>(L"200", std::weak_ptr<CellCollection>(), L"B2");

  EXPECT_CALL(*(cell_a1.get()), AddNotifyLink(_, _));
  EXPECT_CALL(*(cell_a2.get()), AddNotifyLink(_, _));

  cell_b->AddDependencyLink(cell_a1);
  cell_b->AddDependencyLink(cell_a2);

  auto ptr_a1 = cell_a1.get();
  cell_a1.reset();
  cell_b->OnDependencyCellDestroy(*ptr_a1);

  EXPECT_EQ(cell_b->dependency_list_.size(), 1);
  EXPECT_EQ(get<0>(*cell_b->dependency_list_.begin()).lock().get(), cell_a2.get());
}

TEST(CellTest, FilterLostPointerForNotifyAndValueChange) {
  auto cell_b = std::make_shared<NumCell>(L"200", std::weak_ptr<CellCollection>(), L"B2");
  auto cell_c1 = std::make_shared<MockCellAbstract>();
  auto cell_c2 = std::make_shared<MockCellAbstract>();

  EXPECT_CALL(*(cell_c2.get()), OnDependencyCellValueChange(_));
  EXPECT_CALL(*(cell_c2.get()), OnDependencyCellDestroy(_));

  cell_b->AddNotifyLink(cell_c1);
  cell_b->AddNotifyLink(cell_c2);
  cell_c1.reset();

  cell_b->set_edited_value(L"300");

  EXPECT_EQ(cell_b->edited_value(), L"300");

  cell_b.reset();
}

TEST(CellTest, FilterLostPointerForNotifyAndNameChange) {
  auto cell_b = std::make_shared<NumCell>(L"200", std::weak_ptr<CellCollection>(), L"B2");
  auto cell_c1 = std::make_shared<MockCellAbstract>();
  auto cell_c2 = std::make_shared<MockCellAbstract>();

  EXPECT_CALL(*(cell_c2.get()), OnDependencyCellNameChange(_, std::wstring(L"B2"), std::wstring(L"C3")));
  EXPECT_CALL(*(cell_c2.get()), OnDependencyCellDestroy(_));

  cell_b->AddNotifyLink(cell_c1);
  cell_b->AddNotifyLink(cell_c2);
  cell_c1.reset();

  EXPECT_EQ(cell_b->name(), L"B2");
  EXPECT_EQ(cell_b->edited_value(), L"200");
  cell_b->set_name(L"C3");
  EXPECT_EQ(cell_b->name(), L"C3");

  cell_b.reset();
}


